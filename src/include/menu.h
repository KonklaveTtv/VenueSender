#ifndef MENU_H
#define MENU_H

#include "errorhandler.h"
#include "fileutils.h"
#include "structs.h"

#include <iostream>
#include <vector>

// Class to manage menu-related operations
class MenuManager {
public:

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

    // Constants for menu options related to filtering venues
    static const int FILTER_BY_GENRE_OPTION;
    static const int FILTER_BY_STATE_OPTION;
    static const int FILTER_BY_CITY_OPTION;
    static const int FILTER_BY_CAPACITY_OPTION;

    // Constants for other menu options
    static const int CLEAR_SELECTED_VENUES_OPTION;
    static const int VIEW_SELECTED_VENUES_OPTION;
    static const int SHOW_EMAIL_SETTINGS_OPTION;
    static const int VIEW_EDIT_EMAILS_OPTION;
    static const int FINISH_AND_SEND_EMAILS_OPTION;
    static const int EXIT_OPTION;

    // Function to validate if the user's choice is a valid menu option
    inline bool isValidMenuChoice(int choice) {
        
        // Validate if the choice is within valid menu options
        return choice >= static_cast<int>(MenuOption::FilterByGenre) &&
               choice <= static_cast<int>(MenuOption::Exit);
    }

    // Function to display the available menu options to the user
    int displayMenuOptions();

    // Function to display the list of venues selected by the user
    void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);
};

#endif // MENU_H