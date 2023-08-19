#include "fileutils.h"

using namespace std;

namespace confPaths {
std::string venuesCsvPath = "venues.csv";
std::string configJsonPath = "config.json";
std::string mockVenuesCsvPath = "src/test/mock_venues.csv";
std::string mockConfigJsonPath = "src/test/mock_config.json";
}

// Function to trim leading and trailing spaces from a string
string ConsoleUtils::trim(const string& str){
    // Trimming function
    const auto notSpace = [](int ch) {return !isspace(ch); };
    auto first = find_if(str.begin(), str.end(), notSpace);
    auto last = find_if(str.rbegin(), str.rend(), notSpace).base();
    return (first < last ? string(first, last) : string());
}

// Clear the input buffer
void ConsoleUtils::clearInputBuffer() {
    // Clear the input buffer
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
}

// Function to clear the console screen
void ConsoleUtils::clearConsole() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For other platforms
#endif
}

// Function to read venue data from CSV file
void CsvReader::readCSV(vector<Venue>& venues, string& venuesCsvPath) {
    ifstream file(venuesCsvPath);
    if (!file.is_open()) {
        cerr << "Failed to open CSV file: " << venuesCsvPath << endl;
        return;
    }

    string line;
    getline(file, line); // Skip the header line

    while (getline(file, line)) {
        istringstream ss(line);
        string data;
        vector<string> rowData;

        while (getline(ss, data, ',')) {
            rowData.push_back(ConsoleUtils::trim(data));
        }

        if (rowData.size() == 6) {
            Venue venue;
            venue.name = rowData[0];
            venue.email = rowData[1];
            venue.genre = rowData[2];
            venue.state = rowData[3];
            venue.city = rowData[4];
            try {
            venue.capacity = stoi(rowData[5]);
            } catch (const exception& ex) {
                cerr << "Invalid capacity in CSV file:" << venuesCsvPath << endl;
            }
            venues.push_back(venue);
        } else {
            cerr << "Invalid data in CSV file: " << venuesCsvPath << endl;
        }
    }

    file.close();
}

ConfigManager::ConfigManager() {}

// Function to load the settings config.json data and encrypt and decrypt email password
bool ConfigManager::loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost, bool& verbose, 
                                       std::string& senderEmail, std::string& smtpUsername, 
                                       std::string& mailPass, int& smtpPort, std::string& smtpServer, 
                                       std::string& venuesCsvPath) {

    // Load configuration settings from config.json into respective variables
    // Return true if successful, false otherwise
#ifdef UNIT_TESTING
    Json::Value config;
    ifstream configFile(confPaths::mockConfigJsonPath);
    if (!configFile.is_open()) {
        cerr << "Failed to open config.json." << endl;
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
        cerr << "Failed to open config.json." << endl;
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
        if (!encryptionManager.encryptPassword(mailPass, mailPassEncrypted)) {
            cerr << "Failed to encrypt email password for saving in config.json." << endl;
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
            cerr << "Failed to encrypt email password for saving in config.json." << endl;
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
        cerr << "Failed to open config.json for writing." << endl;
        return false;
    }
    configFileOut << config;
    configFileOut.close();

    // Reassign encrypted passwords for decryption and reassignment
    mailPass = config["mock_email_password"].asString();

    // SMTP/Mail Password Decryption Check
    string mailPassDecrypted;

    // Decrypt the encrypted mail password
    mailPassDecrypted = encryptionManager.decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        cerr << "Email password decryption failed." << endl;
        return false;
    }
#else
    ofstream configFileOut(confPaths::configJsonPath);
    if (!configFileOut.is_open()) {
        cerr << "Failed to open config.json for writing." << endl;
        return false;
    }
    configFileOut << config;
    configFileOut.close();

    // Reassign encrypted passwords for decryption and reassignment
    mailPass = config["email_password"].asString();

    // SMTP/Mail Password Decryption Check
    string mailPassDecrypted;

    // Decrypt the encrypted mail password
    mailPassDecrypted = EncryptionManager::decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        cerr << "Email password decryption failed." << endl;
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
        cout << "Configuration settings loaded from config.json." << endl;
        configLoadedSuccessfully = true;
    } else if (smtpServerLoaded || smtpPortLoaded || mailPassLoaded || senderEmailLoaded) {
        cout << "Email settings loaded from config.json." << endl;
    } else {
        cerr << "Failed to load configuration settings from config.json." << endl;
    }
    return configLoadedSuccessfully;
}

// Function to reset flags and passwords in config.json
void ConfigManager::resetConfigFile() {
    Json::Value config;

    std::string configPath = confPaths::configJsonPath;
    std::string emailPasswordKey = "email_password";
    std::string isEmailPassEncryptedKey = "email_pass_encrypted";

#ifdef UNIT_TESTING
    configPath = confPaths::mockConfigJsonPath;
    emailPasswordKey = "mock_email_password";
    isEmailPassEncryptedKey = "mock_email_pass_encrypted";
#endif

    // Read the existing configuration
    ifstream configFile(configPath);
    if (!configFile.is_open()) {
        cerr << "Failed to open " << configPath << "." << endl;
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
        cerr << "Failed to open " << configPath << " for writing." << endl;
        return;
    }
    configFileOut << config;
    configFileOut.close();
}
