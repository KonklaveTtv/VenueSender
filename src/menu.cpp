#include "include/menu.h"

using namespace std;

int MenuManager::displayMenuOptions() {
    int choice;

    do {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
      //cout << "==========================="<< endl;
        cout << "         Main Menu         " << endl;
        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        // List out menu options
        cout << FILTER_BY_GENRE_OPTION << ". Filter by Genre" << endl;
        cout << FILTER_BY_STATE_OPTION << ". Filter by State" << endl;
        cout << FILTER_BY_CITY_OPTION << ". Filter by City" << endl;
        cout << FILTER_BY_CAPACITY_OPTION << ". Filter by Capacity" << endl;      
        cout << VIEW_SELECTED_VENUES_OPTION << ". View Selected Venues" << endl;
        cout << CLEAR_SELECTED_VENUES_OPTION << ". Clear Selected Venues" << endl;
        cout << CREATE_EMAIL_OPTION << ". Create Email" << endl;
        cout << VIEW_EDIT_EMAILS_OPTION << ". View & Edit Email" << endl;
        cout << EMAIL_CUSTOM_ADDRESS_OPTION << ". Email Custom Address" << endl;
        cout << SEND_EMAILS_OPTION << ". Send Emails" << endl;        
        cout << CREATE_VENUE_BOOKING_TEMPLATE_OPTION << ". Create Venue Booking Template" << endl;
        cout << VIEW_EDIT_BOOKING_TEMPLATES_OPTION << ". View & Edit Booking Template" << endl;
        cout << CLEAR_BOOKING_TEMPLATE_OPTION << ". Clear Booking Template" << endl;
        cout << SEND_BOOKING_TEMPLATES_OPTION << ". Send Booking Templates" << endl;        
        cout << SHOW_EMAIL_SETTINGS_OPTION << ". Show Email Configuration" << endl;        
        cout << EXIT_OPTION << ". Exit VenueSender" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Enter your choice: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        if (!(cin >> choice)) {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED); // Error in red
#endif
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_MENU_INPUT_ERROR);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();
        } else if (!isValidMenuChoice(choice)) {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED); // Error in red
#endif
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            cout << "Please choose a number between " << FILTER_BY_GENRE_OPTION << " and " << EXIT_OPTION << "." << endl;
        } else {
            break;
        }
    } while (true);

    return choice;
}

void MenuManager::displaySelectedVenues(const vector<SelectedVenue>& selectedVenues) {
    // Display header
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Green for venue listings
#endif
    cout << "==========================="<< endl;
    cout << "      Selected Venues      " << endl;
    cout << "==========================="<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    if (selectedVenues.empty()) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::NO_VENUES_SELECTED_ERROR);
    } else {
        for (const auto& venue : selectedVenues) {
            cout << "Name: " << venue.name << endl;
            cout << "Email: " << venue.email << endl;
            cout << "City: " << venue.city << endl;
            cout << "--------------------------" << endl;
        }
    }
}

bool MenuManager::handleExitOption() {
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
    cout << "Are you sure you want to exit? (Y/N): ";
    ConsoleUtils::resetColor();
    char confirmExit;
    cin >> confirmExit;

    if (confirmExit == 'Y' || confirmExit == 'y') {
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
        cout << "Exiting the program." << endl;
        ConsoleUtils::resetColor();
        return true;
    } else if (confirmExit == 'N' || confirmExit == 'n') {
        cout << "Returning to the main menu." << endl;
        return false;
    } else {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        return false;
    }
}
