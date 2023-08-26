#include "include/menu.h"

#include "mail.h" // Forward declaration

using namespace std;

void MenuManager::navigateMenus(EmailManager& emailManager, 
                                CURL* curl, 
                                vector<Venue>& venues,
                                vector<SelectedVenue>& selectedVenuesForTemplates,
                                vector<SelectedVenue>& selectedVenuesForEmail,
                                set<string>& uniqueGenres,
                                set<string>& uniqueStates,
                                set<string>& uniqueCities,
                                set<int>& uniqueCapacities,
                                vector<SelectedVenue>& temporaryFilteredVenues,
                                map<string, pair<string, string>>& emailToTemplate,
                                string& subject,
                                string& message,
                                string& attachmentName,
                                string& attachmentPath,
                                string& attachmentSize,
                                VenueFilter& venueFilter,
                                bool useSSL, 
                                bool verifyPeer, 
                                bool verifyHost, 
                                bool verbose, 
                                bool templateExists) {

    // Pass the configuration variables to the MenuManager constructor
    //MenuManager menuManager(useSSL, verifyPeer, verifyHost, senderEmail, smtpPort, smtpServer);

    while (true) {
        mainHeader();

        int mainChoice = displayMenuOptions();

        switch (mainChoice) {
            case VENUE_SELECTION_OPTION: {
                int subChoice = displayVenueSelectionOptions();

                switch (subChoice) {
                    case FILTER_BY_GENRE_OPTION:
                        venueFilter.filterByOption(venues, "Genre", uniqueGenres, temporaryFilteredVenues);
                        VenueFilter::displayFilteredVenues(temporaryFilteredVenues); // Display filtered venues
                        VenueFilter::processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail, selectedVenuesForTemplates, cin, cout); // Select venues
                        temporaryFilteredVenues.clear(); // Clear the temporary filtered list
                        break;
                    case FILTER_BY_STATE_OPTION:
                        venueFilter.filterByOption(venues, "State", uniqueStates, temporaryFilteredVenues);
                        VenueFilter::displayFilteredVenues(temporaryFilteredVenues); // Display filtered venues
                        VenueFilter::processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail, selectedVenuesForTemplates, cin, cout); // Select venues
                        temporaryFilteredVenues.clear(); // Clear the temporary filtered list
                        break;
                    case FILTER_BY_CITY_OPTION:
                        venueFilter.filterByOption(venues, "City", uniqueCities, temporaryFilteredVenues);
                        VenueFilter::displayFilteredVenues(temporaryFilteredVenues); // Display filtered venues
                        VenueFilter::processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail, selectedVenuesForTemplates, cin, cout); // Select venues
                        temporaryFilteredVenues.clear(); // Clear the temporary filtered list
                        break;
                    case FILTER_BY_CAPACITY_OPTION:
                        venueFilter.filterByCapacity(venues, uniqueCapacities, temporaryFilteredVenues);
                        VenueFilter::displayFilteredVenues(temporaryFilteredVenues); // Display filtered venues
                        VenueFilter::processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail, selectedVenuesForTemplates, cin, cout); // Select venues
                        temporaryFilteredVenues.clear(); // Clear the temporary filtered list
                        break;
                    case RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION:
                        // Logic to return to the main menu
                        // No action needed, loop will start from the beginning
                        break;
                }

                break;
            }
            case VENUE_OPTIONS_OPTION: {
                int subChoice = displayVenueOptions();
                switch (subChoice) {
                    case VIEW_SELECTED_VENUES_OPTION:
                        displaySelectedVenues(selectedVenuesForEmail);
                        break;
                    case CLEAR_SELECTED_VENUES_OPTION:
                        EmailManager::clearSelectedVenues(selectedVenuesForEmail);
                        EmailManager::clearSelectedVenuesForTemplates(selectedVenuesForTemplates);
                        break;
                    case RETURN_TO_MAIN_MENU_FROM_VENUE_OPTIONS:
                        // Logic to return to the main menu
                        // No action needed, loop will start from the beginning
                        break;
                }
                break;
            }
            case EMAIL_OPTIONS_OPTION: {
                int subChoice = displayEmailOptions();
                switch (subChoice) {
                    case CREATE_EMAIL_OPTION:
                        emailManager.constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                        break;
                    case VIEW_EDIT_EMAILS_OPTION:
                        emailManager.viewEditEmails(curl, smtpServer, smtpPort, selectedVenuesForEmail, senderEmail, subject, message, attachmentName, attachmentSize, attachmentPath, templateExists, emailToTemplate);
                        break;
                    case EMAIL_CUSTOM_ADDRESS_OPTION:
                        emailManager.emailCustomAddress(curl, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
                        break;
                    case SEND_EMAILS_OPTION:
                        emailManager.confirmSendEmail(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
                        break;
                    case RETURN_TO_MAIN_MENU_FROM_EMAIL_OPTIONS:
                        // Logic to return to the main menu
                        // No action needed, loop will start from the beginning
                        break;
                }
                break;
            }
            case TEMPLATES_OPTION: {
                int subChoice = displayTemplateOptions();
                switch (subChoice) {
                    case CREATE_VENUE_BOOKING_TEMPLATE_OPTION:
                        emailManager.createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
                        break;
                    case VIEW_EDIT_BOOKING_TEMPLATES_OPTION:
                        emailManager.viewEditTemplates(curl, smtpServer, smtpPort, selectedVenuesForEmail, senderEmail, emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
                        break;
                    case CLEAR_BOOKING_TEMPLATE_OPTION:
                        EmailManager::clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
                        break;
                    case SEND_BOOKING_TEMPLATES_OPTION:
                        emailManager.confirmSendBookingTemplates(curl, selectedVenuesForTemplates, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
                        break;
                    case RETURN_TO_MAIN_MENU_FROM_TEMPLATE_OPTIONS:
                        // Logic to return to the main menu
                        // No action needed, loop will start from the beginning
                        break;
                }
                break;
            }
            case CONFIGURATION_OPTION:
                EmailManager::viewEmailSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);
                break;
            default:
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    }
}


int MenuManager::displayMenuOptions() {
    int choice;
    do {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
        cout << "==========================="<< endl;
        cout << "         Main Menu         "<< endl;
        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        cout << VENUE_SELECTION_OPTION << ". Venue Selection" << endl;
        cout << VENUE_OPTIONS_OPTION << ". Venue Options" << endl;
        cout << EMAIL_OPTIONS_OPTION << ". Email" << endl;
        cout << TEMPLATES_OPTION << ". Templates" << endl;
        cout << CONFIGURATION_OPTION << ". Configuration" << endl;
        cout << EXIT_OPTION << ". Exit" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE); // Blue for headers
#endif
        cout << "Enter your choice: ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        // Handle exit right here
        if (choice == EXIT_OPTION) {
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
            cout << "Are you sure you want to exit? (Y/N): ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
            char confirmExit;
            cin >> confirmExit;
            ConsoleUtils::clearInputBuffer();

            if (confirmExit == 'Y' || confirmExit == 'y') {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED); // Blue for headers
#endif
                cout << "Exiting the program." << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
                // Reset configurations and cleanup before exiting
                ConfigManager configManager;
                ConfigManager::resetConfigFile();
                CurlHandleWrapper::cleanup(); // Assuming you have a cleanup method in your CurlHandleWrapper class that calls curl_global_cleanup();
                exit(0);  // Exit the program
            } else if (confirmExit == 'N' || confirmExit == 'n') {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN); // Blue for headers
#endif                
                cout << "Returning to the main menu." << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
                continue;  // Continue to the next iteration of the loop
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
                continue;  // Continue to the next iteration of the loop
            }
        }

        if (choice >= VENUE_SELECTION_OPTION && choice <= EXIT_OPTION) {
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    } while (true);
    return choice;
}

