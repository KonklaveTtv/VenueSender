#include <fstream>
#include <limits>
#include <regex>

#include "json/json.h"
#include <curl/curl.h>

#include "filtercriteria.h"
#include "venueutils.h"
#include "venue.h"
#include "venuesender.h"

// Load configuration settings from config.json
bool loadConfigSettings(std::string& smtpServer, int& smtpPort,
                        std::string& smtpUsername, std::string& smtpPassword,
                        std::string& venuesCsvPath, std::string& emailPassword,
                        std::string& senderEmail, int& senderSmtpPort) {
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
    smtpPassword = config["smtp_password"].asString();
    venuesCsvPath = config["venues_csv_path"].asString();
    emailPassword = config["email_password"].asString();
    senderEmail = config["sender_email"].asString();
    senderSmtpPort = config["sender_smtp_port"].asInt();

    // Notify the user that the config.json has been read successfully
    std::cout << "Configuration settings loaded from config.json." << std::endl;

    return true;
}

// Constants
const char CSV_DELIMITER = ',';
const int MAX_EMAIL_LENGTH = 320; // An example value, adjust as needed
const int INVALID_CAPACITY = -1;  // To indicate an invalid capacity

std::string trim(const std::string& str){
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

// Function to check if an email address is in a valid format
bool isValidEmail(const std::string& email) {
    // A simple regex pattern to check the format of the email
    static const std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailPattern);
}

// Function to get user's email credentials and SMTP settings
void getUserEmailSettings(std::string& emailPassword, std::string& smtpServer, int& smtpPort,
                          std::string& senderEmail, int& senderSmtpPort) {
    std::cout << "Reading user email settings from config.json..." << std::endl;

    // Load email settings from config.json
    std::string smtpUsername, smtpPassword, venuesCsvPath;
    if (!loadConfigSettings(smtpServer, smtpPort, smtpUsername, smtpPassword, venuesCsvPath,
                            emailPassword, senderEmail, senderSmtpPort)) {
        std::cerr << "Failed to load email settings from config.json." << std::endl;
        exit(1); // You might handle this error more gracefully
    }
}

// Function to construct an email by providing subject and message
void constructEmail(std::string& subject, std::string& message) {
    std::cout << "===== Construct Email =====" << std::endl;
    std::cout << "Enter the subject for the email: ";
    std::getline(std::cin, subject);

    std::cout << "Enter the message for the email (press Enter on a new line to finish):\n";
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) {
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
                        int smtpPort) {
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
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(message.length()));
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(message.length()));
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(message.length()));

    // Set the URL for the SMTP server
    std::string smtpUrl = "smtp://" + smtpServer + ":" + std::to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

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
                int smtpPort) {
    for (const SelectedVenue& venue : selectedVenuesForEmail) {
        sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort);
    }
}

// Clear the input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to get user input for email subject and message
void getEmailSubjectAndMessage(std::string& subject, std::string& message) {
    std::cin.ignore(); // Clear input buffer
    std::cout << "Enter subject for the email: ";
    std::getline(std::cin, subject);

    std::cout << "Enter message for the email: ";
    std::getline(std::cin, message);
}
