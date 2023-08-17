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
    ViewEditEmail,
    FinishAndSendEmail,
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
const int SHOW_EMAIL_SETTINGS_OPTION = static_cast<int>(MenuOption::ShowEmailSettings);
const int VIEW_EDIT_EMAILS_OPTION = static_cast<int>(MenuOption::ViewEditEmail);
const int FINISH_AND_SEND_EMAILS_OPTION = static_cast<int>(MenuOption::FinishAndSendEmail);
const int EXIT_OPTION = static_cast<int>(MenuOption::Exit);

// Function to validate user input
bool isValidMenuChoice(int choice);

// Function to display the menu to the user
int displayMenuOptions();

// Function to display selected venues to the user
void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

#endif // MENU_H