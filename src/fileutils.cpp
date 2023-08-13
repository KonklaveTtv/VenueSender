#include "fileutils.h"

// Function to trim leading and trailing spaces from a string
std::string trim(const std::string& str){
    // Trimming function
    const auto notSpace = [](int ch) {return !std::isspace(ch); };
    auto first = std::find_if(str.begin(), str.end(), notSpace);
    auto last = std::find_if(str.rbegin(), str.rend(), notSpace).base();
    return (first < last ? std::string(first, last) : std::string());
}

// Clear the input buffer
void clearInputBuffer() {
    // Clear the input buffer
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to read venue data from CSV file
void readCSV(std::vector<Venue>& venues, const std::string& venuesCsvPath) {
    std::ifstream file(venuesCsvPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open CSV file: " << venuesCsvPath << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string data;
        std::vector<std::string> rowData;

        while (std::getline(ss, data, ',')) {
            rowData.push_back(trim(data));
        }

        if (rowData.size() == 6) {
            Venue venue;
            venue.name = rowData[0];
            venue.email = rowData[1];
            venue.genre = rowData[2];
            venue.state = rowData[3];
            venue.city = rowData[4];
            try {
            venue.capacity = std::stoi(rowData[5]);
            } catch (const std::exception& ex) {
                std::cerr << "Invalid capacity in CSV file:" << venuesCsvPath << std::endl;
            }
            venues.push_back(venue);
        } else {
            std::cerr << "Invalid data in CSV file: " << venuesCsvPath << std::endl;
        }
    }

    file.close();
}

// Function to load the settings config.json data and encrypt and decrypt email/smtp passwords
bool loadConfigSettings(const std::string& configFilePath,
                        std::string& smtpServer, int& smtpPort,
                        std::string& smtpUsername, std::string& smtpPass,
                        std::string& venuesCsvPath, std::string& emailPass,
                        std::string& senderEmail, int& senderSmtpPort) {
    std::array<unsigned char, crypto_secretbox_KEYBYTES> encryptionKey;
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> encryptionNonce;
    initializeEncryptionParams(encryptionKey, encryptionNonce);

    // Load configuration settings from config.json into respective variables
    // Return true if successful, false otherwise
    Json::Value config;
    std::ifstream configFile(configFilePath);

    if (!configFile.is_open()) {
        std::cerr << "Failed to open config.json." << std::endl;
        return false;
    }

    configFile >> config;

    smtpServer = config["smtp_server"].asString();
    smtpPort = config["smtp_port"].asInt();
    smtpUsername = config["smtp_username"].asString();

    // Load encrypted passwords from config.json
    std::string smtpPassEncrypted = config["smtp_password"].asString();
    std::string emailPassEncrypted = config["email_password"].asString();

    venuesCsvPath = config["venues_csv_path"].asString();
    senderEmail = config["sender_email"].asString();
    senderSmtpPort = config["sender_smtp_port"].asInt();

    // SMTP/Mail Password Decryption Check
    std::string smtpPassDecrypted;
    std::string emailPassDecrypted;

    bool isSmtpPassEncrypted = config.isMember("smtp_pass_encrypted") ? config["smtp_pass_encrypted"].asBool() : false;
    bool isEmailPassEncrypted = config.isMember("email_pass_encrypted") ? config["email_pass_encrypted"].asBool() : false;

    if (isSmtpPassEncrypted) {
        if (!decryptPassword(smtpPassEncrypted, smtpPassDecrypted, encryptionKey, encryptionNonce)) {
            std::cerr << "SMTP password decryption failed." << std::endl;
            return false;
        }
        // Additional checks/validation on smtpPassDecrypted
    }

    if (isEmailPassEncrypted) {
        if (!decryptPassword(emailPassEncrypted, emailPassDecrypted, encryptionKey, encryptionNonce)) {
            std::cerr << "Email password decryption failed." << std::endl;
            return false;
        }
        // Additional checks/validation on emailPassDecrypted
    }

    // End of SMTP/Mail Password Decryption Check

    // Define and initialize variables to track loaded settings
    bool smtpServerLoaded = !smtpServer.empty();
    bool smtpPortLoaded = smtpPort > 0;
    bool smtpUsernameLoaded = !smtpUsername.empty();
    bool smtpPassLoaded = !smtpPass.empty();
    bool venuesCsvPathLoaded = !venuesCsvPath.empty();
    bool emailPassLoaded = !emailPass.empty();
    bool senderEmailLoaded = !senderEmail.empty();
    bool senderSmtpPortLoaded = senderSmtpPort > 0;

    // Check if the configuration settings are loaded successfully
    bool configLoadedSuccessfully = smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded &&
                                    smtpPassLoaded && venuesCsvPathLoaded && emailPassLoaded &&
                                    senderEmailLoaded && senderSmtpPortLoaded;

    // Display messages based on loaded settings
    if (smtpServerLoaded || smtpPortLoaded || smtpUsernameLoaded || smtpPassLoaded || 
        venuesCsvPathLoaded || emailPassLoaded || senderEmailLoaded || senderSmtpPortLoaded) {
        std::cout << "Configuration settings loaded from config.json." << std::endl;
        configLoadedSuccessfully = true;
    } else if (smtpServerLoaded || smtpPortLoaded || smtpPassLoaded || senderEmailLoaded || senderSmtpPortLoaded) {
        std::cout << "Email settings loaded from config.json." << std::endl;
    } else if (!configLoadedSuccessfully) {
        std::cerr << "Failed to load configuration settings from config.json." << std::endl;
    }

    // Encrypt the passwords if they are not already encrypted
    if (!isSmtpPassEncrypted) {
        std::string smtpPassEncryptedNew;
        if (!encryptPassword(smtpPass, smtpPassEncryptedNew, encryptionKey, encryptionNonce)) {
            std::cerr << "Failed to encrypt SMTP password for saving in config.json." << std::endl;
            return false;
        }
        config["smtp_password"] = smtpPassEncryptedNew;
        config["smtp_pass_encrypted"] = true;
    }

    if (!isEmailPassEncrypted) {
        std::string emailPassEncryptedNew;
        if (!encryptPassword(emailPass, emailPassEncryptedNew, encryptionKey, encryptionNonce)) {
            std::cerr << "Failed to encrypt email password for saving in config.json." << std::endl;
            return false;
        }
        config["email_password"] = emailPassEncryptedNew;
        config["email_pass_encrypted"] = true;
    }

    // Open the config file for writing and save the updated JSON object
    std::ofstream configFileOut("config.json");
    if (!configFileOut.is_open()) {
        std::cerr << "Failed to open config.json for writing." << std::endl;
        return false;
    }
    configFileOut << config;
    configFileOut.close();

    return configLoadedSuccessfully;
}

// Function to reset flags and passwords in config.json
void resetConfigFile(const std::string& configFilePath) {
    Json::Value config;

    // Read the existing config.json
    std::ifstream configFile(configFilePath);
    configFile >> config;
    configFile.close();

    // Modify the values in the JSON object
    config["smtp_pass_encrypted"] = false;
    config["email_pass_encrypted"] = false;
    config["smtp_password"] = "enter_smtp_password";
    config["email_password"] = "enter_email_password";

    // Write the modified JSON object back to config.json
    std::ofstream configFileOut(configFilePath);
    configFileOut << config;
    configFileOut.close();
}