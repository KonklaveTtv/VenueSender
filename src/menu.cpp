#include "menu.h"

using namespace std;

// Options for filtering venues by different criteria
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

// Function to validate user input
bool MenuManager::isValidMenuChoice(int choice) {
    // Validate if the choice is within valid menu options
    return choice >= static_cast<int>(MenuOption::FilterByGenre) &&
           choice <= static_cast<int>(MenuOption::Exit);
}

void MenuManager::displayErrorAndPause(const string& errorMessage, istream& input, ostream& output) {
    output << errorMessage << endl;
    ConsoleUtils::clearInputBuffer();
    input.get();
}

// Function to display the menu to the user
int MenuManager::displayMenuOptions() {
#ifdef UNIT_TESTING
    int choice;
    do {
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

        if (!(cin >> choice)) {
            cout << "Invalid input, please enter a number." << endl;
        } else if (!isValidMenuChoice(choice)) {
            cout << "Invalid choice. Please enter a number between " 
                 << static_cast<int>(MenuManager::MenuOption::FilterByGenre) 
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

        if (!(cin >> choice)) {
            cout << "Invalid input, please enter a number. Press return to retry..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            cin.get();     // This will wait for a key press
        } else if (!isValidMenuChoice(choice)) {
            cout << "Invalid choice. Please enter a number between " 
                 << static_cast<int>(MenuOption::FilterByGenre) 
                 << " and " 
                 << static_cast<int>(MenuOption::Exit) << "." << endl;
            cout << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press
        } else {
            break;
        }
    } while (true);

    return choice;
#endif
}

// Function to display selected venues to the user
void MenuManager::displaySelectedVenues(const vector<SelectedVenue>& selectedVenues) {
    cout << "==========================="<< endl;
    cout << "===== Selected Venues =====" << endl;
    cout << "==========================="<< endl;
    if (selectedVenues.empty()) {
        cout << "No venues selected yet." << endl;
    } else {
        for (const auto& venue : selectedVenues) {
            cout << "Name: " << venue.name << endl;
            cout << "Email: " << venue.email << endl;
            cout << "City: " << venue.city << endl;
            cout << "--------------------------" << endl;
        }
    }
#ifndef UNIT_TESTING
    cout << "===========================" << endl;
    cout << "Press return to go back to Main Menu" << endl;
    cin.ignore();  // Clear input buffer
    ConsoleUtils::clearInputBuffer();
    cin.get();     // This will wait for a key press
#endif
}