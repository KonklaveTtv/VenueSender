#include "include/fileutils.h"

#include "errorhandler.h" // Include here due to circular dependency between fileutils.h and errorhandler.h

// Use the standard namespace
using namespace std;

// Namespace to hold configuration file paths
namespace confPaths {
string venuesCsvPath = "venues.csv";
string configJsonPath = "config.json";
string mockVenuesCsvPath = "src/test/mock_venues.csv";
string mockConfigJsonPath = "src/test/mock_config.json";
}

// Function to trim leading and trailing spaces from a string
string ConsoleUtils::trim(const string& str){
    // Trimming function
    const auto notSpace = [](int ch) {return !isspace(ch); };
    auto first = find_if(str.begin(), str.end(), notSpace);
    auto last = find_if(str.rbegin(), str.rend(), notSpace).base();
    return (first < last ? string(first, last) : string());
}

void ConsoleUtils::clearConsole() {
    // Platform-specific code to clear the console
    // Windows:
#ifdef _WIN32
    system("cls");
#else 
    // Mac/Linux:
    system("clear");
#endif
}

// Default constructor for ConfigManager
ConfigManager::ConfigManager() = default;

// Function to load settings from a configuration file
bool ConfigManager::loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost, bool& verbose, 
                                       string& senderEmail, string& smtpUsername, 
                                       string& mailPass, int& smtpPort, string& smtpServer, 
                                       string& venuesCsvPath) {

    // Logic to read and load settings from a JSON file
    // Includes conditional compilation for unit testing
    // Handles encryption and decryption of email passwords
    // Validates the loaded settings and returns a boolean flag to indicate success or failure
#ifdef UNIT_TESTING

    Json::Value config;
    ifstream configFile(confPaths::mockConfigJsonPath);
    if (!configFile.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR);
        return false;
    }

    configFile >> config;

    // Load smtp user settings
    senderEmail = config["mock_sender_email"].asString();
    smtpPort = config["mock_smtp_port"].asInt();
    smtpServer = config["mock_smtp_server"].asString();
    smtpUsername = config["mock_smtp_username"].asString();

    // Load SSL settings
    useSSL = config["useSSL"].asBool();
    verifyPeer = config["verifyPeer"].asBool();
    verifyHost = config["verifyHost"].asBool();

    // Enable verbose for cURL
    verbose = config["verbose"].asBool();
#else
    Json::Value config;
    ifstream configFile(confPaths::configJsonPath);
    if (!configFile.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR);
        return false;
    }

    configFile >> config;

    // Load smtp user settings
    senderEmail = config["sender_email"].asString();
    smtpPort = config["smtp_port"].asInt();
    smtpServer = config["smtp_server"].asString();
    smtpUsername = config["smtp_username"].asString();

    // Load SSL settings
    useSSL = config["useSSL"].asBool();
    verifyPeer = config["verifyPeer"].asBool();
    verifyHost = config["verifyHost"].asBool();

    // Enable verbose for cURL
    verbose = config["verbose"].asBool();
#endif

// SMTP/Mail Encryption/Decryption

#ifdef UNIT_TESTING

    // Load plain text passwords from mock_config.json
    mailPass = config["mock_email_password"].asString();

    // SMTP/Mail Encryption Check
    bool ismailPassEncrypted = config.isMember("mock_email_pass_encrypted") ? config["mock_email_pass_encrypted"].asBool() : false;

    string mailPassEncrypted;

    // Encrypt the mail password if it is not already encrypted and update the config file
    if (!ismailPassEncrypted) {
        if (!EncryptionManager::encryptPassword(mailPass, mailPassEncrypted)) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ENCRYPTION_ERROR);
            return false;
        }
        config["mock_email_password"] = mailPassEncrypted;
        config["mock_email_pass_encrypted"] = true;
    } else {
        mailPassEncrypted = config["mock_email_password"].asString();
    }
