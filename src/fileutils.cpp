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
bool loadConfigSettings(std::string& smtpServer, int& smtpPort,
                        std::string& smtpUsername, std::string& smtpPass,
                        std::string& venuesCsvPath, std::string& emailPassword,
                        std::string& senderEmail, int& senderSmtpPort) {
    // Load configuration settings from config.json into respective variables
    // Return true if successful, false otherwise
    Json::Value config;
    std::ifstream configFile("config.json");

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


// SMTP/Mail Password Encryption Check

    // Initialize encryption key and nonce
    std::array<unsigned char, crypto_secretbox_KEYBYTES> encryptionKey;
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> encryptionNonce;
    initializeEncryptionParams(encryptionKey, encryptionNonce);

    std::string smtpPassDecrypted;
    std::string emailPassDecrypted;

    // Check if the SMTP password and email password are encrypted in the config file
    bool isSmtpPassEncrypted = config.isMember("smtp_pass_encrypted") ? config["smtp_pass_encrypted"].asBool() : false;
    bool isEmailPassEncrypted = config.isMember("email_pass_encrypted") ? config["email_pass_encrypted"].asBool() : false;

    // Generate the expected identifier based on encryption key and nonce
    std::array<unsigned char, crypto_secretbox_MACBYTES> expectedIdentifier;
    generateIdentifier(encryptionKey.data(), reinterpret_cast<const char*>(encryptionNonce.data()), expectedIdentifier);

    // Check if the SMTP password is encrypted
    if (isSmtpPassEncrypted) {
        // Declare an array to store the identifier of the decrypted SMTP password
        std::array<unsigned char, crypto_secretbox_MACBYTES> smtpPassIdentifier;
        
        // Decrypt the SMTP password using the encryption key
        decryptPassword(smtpPassEncrypted, smtpPass, encryptionKey);
        
        // Generate the identifier for the decrypted SMTP password
        generateIdentifier(encryptionKey.data(), smtpPass.c_str(), smtpPassIdentifier);
        
        // Compare the identifiers to check for mismatch
        if (!isSmtpPassEncrypted || smtpPassIdentifier != expectedIdentifier) {
            std::cerr << "Password decryption failed. Set 'smtp_pass_encrypted' & 'email_pass_encrypted' to false and re-enter both passwords (config.json)." << std::endl;
            return false;
        }
    }

    // Check if the email password is encrypted
    if (isEmailPassEncrypted) {
        // Declare an array to store the identifier of the decrypted email password
        std::array<unsigned char, crypto_secretbox_MACBYTES> emailPassIdentifier;
        
        // Decrypt the email password using the encryption key
        decryptPassword(emailPassEncrypted, emailPassword, encryptionKey);
        
        // Generate the identifier for the decrypted email password
        generateIdentifier(encryptionKey.data(), emailPassword.c_str(), emailPassIdentifier);
        
        // Compare the identifiers to check for mismatch
        if (!isEmailPassEncrypted || emailPassIdentifier != expectedIdentifier) {
            std::cerr << "Password decryption failed. Set 'smtp_pass_encrypted' & 'email_pass_encrypted' to false and re-enter both passwords (config.json)." << std::endl;
            return false;
        }
    }

    if (isSmtpPassEncrypted || isEmailPassEncrypted) {
            std::cerr << "Password decryption failed. Set 'smtp_pass_encrypted' & 'email_pass_encrypted' to false and re-enter both passwords (config.json)." << std::endl;
            return false;
    }

    smtpPass = smtpPassEncrypted;
    if (encryptPassword(smtpPass, smtpPassEncrypted, encryptionKey)) {
        config["smtp_password"] = smtpPassEncrypted;
        config["smtp_pass_encrypted"] = true; // Mark the password as encrypted
        std::cout << "Passwords encrypted successfully." << std::endl;
    } else {
        std::cerr << "Password encryption failed. Set 'smtp_pass_encrypted' & 'email_pass_encrypted' to false and re-enter both passwords (config.json)." << std::endl;
        return false;
    }

    emailPassword = emailPassEncrypted;
    if (encryptPassword(emailPassword, emailPassEncrypted, encryptionKey)) {
        config["email_password"] = emailPassEncrypted;
        config["email_pass_encrypted"] = true; // Mark the password as encrypted
    } else {
        std::cerr << "Password decryption failed. Set 'smtp_pass_encrypted' & 'email_pass_encrypted' to false and re-enter both passwords (config.json)." << std::endl;
        return false;
    }

    smtpPass = smtpPassDecrypted;
    emailPassword = emailPassDecrypted;

// End of SMTP/Mail Password Encryption Check

    // Define and initialize variables to track loaded settings
    bool smtpServerLoaded = !smtpServer.empty();
    bool smtpPortLoaded = smtpPort > 0;
    bool smtpUsernameLoaded = !smtpUsername.empty();
    bool smtpPassLoaded = !smtpPass.empty();
    bool venuesCsvPathLoaded = !venuesCsvPath.empty();
    bool emailPasswordLoaded = !emailPassword.empty();
    bool senderEmailLoaded = !senderEmail.empty();
    bool senderSmtpPortLoaded = senderSmtpPort > 0;

    // Check if the configuration settings are loaded successfully
    bool configLoadedSuccessfully = smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded &&
                                    smtpPassLoaded && venuesCsvPathLoaded && emailPasswordLoaded &&
                                    senderEmailLoaded && senderSmtpPortLoaded;

    // Display messages based on loaded settings
    if (smtpServerLoaded || smtpPortLoaded || smtpUsernameLoaded || smtpPassLoaded || 
        venuesCsvPathLoaded || emailPasswordLoaded || senderEmailLoaded || senderSmtpPortLoaded) {
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
        if (!encryptPassword(smtpPass, smtpPassEncryptedNew, encryptionKey)) {
            std::cerr << "Failed to encrypt SMTP password for saving in config.json." << std::endl;
            return false;
        }
        config["smtp_password"] = smtpPassEncryptedNew;
        config["smtp_pass_encrypted"] = true;
    }

    if (!isEmailPassEncrypted) {
        std::string emailPassEncryptedNew;
        if (!encryptPassword(emailPassword, emailPassEncryptedNew, encryptionKey)) {
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