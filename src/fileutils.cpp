#include "fileutils.h"

using namespace std;

// Function to trim leading and trailing spaces from a string
string trim(const string& str){
    // Trimming function
    const auto notSpace = [](int ch) {return !isspace(ch); };
    auto first = find_if(str.begin(), str.end(), notSpace);
    auto last = find_if(str.rbegin(), str.rend(), notSpace).base();
    return (first < last ? string(first, last) : string());
}

// Clear the input buffer
void clearInputBuffer() {
    // Clear the input buffer
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
}

// Function to read venue data from CSV file
void readCSV(vector<Venue>& venues, const string& venuesCsvPath) {
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

// Function to load the settings config.json data and encrypt and decrypt email/smtp passwords
bool loadConfigSettings(string& smtpServer, int& smtpPort,
                        string& smtpUsername, string& smtpPass,
                        string& venuesCsvPath, string& mailPass,
                        string& senderEmail, int& senderSmtpPort,
                        bool& useSSL, bool& verifyPeer, bool& verifyHost) {

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
    senderSmtpPort = config["mock_sender_smtp_port"].asInt();
    smtpPort = config["mock_smtp_port"].asInt();
    smtpServer = config["mock_smtp_server"].asString();
    smtpUsername = config["mock_smtp_username"].asString();
    
    // Load SSL settings
    useSSL = config["useSSL"].asBool();
    verifyPeer = config["verifyPeer"].asBool();
    verifyHost = config["verifyHost"].asBool();

    // Load venues.csv path from config
    venuesCsvPath = confPaths::mockVenuesCsvPath;
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
    senderSmtpPort = config["sender_smtp_port"].asInt();
    smtpPort = config["smtp_port"].asInt();
    smtpServer = config["smtp_server"].asString();
    smtpUsername = config["smtp_username"].asString();

    // Load SSL settings
    useSSL = config["useSSL"].asBool();
    verifyPeer = config["verifyPeer"].asBool();
    verifyHost = config["verifyHost"].asBool();

    // Load venues.csv path from config
    venuesCsvPath = confPaths::venuesCsvPath;
#endif

// SMTP/Mail Encryption/Decryption

#ifdef UNIT_TESTING
    // Load plain text passwords from mock_config.json
    smtpPass = config["mock_smtp_password"].asString();
    mailPass = config["mock_email_password"].asString();

    // SMTP/Mail Encryption Check
    bool isSmtpPassEncrypted = config.isMember("mock_smtp_pass_encrypted") ? config["mock_smtp_pass_encrypted"].asBool() : false;
    bool ismailPassEncrypted = config.isMember("mock_email_pass_encrypted") ? config["mock_email_pass_encrypted"].asBool() : false;

    string smtpPassEncrypted;
    string mailPassEncrypted;

    // Encrypt the smtp password if it is not already encrypted and update the config file
    if (!isSmtpPassEncrypted) {
        if (!encryptPassword(smtpPass, smtpPassEncrypted)) {
            cerr << "Failed to encrypt SMTP password for saving in config.json." << endl;
            return false;
        }
        config["mock_smtp_password"] = smtpPassEncrypted;
        config["mock_smtp_pass_encrypted"] = true;
    } else {
        smtpPassEncrypted = config["mock_smtp_password"].asString();
    }

    // Encrypt the mail password if it is not already encrypted and update the config file
    if (!ismailPassEncrypted) {
        if (!encryptPassword(mailPass, mailPassEncrypted)) {
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
    smtpPass = config["smtp_password"].asString();
    mailPass = config["email_password"].asString();

    // SMTP/Mail Encryption Check
    bool isSmtpPassEncrypted = config.isMember("smtp_pass_encrypted") ? config["smtp_pass_encrypted"].asBool() : false;
    bool ismailPassEncrypted = config.isMember("email_pass_encrypted") ? config["email_pass_encrypted"].asBool() : false;

    string smtpPassEncrypted;
    string mailPassEncrypted;

    // Encrypt the smtp password if it is not already encrypted and update the config file
    if (!isSmtpPassEncrypted) {
        if (!encryptPassword(smtpPass, smtpPassEncrypted)) {
            cerr << "Failed to encrypt SMTP password for saving in config.json." << endl;
            return false;
        }
        config["smtp_password"] = smtpPassEncrypted;
        config["smtp_pass_encrypted"] = true;
    } else {
        smtpPassEncrypted = config["smtp_password"].asString();
    }

    // Encrypt the mail password if it is not already encrypted and update the config file
    if (!ismailPassEncrypted) {
        if (!encryptPassword(mailPass, mailPassEncrypted)) {
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
    smtpPass = config["mock_smtp_password"].asString();
    mailPass = config["mock_email_password"].asString();

    // SMTP/Mail Password Decryption Check
    string smtpPassDecrypted;
    string mailPassDecrypted;

    // Decrypt the encrypted passwords
    smtpPassDecrypted = decryptPassword(smtpPass);
    if (smtpPassDecrypted.empty()) {
        cerr << "SMTP password decryption failed." << endl;
        return false;
    }

    mailPassDecrypted = decryptPassword(mailPass);
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
    smtpPass = config["smtp_password"].asString();
    mailPass = config["email_password"].asString();

    // SMTP/Mail Password Decryption Check
    string smtpPassDecrypted;
    string mailPassDecrypted;

    // Decrypt the encrypted passwords
    smtpPassDecrypted = decryptPassword(smtpPass);
    if (smtpPassDecrypted.empty()) {
        cerr << "SMTP password decryption failed." << endl;
        return false;
    }

    mailPassDecrypted = decryptPassword(mailPass);
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
    bool smtpPassLoaded = !smtpPass.empty();
    bool venuesCsvPathLoaded = !venuesCsvPath.empty();
    bool mailPassLoaded = !mailPass.empty();
    bool senderEmailLoaded = !senderEmail.empty();
    bool senderSmtpPortLoaded = senderSmtpPort > 0;

    // Check if the configuration settings are loaded successfully
    bool configLoadedSuccessfully = smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded &&
                                    smtpPassLoaded && venuesCsvPathLoaded && mailPassLoaded &&
                                    senderEmailLoaded && senderSmtpPortLoaded;

    // Display messages based on loaded settings
    if (smtpServerLoaded || smtpPortLoaded || smtpUsernameLoaded || smtpPassLoaded || 
        venuesCsvPathLoaded || mailPassLoaded || senderEmailLoaded || senderSmtpPortLoaded) {
        cout << "Configuration settings loaded from config.json." << endl;
        configLoadedSuccessfully = true;
    } else if (smtpServerLoaded || smtpPortLoaded || smtpPassLoaded || senderEmailLoaded || senderSmtpPortLoaded) {
        cout << "Email settings loaded from config.json." << endl;
    } else if (!configLoadedSuccessfully) {
        cerr << "Failed to load configuration settings from config.json." << endl;
    }

    return configLoadedSuccessfully;
}

// Function to reset flags and passwords in config.json
void resetConfigFile() {
    Json::Value config;

#ifdef UNIT_TESTING
    // Read the existing mock_config.json
    ifstream configFile(confPaths::mockConfigJsonPath);
    if (!configFile.is_open()) {
        cerr << "Failed to open mock_config.json." << endl;
        return;
    }
    configFile >> config;
    configFile.close();

    // Modify the values in the JSON object for testing
    config["mock_smtp_pass_encrypted"] = false;
    config["mock_email_pass_encrypted"] = false;
    config["mock_smtp_password"] = "mock_smtp_password";
    config["mock_email_password"] = "mock_email_password";

    // Close the input file stream before opening for output
    configFile.close();

   // Write the modified JSON object back to mock_config.json
    ofstream configFileOut(confPaths::mockConfigJsonPath);
    if (!configFileOut.is_open()) {
        cerr << "Failed to open mock_config.json for writing." << endl;
        return;
    }
    configFileOut << config;
    configFileOut.close();
#else
    // Read the existing config.json
    ifstream configFile(confPaths::configJsonPath);
    if (!configFile.is_open()) {
        cerr << "Failed to open config.json." << endl;
        return;
    }
    configFile >> config;
    configFile.close();

    // Modify the values in the JSON object
    config["smtp_pass_encrypted"] = false;
    config["email_pass_encrypted"] = false;
    config["smtp_password"] = "enter_smtp_password";
    config["email_password"] = "enter_email_password";

    // Close the input file stream before opening for output
    configFile.close();

    // Write the modified JSON object back to config.json
    ofstream configFileOut(confPaths::configJsonPath);
    configFileOut << config;
    configFileOut.close();
#endif // UNIT_TESTING
}
