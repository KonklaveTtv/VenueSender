#include "venueutils.h"
#include "filtercriteria.h"
#include "venue.h"
#include "venuesender.h"

#include <fstream>
#include <limits>
#include <regex>

#include "json/json.h"

/* CurlHandleWrapper*/
/*-------------------*/
int CurlHandleWrapper::progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/) {
    // You can calculate the progress percentage based on the parameters provided
    // and provide updates to the user here
    // Example: Print the progress every 10% completion
    if (dltotal > 0) {
        double progress = (dlnow / dltotal) * 100;
        if (progress >= 10) {
            std::cout << "Email sending progress: " << progress << "%" << std::endl;
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

    // Initialize encryption key and nonce
    std::array<unsigned char, crypto_secretbox_KEYBYTES> encryptionKey;
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> encryptionNonce;
    initializeEncryptionParams(encryptionKey, encryptionNonce);

    std::string smtpPassDecrypted;
    std::string emailPassDecrypted;

    // Check if the passwords are already encrypted
    bool isSmtpPassEncrypted = config.isMember("smtp_pass_encrypted") ? config["smtp_pass_encrypted"].asBool() : false;
    bool isEmailPassEncrypted = config.isMember("email_pass_encrypted") ? config["email_pass_encrypted"].asBool() : false;

    // Decrypt or use decrypted passwords
    if (isSmtpPassEncrypted) {
        if (!decryptPassword(smtpPassEncrypted, smtpPassDecrypted, encryptionKey)) {
            std::cerr << "Failed to decrypt SMTP password. Please re-enter passwords in config.json." << std::endl;
            return false;
        }
    } else {
        smtpPassDecrypted = smtpPassEncrypted;
    }

    if (isEmailPassEncrypted) {
        if (!decryptPassword(emailPassEncrypted, emailPassDecrypted, encryptionKey)) {
            std::cerr << "Failed to decrypt email password. Please re-enter passwords in config.json." << std::endl;
            return false;
        }
    } else {
        emailPassDecrypted = emailPassEncrypted;
    }

    smtpPass = smtpPassDecrypted;
    emailPassword = emailPassDecrypted;

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
    
    // Limit the input length for the subject
    const int maxSubjectLength = 100; // Adjust as needed
    std::cout << "Enter the subject for the email: ";
    std::getline(std::cin, subject);
    if (subject.length() > maxSubjectLength) {
        std::cout << "Subject too long. Please try again." << std::endl;
        return; // Or handle the error appropriately
    }

    // Limit the input length for the message
    const int maxMessageLength = 1000; // Adjust as needed
    std::cout << "Enter the message for the email (press Enter on a new line to finish):\n";
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) {
        if (message.length() + line.length() > maxMessageLength) {
            std::cout << "Message too long. Continuing with current input." << std::endl;
            break; // Or handle the error appropriately
        }
        message += line + "\n";
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
                        const std::string& smtpPass) {
    // Set up and send an email using libcurl
    if (!curl) {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return false;
    }

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

    // Set the progress callback function
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, nullptr);
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

// Function to send emails to selected venues
void sendEmails(CURL* curl,
                const std::vector<SelectedVenue>& selectedVenuesForEmail,
                const std::string& senderEmail,
                const std::string& subject,
                const std::string& message,
                const std::string& smtpServer,
                int smtpPort,
                const std::string& smtpUsername,
                const std::string& smtpPass) {
    // Set progress callback for the bulk email sending process
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    // Iterate through selected venues and send individual emails
    for (const SelectedVenue& venue : selectedVenuesForEmail) {
        sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, smtpUsername, smtpPass);
    }
}

void viewEmailSendingProgress(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail) {
    // Set the custom progress callback function from CurlHandleWrapper
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);

    for (size_t i = 0; i < selectedVenuesForEmail.size(); ++i) {
        const SelectedVenue& venue = selectedVenuesForEmail[i];
        std::cout << "Sending email to: " << venue.email << std::endl;

        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "Email sending progress completed." << std::endl;
}

// Clear the input buffer
void clearInputBuffer() {
    // Clear the input buffer
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to get user input for email subject and message
void getEmailSubjectAndMessage(std::string& subject, std::string& message) {
    // Prompt user to enter email subject and message
    std::cin.ignore(); // Clear input buffer
    std::cout << "Enter subject for the email: ";
    std::getline(std::cin, subject);

    std::cout << "Enter message for the email: ";
    std::getline(std::cin, message);
}