int MenuManager::displayVenueSelectionOptions() {
    int choice;
    do {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
        cout << "==========================="<< endl;
        cout << "      Venue Selection      "<< endl;
        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        cout << FILTER_BY_GENRE_OPTION << ". Filter by Genre" << endl;
        cout << FILTER_BY_STATE_OPTION << ". Filter by State" << endl;
        cout << FILTER_BY_CITY_OPTION << ". Filter by City" << endl;
        cout << FILTER_BY_CAPACITY_OPTION << ". Filter by Capacity" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION << ". Return to Main Menu" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE); // Blue for headers
#endif
        cout << "Enter your choice: ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        if (choice >= FILTER_BY_GENRE_OPTION && choice <= RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION) {
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    } while (true);
    return choice;
}

int MenuManager::displayVenueOptions() {
    int choice;
    do {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
        cout << "==========================="<< endl;
        cout << "       Venue Options       "<< endl;
        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        cout << VIEW_SELECTED_VENUES_OPTION << ". View Selected Venues" << endl;
        cout << CLEAR_SELECTED_VENUES_OPTION << ". Clear Selected Venues" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_VENUE_OPTIONS << ". Return to Main Menu" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Enter your choice: ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        if (choice >= VIEW_SELECTED_VENUES_OPTION && choice <= RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION) {
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    } while (true);
    return choice;
}

int MenuManager::displayEmailOptions() const {
    int choice;
    do {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
        cout << "==========================="<< endl;
        cout << "           Email           "<< endl;
        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        cout << CREATE_EMAIL_OPTION << ". Create Email" << endl;
        cout << VIEW_EDIT_EMAILS_OPTION << ". View & Edit Email" << endl;
        cout << EMAIL_CUSTOM_ADDRESS_OPTION << ". Email Custom Address" << endl;
        cout << SEND_EMAILS_OPTION << ". Send Emails" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_EMAIL_OPTIONS << ". Return to Main Menu" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Enter your choice: ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        if (choice >= CREATE_EMAIL_OPTION && choice <= RETURN_TO_MAIN_MENU_FROM_EMAIL_OPTIONS) {
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    } while (true);
    return choice;
}

int MenuManager::displayTemplateOptions() const {
    int choice;
    do {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
      //cout << "==========================="<< endl;
        cout << "         Templates         "<< endl;
        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        cout << CREATE_VENUE_BOOKING_TEMPLATE_OPTION << ". Create Venue Booking Template" << endl;
        cout << VIEW_EDIT_BOOKING_TEMPLATES_OPTION << ". View & Edit Booking Template" << endl;
        cout << CLEAR_BOOKING_TEMPLATE_OPTION << ". Clear Booking Template" << endl;
        cout << SEND_BOOKING_TEMPLATES_OPTION << ". Send Booking Templates" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_TEMPLATE_OPTIONS << ". Return to Main Menu" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Enter your choice: ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        if (choice >= CREATE_VENUE_BOOKING_TEMPLATE_OPTION && choice <= RETURN_TO_MAIN_MENU_FROM_TEMPLATE_OPTIONS) {
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
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
    cout << "      Selected Venues      "<< endl;
    cout << "==========================="<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    if (selectedVenues.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::NO_VENUES_SELECTED_ERROR);
    } else {
        for (const auto& venue : selectedVenues) {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif
            cout << "Name: " << venue.name << endl;
            cout << "Email: " << venue.email << endl;
            cout << "City: " << venue.city << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "---------------------------"<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        }
    }
}
