#ifndef MENU_H
#define MENU_H

#include "encryption.h"
#include "filtercriteria.h"

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

    // Options for filtering venues by different criteria
    static const int FILTER_BY_GENRE_OPTION;
    static const int FILTER_BY_STATE_OPTION;
    static const int FILTER_BY_CITY_OPTION;
    static const int FILTER_BY_CAPACITY_OPTION;

    // Other menu options
    static const int CLEAR_SELECTED_VENUES_OPTION;
    static const int VIEW_SELECTED_VENUES_OPTION;
    static const int SHOW_EMAIL_SETTINGS_OPTION;
    static const int VIEW_EDIT_EMAILS_OPTION;
    static const int FINISH_AND_SEND_EMAILS_OPTION;
    static const int EXIT_OPTION;

    // Function to validate user input
    bool isValidMenuChoice(int choice);

    // Function to display the menu to the user
    int displayMenuOptions();

    // Function to display selected venues to the user
    void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);
};

#endif // MENU_H