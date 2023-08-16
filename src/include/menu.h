#ifndef MENU_H
#define MENU_H

#include "encryption.h"
#include "filtercriteria.h"

// Enumeration representing the available options in the main menu
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

// Function to validate user input
bool isValidMenuChoice(int choice);

// Function to display the menu to the user
int displayMenuOptions();

// Function to display the email settings set in config.json to the user
void viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost, 
                       const std::string& senderEmail, const std::string& mailPassDecrypted,
                       int smtpPort, const std::string& smtpServer);

// Function to display selected venues to the user
void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

#endif // MENU_H