#else
    // Load plain text passwords from config.json
    mailPass = config["email_password"].asString();

    // SMTP/Mail Encryption Check
    bool ismailPassEncrypted = config.get("email_pass_encrypted", false).asBool();

    string mailPassEncrypted;

    // Encrypt the mail password if it is not already encrypted and update the config file
    if (!ismailPassEncrypted) {
        if (!EncryptionManager::encryptPassword(mailPass, mailPassEncrypted)) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ENCRYPTION_ERROR);
            return false;
        }
        config["email_password"] = mailPassEncrypted;
        config["email_pass_encrypted"] = true;
    } else {
        mailPassEncrypted = config["email_password"].asString();
    }
#endif

    // Open the config file for writing and save the updated JSON object
#ifdef UNIT_TESTING

    ofstream configFileOut(confPaths::mockConfigJsonPath);
    if (!configFileOut.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_TO_WRITE_ERROR);
        return false;
    }
    configFileOut << config;
    configFileOut.close();

    // Reassign encrypted passwords for decryption and reassignment
    mailPass = config["mock_email_password"].asString();

    // SMTP/Mail Password Decryption Check
    string mailPassDecrypted;

    // Decrypt the encrypted mail password
    mailPassDecrypted = EncryptionManager::decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ENCRYPTION_ERROR);
        return false;
    }
#else
    ofstream configFileOut(confPaths::configJsonPath);
    if (!configFileOut.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_TO_WRITE_ERROR);
        return false;
    }
    configFileOut << config;
    configFileOut.close();

    // Reassign encrypted passwords for decryption and reassignment
    mailPass = config["email_password"].asString();

    // Decrypt the encrypted mail password
    string mailPassDecrypted = EncryptionManager::decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_DECRYPTION_ERROR);
        return false;
    }
#endif
// End of SMTP/Mail Password Encryption/Decryption


    // Define and initialize variables to track loaded settings
    bool smtpServerLoaded = !smtpServer.empty();
    bool smtpPortLoaded = smtpPort > 0;
    bool smtpUsernameLoaded = !smtpUsername.empty();
    bool venuesCsvPathLoaded = !venuesCsvPath.empty();
    bool mailPassLoaded = !mailPass.empty();
    bool senderEmailLoaded = !senderEmail.empty();

    // Check if the configuration settings are loaded successfully
    bool configLoadedSuccessfully = smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded &&
                                    venuesCsvPathLoaded && mailPassLoaded && senderEmailLoaded;

    // Display messages based on loaded settings
    if (smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded && venuesCsvPathLoaded && mailPassLoaded && senderEmailLoaded) {
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN); // Green for success
        cout << "Configuration settings loaded from config.json." << endl;
        ConsoleUtils::resetColor(); // Reset color
        configLoadedSuccessfully = true;
    } else if (smtpServerLoaded || smtpPortLoaded || mailPassLoaded || senderEmailLoaded) {
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE); // Yellow for partial success
        cout << "Email settings loaded from config.json." << endl;
        ConsoleUtils::resetColor(); // Reset color
    } else {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED); // Red for error
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        ConsoleUtils::resetColor(); // Reset color
    }
    return configLoadedSuccessfully;
}

// Function to reset flags and passwords in the configuration file
void ConfigManager::resetConfigFile() {
    // Logic to read the existing JSON configuration
    // Modify specific keys and values
    // Write the updated JSON back to the file
    Json::Value config;

    string configPath = confPaths::configJsonPath;
    string emailPasswordKey = "email_password";
    string isEmailPassEncryptedKey = "email_pass_encrypted";

#ifdef UNIT_TESTING
    configPath = confPaths::mockConfigJsonPath;
    emailPasswordKey = "mock_email_password";
    isEmailPassEncryptedKey = "mock_email_pass_encrypted";
#endif

    // Read the existing configuration
    ifstream configFile(configPath);
    if (!configFile.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR, configPath);
        return;
    }
    configFile >> config;
    configFile.close();

    // Modify the values in the JSON object
    config[isEmailPassEncryptedKey] = false;
    config[emailPasswordKey] = (configPath == confPaths::mockConfigJsonPath) ? "mock_email_password" : "enter_email_password";

    // Write the modified JSON object back to the appropriate file
    ofstream configFileOut(configPath);
    if (!configFileOut.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_TO_WRITE_ERROR, configPath);
        return;
    }
    configFileOut << config;
    configFileOut.close();
}
