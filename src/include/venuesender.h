#ifndef VENUESENDER_H
#define VENUESENDER_H

#include "filtercriteria.h"

#include <chrono>
#include <string>
#include <thread>

#include <curl/curl.h>

extern std::string emailBeingSent;

// Forward declaration of the Venue struct
struct Venue;

// Forward declaration of the SelectedVenue struct
struct SelectedVenue;

// Define enums for return codes
enum class ReturnCode {
    Success,
    ConfigLoadError,
    EmailDecryptionError,
    SmtpPassDecryptionError
};

// Constants for menu options
enum class MenuOption {
    FilterByGenre = 1,
    FilterByState,
    FilterByCity,
    FilterByCapacity,
    ClearSelectedVenues,
    ViewSelectedVenues,
    ShowEmailSettings,
    FinishAndSendEmails,
    Exit
};

class CurlHandleWrapper {
public:
    CurlHandleWrapper();
    ~CurlHandleWrapper();
    CURL* get() const;
    static void init();
    static void cleanup();

    // Change visibility of progressCallback to public
    int progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/);

private:
    CURL* curl;
};

// Member pointers as static constexpr members
static constexpr auto genreMemberPtr = &Venue::genre;
static constexpr auto stateMemberPtr = &Venue::state;
static constexpr auto cityMemberPtr = &Venue::city;
static constexpr auto capacityMemberPtr = &Venue::capacity;

// Constants
const char CSV_DELIMITER = ','; // Delimiter used in CSV files
const int MAX_EMAIL_LENGTH = 320; // Maximum length for an email address, adjust as needed
const int MAX_MESSAGE_LENGTH = 100; // Maximum length for the email subject, adjust as needed
const int MAX_SUBJECT_LENGTH = 2000; // Maximum length for an email address, adjust as needed

// Options for filtering venues by different criteria
const int FILTER_BY_GENRE_OPTION = static_cast<int>(MenuOption::FilterByGenre);
const int FILTER_BY_STATE_OPTION = static_cast<int>(MenuOption::FilterByState);
const int FILTER_BY_CITY_OPTION = static_cast<int>(MenuOption::FilterByCity);
const int FILTER_BY_CAPACITY_OPTION = static_cast<int>(MenuOption::FilterByCapacity);

// Other menu options
const int CLEAR_SELECTED_VENUES_OPTION = static_cast<int>(MenuOption::ClearSelectedVenues);
const int VIEW_SELECTED_VENUES_OPTION = static_cast<int>(MenuOption::ViewSelectedVenues);
const int FINISH_AND_SEND_EMAILS_OPTION = static_cast<int>(MenuOption::FinishAndSendEmails);
const int EXIT_OPTION = static_cast<int>(MenuOption::Exit);

// Function to clear input buffer
void clearInputBuffer();

// Function to read CSV file and populate data
void readCSV(std::vector<Venue>& venues, const std::string& venuesCsvPath);

std::string getVenuesCsvPathFromConfig();

// Function to load the settings config.json data and encrypt and decrypt email/smtp passwords
bool loadConfigSettings(std::string& smtpServer, int& smtpPort, std::string& smtpUsername,
                        std::string& smtpPass, std::string& venuesCsvPath, 
                        std::string& emailPassword, std::string& senderEmail, int& senderSmtpPort);

// Function to display the menu
int displayMenuOptions();

void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

// Forward declare the processVenueSelection function
void processVenueSelection(const std::vector<SelectedVenue>& temporaryFilteredVenues,
                           std::vector<SelectedVenue>& selectedVenuesForEmail);

// Function to construct an email by providing subject and message
void constructEmail(std::string& subject, std::string& message);

// Check if an email address is in a valid format
bool isValidEmail(const std::string& email);

// Function to send an individual email to a recipient with custom subject and message
bool sendIndividualEmail(CURL* curl,
                        const SelectedVenue& selectedVenue,
                        const std::string& senderEmail,
                        const std::string& subject,
                        const std::string& message,
                        const std::string& smtpServer,
                        int smtpPort,
                        const std::string& smtpUsername,
                        const std::string& smtpPass,
                        double& progress);

// Function to view the progress of email sending done by sendIndividualEmail()
void viewEmailSendingProgress(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail,
                              const std::string& senderEmail,
                              const std::string& subject,
                              const std::string& message,
                              const std::string& smtpServer,
                              int smtpPort,
                              const std::string& smtpUsername,
                              const std::string& smtpPass);

#endif // VENUESENDER_H
