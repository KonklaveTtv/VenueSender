#ifndef VENUESENDER_H
#define VENUESENDER_H

#include "filtercriteria.h"

#include <set>
#include <string>
#include <vector>

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

std::string getStringInput(const std::string& prompt);

int getIntInput(const std::string& prompt);

bool loadConfigSettings(std::string& smtpServer, int& smtpPort, std::string& smtpUsername,
                        std::string& smtpPassword, std::string& venuesCsvPath, 
                        std::string& emailPassword, std::string& senderEmail, int& senderSmtpPort);

// Function to read CSV file and populate data
void readCSV(std::vector<Venue>& venues, const std::string& venuesCsvPath);

// Function to display the menu
int displayMenuOptions();

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

// New function to filter selected venues based on user's selected criteria
std::vector<SelectedVenue> filteredSelectedVenues(const std::vector<Venue>& venues, const FilterCriteria& selectedCriteria);

// Function to convert vector of Venue objects to vector of SelectedVenue objects
std::vector<SelectedVenue> convertToSelectedVenues(const std::vector<Venue>& venues);

// Function to clear input buffer
void clearInputBuffer();

// Function to display confirmation of selected filters and email details
void displayConfirmation(const std::vector<SelectedVenue>& selectedVenues,
                         const std::string& selectedGenre,
                         const std::string& selectedState,
                         const std::string& selectedCity,
                         int selectedCapacity,
                         const std::string& subject,
                         const std::string& message);

void displayVenueConfirmation(const std::vector<SelectedVenue>& selectedVenues,
                              const std::string& subject,
                              const std::string& message);

void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

std::vector<SelectedVenue> selectVenuesForEmail(const std::vector<SelectedVenue>& venues);

// Function to send bulk emails to recipients with custom subject and message using libcurl
void sendEmails(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail, const std::string& senderEmail, const std::string& subject, const std::string& message, const std::string& smtpServer, int smtpPort);

void getEmailSubjectAndMessage(std::string& subject, std::string& message);

bool getSendConfirmation();

bool getContinueFilteringConfirmation();

std::string getVenuesCsvPathFromConfig();
void testReadCSV();
void testIsValidEmail();
void testGetUniqueValues();

#endif // VENUESENDER_H
