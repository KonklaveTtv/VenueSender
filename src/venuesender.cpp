#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

#include "json/json.h"
#include <curl/curl.h>

#include "filtercriteria.h"
#include "venueutils.h"
#include "venue.h"
#include "venuesender.h"

// Function to get a string input from the user
std::string getStringInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

// Function to get an integer input from the user
int getIntInput(const std::string& prompt) {
    int input = 0;
    std::string inputStr;
    bool validInput = false;

    while (!validInput) {
        std::cout << prompt;
        std::getline(std::cin, inputStr);
        std::stringstream ss(inputStr);
        if (ss >> input && ss.eof()) {
            validInput = true;
        } else {
            std::cout << "Invalid input. Please enter a valid integer." << std::endl;
        }
    }

    return input;
}

std::vector<SelectedVenue> selectVenuesForEmail(const std::vector<SelectedVenue>& venues) {
    std::vector<SelectedVenue> selectedVenuesForEmail;
    int numVenues = static_cast<int>(venues.size());

    if (numVenues == 0) {
        std::cout << "No venues to select from." << std::endl;
        return selectedVenuesForEmail;
    }

    std::cout << "===== Select Venues to Email =====" << std::endl;

    // Display the list of filtered venues to the user
    for (int i = 0; i < numVenues; ++i) {
        std::cout << i + 1 << ". Venue: " << venues[i].name << std::endl;
        std::cout << "   Email: " << venues[i].email << std::endl;
        std::cout << "   City: " << venues[i].city << std::endl;
        std::cout << "--------------------------" << std::endl;
    }

    // Ask the user to select venues to add to the selectedVenuesForEmail vector
    std::cout << "Enter the numbers of venues to email (comma-separated, e.g., 1,2,3): ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    int venueIndex;
    while (iss >> venueIndex) {
        if (venueIndex >= 1 && venueIndex <= numVenues) {
            selectedVenuesForEmail.push_back(SelectedVenue()); // Use the default constructor
            selectedVenuesForEmail.back().name = venues[venueIndex - 1].name;
            selectedVenuesForEmail.back().email = venues[venueIndex - 1].email;
            selectedVenuesForEmail.back().city = venues[venueIndex - 1].city;
        }
    }

    return selectedVenuesForEmail;
}

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

// Venue selection logic
std::vector<SelectedVenue> selectVenuesFromFilteredResults(const std::vector<SelectedVenue>& filteredVenues) {
    std::vector<SelectedVenue> selectedVenues;
    std::cout << "Select venues to add (comma-separated indices, e.g., 1,2,3): ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string indexStr;
    while (std::getline(iss, indexStr, ',')) {
        int venueIndex = std::stoi(indexStr);
        if (venueIndex >= 1 && venueIndex <= static_cast<int>(filteredVenues.size())) {
            selectedVenues.push_back(filteredVenues[static_cast<size_t>(venueIndex) - 1]);
        }
    }
    return selectedVenues;
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

void displayConfirmation(const std::vector<SelectedVenue>& selectedVenues,
                         const std::string& selectedGenre,
                         const std::string& selectedState,
                         const std::string& selectedCity,
                         int selectedCapacity,
                         const std::string& subject,
                         const std::string& message) {
    std::cout << "===== Confirmation =====" << std::endl;
    std::cout << "Selected Venues: " << std::endl;
    for (const auto& venue : selectedVenues) {
        std::cout << venue.name << " (" << venue.email << ")" << venue.city << std::endl;
    }
    std::cout << "Selected Genre: " << selectedGenre << std::endl;
    std::cout << "Selected State: " << selectedState << std::endl;
    std::cout << "Selected City: " << selectedCity << std::endl;
    std::cout << "Selected Capacity: " << selectedCapacity << std::endl;
    std::cout << "Email Subject: " << subject << std::endl;
    std::cout << "Email Message: " << message << std::endl;
    std::cout << "========================" << std::endl;
}

// Clear the input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to filter venues based on the selected criteria
std::vector<SelectedVenue> filterSelectedVenues(const std::vector<Venue>& venues, const FilterCriteria& criteria) {
    std::vector<SelectedVenue> filteredSelectedVenues; // Create a new vector of SelectedVenue objects
    std::copy_if(venues.begin(), venues.end(), std::back_inserter(filteredSelectedVenues),
        [&](const Venue& venue) {
            return (criteria.genre.empty() || venue.genre == criteria.genre) &&
                   (criteria.state.empty() || venue.state == criteria.state) &&
                   (criteria.city.empty() || venue.city == criteria.city) &&
                   (criteria.capacity == 0 || venue.capacity >= criteria.capacity);
        }
    );

    // Display the filtered venues to the user
    displaySelectedVenues(filteredSelectedVenues);
    return filteredSelectedVenues;
}

// Function to get user input for email subject and message
void getEmailSubjectAndMessage(std::string& subject, std::string& message) {
    std::cin.ignore(); // Clear input buffer
    std::cout << "Enter subject for the email: ";
    std::getline(std::cin, subject);

    std::cout << "Enter message for the email: ";
    std::getline(std::cin, message);
}

// Function to get confirmation from the user to send emails
bool getSendConfirmation() {
    char confirmSend;
    std::cout << "Do you want to send these emails? (Y/N): ";
    std::cin >> confirmSend;
    clearInputBuffer(); // Clear input buffer after reading confirmation character
    return (confirmSend == 'Y' || confirmSend == 'y');
}

// Function to get confirmation from the user to continue filtering
bool getContinueFilteringConfirmation() {
    char continueFiltering;
    std::cout << "Do you want to continue filtering? (Y/N): ";
    std::cin >> continueFiltering;
    clearInputBuffer(); // Clear input buffer after reading continuation character
    return (continueFiltering == 'Y' || continueFiltering == 'y');
}
