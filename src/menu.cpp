#include "include/menu.h"

// Use the standard namespace
using namespace std;

// Initialize constants for menu options
const int MenuManager::FILTER_BY_GENRE_OPTION = static_cast<int>(MenuManager::MenuOption::FilterByGenre);
const int MenuManager::FILTER_BY_STATE_OPTION = static_cast<int>(MenuManager::MenuOption::FilterByState);
const int MenuManager::FILTER_BY_CITY_OPTION = static_cast<int>(MenuManager::MenuOption::FilterByCity);
const int MenuManager::FILTER_BY_CAPACITY_OPTION = static_cast<int>(MenuManager::MenuOption::FilterByCapacity);

// Other menu options
const int MenuManager::CLEAR_SELECTED_VENUES_OPTION = static_cast<int>(MenuManager::MenuOption::ClearSelectedVenues);
const int MenuManager::VIEW_SELECTED_VENUES_OPTION = static_cast<int>(MenuManager::MenuOption::ViewSelectedVenues);
const int MenuManager::SHOW_EMAIL_SETTINGS_OPTION = static_cast<int>(MenuManager::MenuOption::ShowEmailSettings);
const int MenuManager::VIEW_EDIT_EMAILS_OPTION = static_cast<int>(MenuManager::MenuOption::ViewEditEmail);
const int MenuManager::FINISH_AND_SEND_EMAILS_OPTION = static_cast<int>(MenuManager::MenuOption::FinishAndSendEmail);
const int MenuManager::EXIT_OPTION = static_cast<int>(MenuManager::MenuOption::Exit);

// Function to validate if the user's choice is a valid menu option
bool MenuManager::isValidMenuChoice(int choice) {
    
    // Validate if the choice is within valid menu options
    return choice >= static_cast<int>(MenuOption::FilterByGenre) &&
           choice <= static_cast<int>(MenuOption::Exit);
}

// Function to display the menu options to the user and get their choice
int MenuManager::displayMenuOptions() {
#ifdef UNIT_TESTING
    int choice;
    do {
        // Display the main menu options
        cout << "====================="<< endl;
        cout << "===== Main Menu =====" << endl;
        cout << "====================="<< endl;
        cout << static_cast<int>(MenuManager::MenuOption::FilterByGenre) << ". Filter by Genre" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::FilterByState) << ". Filter by State" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::FilterByCity) << ". Filter by City" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::FilterByCapacity) << ". Filter by Capacity" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::ClearSelectedVenues) << ". Clear Selected Venues" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::ViewSelectedVenues) << ". View Selected Venues" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::ShowEmailSettings) << ". Show Email Settings" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::ViewEditEmail) << ". View & Edit Email" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::FinishAndSendEmail) << ". Finish & Send Emails" << endl;
        cout << static_cast<int>(MenuManager::MenuOption::Exit) << ". Exit VenueSender" << endl;
        cout << "Enter your choice: ";

        // Get user's choice
        if (!(cin >> choice)) {            
            // Handle invalid menu input (non-numeric)
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_MENU_INPUT_ERROR);
        } else if (!isValidMenuChoice(choice)) {
                // Handle choice that is out of valid range
                ErrorHandler errorHandler;
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR); 
            cout << static_cast<int>(MenuManager::MenuOption::FilterByGenre) 
                 << " and " 
                 << static_cast<int>(MenuManager::MenuOption::Exit) << "." << endl;
        } else {
            break;
        }
    } while (true);

    return choice;
#else
    int choice;
    do {
        // Display the main menu options
        cout << "====================="<< endl;
        cout << "===== Main Menu =====" << endl;
        cout << "====================="<< endl;
        cout << static_cast<int>(MenuOption::FilterByGenre) << ". Filter by Genre" << endl;
        cout << static_cast<int>(MenuOption::FilterByState) << ". Filter by State" << endl;
        cout << static_cast<int>(MenuOption::FilterByCity) << ". Filter by City" << endl;
        cout << static_cast<int>(MenuOption::FilterByCapacity) << ". Filter by Capacity" << endl;
        cout << static_cast<int>(MenuOption::ClearSelectedVenues) << ". Clear Selected Venues" << endl;
        cout << static_cast<int>(MenuOption::ViewSelectedVenues) << ". View Selected Venues" << endl;
        cout << static_cast<int>(MenuOption::ShowEmailSettings) << ". Show Email Settings" << endl;
        cout << static_cast<int>(MenuOption::ViewEditEmail) << ". View & Edit Email" << endl;
        cout << static_cast<int>(MenuOption::FinishAndSendEmail) << ". Finish & Send Emails" << endl;
        cout << static_cast<int>(MenuOption::Exit) << ". Exit VenueSender" << endl;
        cout << "Enter your choice: ";

        // Get user's choice
        if (!(cin >> choice)) {
            // Handle invalid menu input (non-numeric)
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_MENU_INPUT_ERROR);
            cin.ignore(); // If there's a chance you might have used cin before this point
            cin.get(); // This will wait for a key press
        } else if (!isValidMenuChoice(choice)) {
                ErrorHandler errorHandler;
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR); 
            cout << static_cast<int>(MenuManager::MenuOption::FilterByGenre) 
                 << " and " 
                 << static_cast<int>(MenuManager::MenuOption::Exit) << "." << endl;
            cout << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer();
            cin.get(); // This will wait for a key press
        } else {
            break;
        }
    } while (true);

    return choice;
#endif
}

// Function to display the venues that have been selected by the user
void MenuManager::displaySelectedVenues(const vector<SelectedVenue>& selectedVenues) {

    // Display header
    cout << "==========================="<< endl;
    cout << "===== Selected Venues =====" << endl;
    cout << "==========================="<< endl;

    // Check if any venues have been selected
    if (selectedVenues.empty()) {
        
        // Handle case where no venues have been selected
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::NO_VENUES_SELECTED_ERROR);
    } else {
        // Loop through and display each selected venue's details
        for (const auto& venue : selectedVenues) {
            cout << "Name: " << venue.name << endl;
            cout << "Email: " << venue.email << endl;
            cout << "City: " << venue.city << endl;
            cout << "--------------------------" << endl;
        }
    }
#ifndef UNIT_TESTING
    // Show additional information if not in unit testing mode
    cout << "===========================" << endl;
    ErrorHandler errorHandler;
    errorHandler.showInfoAndReturn();
    cin.get(); // This will wait for a key press
#endif
}
