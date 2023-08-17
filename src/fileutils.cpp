#include "fileutils.h"

using namespace std;

EncryptionManager encryptionManager;

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

// Function to clear the console screen
void clearConsole() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For other platforms
#endif
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

// Function to load the settings config.json data and encrypt and decrypt email password
bool loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost, 
                        std::string& senderEmail, std::string& smtpUsername, 
                        std::string& mailPass, int& smtpPort, std::string& smtpServer, 
                        std::string& venuesCsvPath) {

    // Define keys and paths based on whether UNIT_TESTING is enabled or not
    std::string configPath, emailKey, smtpPortKey, smtpServerKey, smtpUsernameKey, 
                 venuesPath, mailPassKey, encryptedMailPassKey;
    
#ifdef UNIT_TESTING
    configPath = confPaths::mockConfigJsonPath;
    emailKey = "mock_sender_email";
    smtpPortKey = "mock_smtp_port";
    smtpServerKey = "mock_smtp_server";
    smtpUsernameKey = "mock_smtp_username";
    venuesPath = confPaths::mockVenuesCsvPath;
    mailPassKey = "mock_email_password";
    encryptedMailPassKey = "mock_email_pass_encrypted";
#else
    configPath = confPaths::configJsonPath;
    emailKey = "sender_email";
    smtpPortKey = "smtp_port";
    smtpServerKey = "smtp_server";
    smtpUsernameKey = "smtp_username";
    venuesPath = confPaths::venuesCsvPath;
    mailPassKey = "email_password";
    encryptedMailPassKey = "email_pass_encrypted";
#endif

    // Read the configuration
    Json::Value config;
    ifstream configFile(configPath);
    if (!configFile.is_open()) {
        cerr << "Failed to open " << configPath << "." << endl;
        return false;
    }
    configFile >> config;

    // Load smtp user settings
    senderEmail = config[emailKey].asString();
    smtpPort = config[smtpPortKey].asInt();
    smtpServer = config[smtpServerKey].asString();
    smtpUsername = config[smtpUsernameKey].asString();
    venuesCsvPath = venuesPath;

    // Load SSL settings (they're the same for both unit testing and not)
    useSSL = config["useSSL"].asBool();
    verifyPeer = config["verifyPeer"].asBool();
    verifyHost = config["verifyHost"].asBool();

    // SMTP/Mail Encryption/Decryption
    mailPass = config[mailPassKey].asString();

    bool ismailPassEncrypted = config.isMember(encryptedMailPassKey) ? config[encryptedMailPassKey].asBool() : false;
    string mailPassEncrypted;

    if (!ismailPassEncrypted) {
        if (!encryptionManager.encryptPassword(mailPass, mailPassEncrypted)) {
            cerr << "Failed to encrypt email password for saving in " << configPath << "." << endl;
            return false;
        }
        config[mailPassKey] = mailPassEncrypted;
        config[encryptedMailPassKey] = true;
    } else {
        mailPassEncrypted = config[mailPassKey].asString();
    }
    ofstream configFileOut(configPath);
    if (!configFileOut.is_open()) {
        cerr << "Failed to open " << configPath << " for writing." << endl;
        return false;
    }
    configFileOut << config;
    configFileOut.close();

    // Decryption
    string mailPassDecrypted = encryptionManager.decryptPassword(mailPassEncrypted);
    if (mailPassDecrypted.empty()) {
        cerr << "Email password decryption failed." << endl;
        return false;
    }
    mailPass = mailPassDecrypted;
 
    // Check for successfully loaded settings
    bool smtpServerLoaded = !smtpServer.empty();
    bool smtpPortLoaded = smtpPort > 0;
    bool smtpUsernameLoaded = !smtpUsername.empty();
    bool venuesCsvPathLoaded = !venuesCsvPath.empty();
    bool mailPassLoaded = !mailPass.empty();
    bool senderEmailLoaded = !senderEmail.empty();

    bool configLoadedSuccessfully = smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded &&
                                    venuesCsvPathLoaded && mailPassLoaded && senderEmailLoaded;

    if (smtpServerLoaded || smtpPortLoaded || smtpUsernameLoaded || venuesCsvPathLoaded || mailPassLoaded || senderEmailLoaded) {
        cout << "Configuration settings loaded from " << configPath << "." << endl;
        configLoadedSuccessfully = true;
    } else if (!configLoadedSuccessfully) {
        cerr << "Failed to load configuration settings from " << configPath << "." << endl;
    }

    return configLoadedSuccessfully;
}

// Function to reset flags and passwords in config.json
void resetConfigFile() {
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
