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
    ConsoleUtils::clearInputBuffer(input);
    input.get();
}

// Function to display the menu to the user
int MenuManager::displayMenuOptions(std::istream& input, std::ostream& output) {
#ifdef UNIT_TESTING
    int choice;
    do {
        output << "====================="<< endl;
        output << "===== Main Menu =====" << endl;
        output << "====================="<< endl;
        output << static_cast<int>(MenuOption::FilterByGenre) << ". Filter by Genre" << endl;
        output << static_cast<int>(MenuOption::FilterByState) << ". Filter by State" << endl;
        output << static_cast<int>(MenuOption::FilterByCity) << ". Filter by City" << endl;
        output << static_cast<int>(MenuOption::FilterByCapacity) << ". Filter by Capacity" << endl;
        output << static_cast<int>(MenuOption::ClearSelectedVenues) << ". Clear Selected Venues" << endl;
        output << static_cast<int>(MenuOption::ViewSelectedVenues) << ". View Selected Venues" << endl;
        output << static_cast<int>(MenuOption::ShowEmailSettings) << ". Show Email Settings" << endl;
        output << static_cast<int>(MenuOption::ViewEditEmail) << ". View & Edit Email" << endl;
        output << static_cast<int>(MenuOption::FinishAndSendEmail) << ". Finish & Send Emails" << endl;
        output << static_cast<int>(MenuOption::Exit) << ". Exit VenueSender" << endl;
        output << "Enter your choice: ";

        if (!(input >> choice)) {
            output << "Invalid input, please enter a number. Press return to retry..." << endl;
            input.ignore();
            input.get();
        } else if (!isValidMenuChoice(choice)) {
            output << "Invalid choice. Please enter a number between " 
                 << static_cast<int>(MenuOption::FilterByGenre) 
                 << " and " 
                 << static_cast<int>(MenuOption::Exit) << "." << endl;
            output << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer(input);
            input.get();
        } else {
            break;
        }
    } while (true);

    return choice;
#else
    int choice;
    do {
        output << "====================="<< endl;
        output << "===== Main Menu =====" << endl;
        output << "====================="<< endl;
        output << static_cast<int>(MenuOption::FilterByGenre) << ". Filter by Genre" << endl;
        output << static_cast<int>(MenuOption::FilterByState) << ". Filter by State" << endl;
        output << static_cast<int>(MenuOption::FilterByCity) << ". Filter by City" << endl;
        output << static_cast<int>(MenuOption::FilterByCapacity) << ". Filter by Capacity" << endl;
        output << static_cast<int>(MenuOption::ClearSelectedVenues) << ". Clear Selected Venues" << endl;
        output << static_cast<int>(MenuOption::ViewSelectedVenues) << ". View Selected Venues" << endl;
        output << static_cast<int>(MenuOption::ShowEmailSettings) << ". Show Email Settings" << endl;
        output << static_cast<int>(MenuOption::ViewEditEmail) << ". View & Edit Email" << endl;
        output << static_cast<int>(MenuOption::FinishAndSendEmail) << ". Finish & Send Emails" << endl;
        output << static_cast<int>(MenuOption::Exit) << ". Exit VenueSender" << endl;
        output << "Enter your choice: ";

        if (!(input >> choice)) {
            output << "Invalid input, please enter a number. Press return to retry..." << endl;
            input.ignore();
            input.get();
        } else if (!isValidMenuChoice(choice)) {
            output << "Invalid choice. Please enter a number between " 
                 << static_cast<int>(MenuOption::FilterByGenre) 
                 << " and " 
                 << static_cast<int>(MenuOption::Exit) << "." << endl;
            output << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer(input);
            input.get();
        } else {
            break;
        }
    } while (true);

    return choice;
#endif
}

void MenuManager::displaySelectedVenues(const vector<SelectedVenue>& selectedVenues, istream& /*input*/, ostream& output) {
    output << "==========================="<< endl;
    output << "===== Selected Venues =====" << endl;
    output << "==========================="<< endl;
    if (selectedVenues.empty()) {
        output << "No venues selected yet." << endl;
    } else {
        for (const auto& venue : selectedVenues) {
            output << "Name: " << venue.name << endl;
            output << "Email: " << venue.email << endl;
            output << "City: " << venue.city << endl;
            output << "--------------------------" << endl;
        }
    }

    #ifndef UNIT_TESTING
        output << "===========================" << endl;
        output << "Press return to go back to Main Menu" << endl;
        ConsoleUtils::clearInputBuffer(/*input*/);
    #endif
}