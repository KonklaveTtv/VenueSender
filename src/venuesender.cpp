#include "venueutils.h"
#include "filtercriteria.h"
#include "venue.h"
#include "venuesender.h"

#include <fstream>
#include <limits>
#include <regex>

#include "json/json.h"

std::string emailBeingSent;

/* CurlHandleWrapper*/
/*-------------------*/
int CurlHandleWrapper::progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/) {
    // You can calculate the progress percentage based on the parameters provided
    // and provide updates to the user here
    // Example: Print the progress every 10% completion
    if (dltotal > 0) {
        double progress = (dlnow / dltotal) * 100;
        if (progress >= 10) {
            std::cout << "Email sending progress: " << progress << "% (" << emailBeingSent << ")" << std::endl;
        }
    }
    return 0;
}

CurlHandleWrapper::CurlHandleWrapper() {
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress callback
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
    }
}

CurlHandleWrapper::~CurlHandleWrapper() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

CURL* CurlHandleWrapper::get() const {
    return curl;
}

void CurlHandleWrapper::init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void CurlHandleWrapper::cleanup() {
    curl_global_cleanup();
}
/*-------------------*/

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

// Function to check if an email address is in a valid format
bool isValidEmail(const std::string& email) {
    // A simple regex pattern to check the format of the email
    static const std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailPattern);
}

// Trim leading and trailing spaces from a string
std::string trim(const std::string& str){
    // Trimming function
    const auto notSpace = [](int ch) {return !std::isspace(ch); };
    auto first = std::find_if(str.begin(), str.end(), notSpace);
    auto last = std::find_if(str.rbegin(), str.rend(), notSpace).base();
    return (first < last ? std::string(first, last) : std::string());
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

// Function to construct an email by providing subject and message
void constructEmail(std::string& subject, std::string& message) {
    std::cout << "===== Construct Email =====" << std::endl;
    
    // Limit the input length for the subject and message
    const int maxSubjectLength = MAX_SUBJECT_LENGTH; // Adjust as needed
    const int maxMessageLength = MAX_MESSAGE_LENGTH; // Adjust as needed

    // Prompt user to enter email subject and message
    std::cin.ignore(); // Clear input buffer
    std::cout << "Enter subject for the email: ";
    std::getline(std::cin, subject);

    if (subject.length() > maxSubjectLength) {
        std::cout << "Subject too long. Please try again." << std::endl;
        return; // Or handle the error appropriately
    }

    std::cout << "Enter the message for the email (press Enter on a new line to finish):\n";
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) {
        if (message.length() + line.length() > maxMessageLength) {
            std::cout << "Message too long. Continuing with current input." << std::endl;
            break; // Or handle the error appropriately
        }
        message += trim(line) + "\n";
    }
    std::cout << "============================" << std::endl;
}

// Function to send an individual email to a recipient with custom subject and message using libcurl
bool sendIndividualEmail(CURL* curl,
                        const SelectedVenue& selectedVenue,
                        const std::string& senderEmail,
                        const std::string& subject,
                        const std::string& message,
                        const std::string& smtpServer,
                        int smtpPort,
                        const std::string& smtpUsername,
                        const std::string& smtpPass,
                        double& progress) {
    // Set the value of emailBeingSent
    emailBeingSent = selectedVenue.email;

    // Set up and send an email using libcurl
    if (!curl) {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return false;
    }

    std::cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << std::endl;

    // Set the recipient email address
    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, selectedVenue.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    // Set the email sender
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

    // Set the email subject
    std::string subjectHeader = "Subject: " + subject;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, subjectHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the email body (message)
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(message.length()));

    // Set the URL for the SMTP server
    std::string smtpUrl = "smtp://" + smtpServer + ":" + std::to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // Set SMTP username and password
    std::string smtpUserPass = smtpUsername + ":" + smtpPass;
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUserPass.c_str());

    std::cout << "Authenticating with SMTP server..." << std::endl;

    // Set the progress callback function
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    // Perform the email sending
    CURLcode res = curl_easy_perform(curl);

    // Clean up and return the result
    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cerr << "Failed to send email: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    return true;
}

void viewEmailSendingProgress(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail,
                              const std::string& senderEmail,
                              const std::string& subject,
                              const std::string& message,
                              const std::string& smtpServer,
                              int smtpPort,
                              const std::string& smtpUsername,
                              const std::string& smtpPass) {
    // Set the custom progress callback function from CurlHandleWrapper
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);

    double progress = 0.0; // Initialize progress

    for (size_t i = 0; i < selectedVenuesForEmail.size(); ++i) {
        const SelectedVenue& venue = selectedVenuesForEmail[i];
        emailBeingSent = venue.email; // Set the value of emailBeingSent
        std::cout << "Sending email " << (i + 1) << " of " << selectedVenuesForEmail.size() << " to: " << venue.email << std::endl;

        // Send the individual email with progress tracking
        sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, smtpUsername, smtpPass, progress);

        emailBeingSent.clear(); // Reset the value of emailBeingSent
    }
    std::cout << "Email sending progress completed." << std::endl;
}

// Clear the input buffer
void clearInputBuffer() {
    // Clear the input buffer
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

