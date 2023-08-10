#ifndef VENUESENDER_H
#define VENUESENDER_H

#include "filtercriteria.h"

#include <string>

#include <curl/curl.h>

// Forward declaration of the Venue struct
struct Venue;

// Forward declaration of the SelectedVenue struct
struct SelectedVenue;

// Add these member pointers as static constexpr members in the Venue struct
static constexpr auto genreMemberPtr = &Venue::genre;
static constexpr auto stateMemberPtr = &Venue::state;
static constexpr auto cityMemberPtr = &Venue::city;
static constexpr auto capacityMemberPtr = &Venue::capacity;

// Constants for menu options
enum class MenuOption {
    FilterByGenre = 1,
    FilterByState,
    FilterByCity,
    FilterByCapacity,
    ClearSelectedVenues,
    ViewSelectedVenues,
    FinishAndSendEmails,
    Exit
};

// Load configuration settings from config.json
bool loadConfigSettings(std::string& smtpServer, int& smtpPort, std::string& smtpUsername,
                        std::string& smtpPassword, std::string& venuesCsvPath, 
                        std::string& emailPassword, std::string& senderEmail, int& senderSmtpPort);

// Function to read CSV file and populate data
void readCSV(std::vector<Venue>& venues, const std::string& venuesCsvPath);

std::string getVenuesCsvPathFromConfig();

// Function to display the menu
int displayMenuOptions();

// Check if an email address is in a valid format
bool isValidEmail(const std::string& email);

// Function to get user's email credentials and SMTP settings
void getUserEmailSettings(std::string& emailPassword, std::string& smtpServer, int& smtpPort, std::string& senderEmail, int& senderSmtpPort);

// Function to send an individual email to a recipient with custom subject and message
bool sendIndividualEmail(CURL* curl,
                         const SelectedVenue& selectedVenue,
                         const std::string& senderEmail,
                         const std::string& subject,
                         const std::string& message,
                         const std::string& smtpServer,
                         int smtpPort);

// Function to clear input buffer
void clearInputBuffer();

void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

// Function to send bulk emails to recipients with custom subject and message using libcurl
void sendEmails(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail, const std::string& senderEmail, const std::string& subject, const std::string& message, const std::string& smtpServer, int smtpPort);

void getEmailSubjectAndMessage(std::string& subject, std::string& message);

#endif // VENUESENDER_H
