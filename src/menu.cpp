#include "include/menu.h"

#include "mail.h" // Forward declaration

using namespace std;

bool MenuManager::navigateMenus(EmailManager& emailManager, 
                                CURL* curl, 
                                vector<Venue>& venues,
                                vector<SelectedVenue>& selectedVenuesForTemplates,
                                vector<SelectedVenue>& selectedVenuesForEmail,
                                map<string, pair<string, string>>& emailToTemplate,
                                string& sslCertPath,
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
                                bool templateExists,
                                bool initColor) {
    while (true) {
        mainHeader();

        int mainChoice = displayMenuOptions();

        switch (mainChoice) {
            case VENUE_SELECTION_OPTION: {
                while (true) {
                    int subChoice = displayVenueSelectionOptions();

                    switch (subChoice) {
                        case SELECT_VENUES_OPTION:
                            venueFilter.processVenueSelection(venues, selectedVenuesForEmail, selectedVenuesForTemplates, cin, cout);
                            continue;
                        case RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION:
                            // Logic to return to the main menu
                            // No action needed, loop will start from the beginning
                            break;
                    }
                    break;
                }
                break;
            }
            case VENUE_OPTIONS_OPTION: {
                while (true) {
                    int subChoice = displayVenueOptions();
                    
                    switch (subChoice) {
                        case VIEW_SELECTED_VENUES_OPTION:
                            displaySelectedVenues(selectedVenuesForEmail);
                            continue;
                        case CLEAR_SELECTED_VENUES_OPTION:
                            EmailManager::clearSelectedVenues(selectedVenuesForEmail);
                            EmailManager::clearSelectedVenuesForTemplates(selectedVenuesForTemplates);
                            continue;
                        case RETURN_TO_MAIN_MENU_FROM_VENUE_OPTIONS:
                            // Logic to return to the main menu
                            // No action needed, loop will start from the beginning
                            break;
                    }

                    break;
                }
                break;
            }
            case EMAIL_OPTIONS_OPTION: {
                while (true) {
                    int subChoice = displayEmailOptions();
                    switch (subChoice) {
                        case CREATE_EMAIL_OPTION:
                            emailManager.constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                            continue;
                        case VIEW_EDIT_EMAILS_OPTION:
                            emailManager.viewEditEmails(curl, smtpServer, smtpPort, selectedVenuesForEmail, senderEmail, subject, message, attachmentName, attachmentSize, attachmentPath, templateExists, emailToTemplate);
                            continue;
                        case EMAIL_CUSTOM_ADDRESS_OPTION:
                            emailManager.emailCustomAddress(curl, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
                            continue;
                        case SEND_EMAILS_OPTION:
                            EmailManager::confirmSendEmail(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
                            continue;
                        case RETURN_TO_MAIN_MENU_FROM_EMAIL_OPTIONS:
                            // Logic to return to the main menu
                            // No action needed, loop will start from the beginning
                            break;
                    }
                    break;
                }
                break;
            }
            case TEMPLATES_OPTION: {
                while (true) {
                    int subChoice = displayTemplateOptions();
                    switch (subChoice) {
                        case CREATE_VENUE_BOOKING_TEMPLATE_OPTION:
                            emailManager.createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
                            continue;
                        case VIEW_EDIT_BOOKING_TEMPLATES_OPTION:
                            emailManager.viewEditTemplates(curl, smtpServer, smtpPort, selectedVenuesForEmail, senderEmail, emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
                            continue;
                        case SEND_BOOKING_TEMPLATES_OPTION:
                            EmailManager::confirmSendBookingTemplates(curl, selectedVenuesForTemplates, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
                            continue;
                        case CLEAR_BOOKING_TEMPLATE_OPTION:
                            EmailManager::clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
                            continue;
                        case RETURN_TO_MAIN_MENU_FROM_TEMPLATE_OPTIONS:
                            // Logic to return to the main menu
                            // No action needed, loop will start from the beginning
                            break;
                    }
                    break;
                }
                break;
            }
            case CONFIGURATION_OPTION: {
                while (true) {
                    int subChoice = displayConfigurationOptions();
                    switch (subChoice) {
                        case SHOW_EMAIL_SETTINGS_OPTION:
                            EmailManager::viewEmailSettings(useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);
                            continue;
                        case EDIT_EMAIL_SETTINGS_OPTION:
                            MenuManager::editConfigurationSettings(useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, initColor);
                            continue;
                        case RETURN_TO_MAIN_MENU_FROM_CONFIGURATION_OPTIONS:
                            // Logic to return to the main menu
                            // No action needed, loop will start from the beginning
                            break;
                    }
                    break;
                }
                break;
            }
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
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
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
                cout << "Exiting VenueSender" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
                // Cleanup before exiting
                CurlHandleWrapper::cleanup(); // Assuming you have a cleanup method in your CurlHandleWrapper class that calls curl_global_cleanup();
                exit(0);  // Exit the program
            } else if (confirmExit == 'N' || confirmExit == 'n') {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN); // Blue for headers
#endif                
                cout << "Returning to the Main Menu" << endl;
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
        cout << SELECT_VENUES_OPTION << ". Select Venues" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION << ". Return to Main Menu" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE); // Blue for headers
#endif
        cout << "Enter your choice: ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        if (choice >= SELECT_VENUES_OPTION && choice <= RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION) {
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
        if (choice >= VIEW_SELECTED_VENUES_OPTION && choice <= RETURN_TO_MAIN_MENU_FROM_VENUE_OPTIONS) {
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    } while (true);
    return choice;
}

int MenuManager::displayEmailOptions() {
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

int MenuManager::displayTemplateOptions() {
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
        cout << SEND_BOOKING_TEMPLATES_OPTION << ". Send Booking Templates" << endl;
        cout << CLEAR_BOOKING_TEMPLATE_OPTION << ". Clear Booking Template" << endl;
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

int MenuManager::displayConfigurationOptions() {
    int choice;
    do {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
        cout << "==========================="<< endl;
        cout << "       Configuration       "<< endl;
        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        cout << SHOW_EMAIL_SETTINGS_OPTION << ". Show Email Settings" << endl;
        cout << EDIT_EMAIL_SETTINGS_OPTION << ". Edit Email Settings" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_CONFIGURATION_OPTIONS << ". Return to Main Menu" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Enter your choice: ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        if (choice >= SHOW_EMAIL_SETTINGS_OPTION && choice <= RETURN_TO_MAIN_MENU_FROM_CONFIGURATION_OPTIONS) {
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    } while (true);
    return choice;
}

// Function to override the settings set by config.json
bool MenuManager::editConfigurationSettings(bool& useSSL, string& sslCertPath, bool& verifyPeer, bool& verifyHost, bool& verbose, 
                                            string& senderEmail, string& smtpUsername, 
                                            string& mailPass, int& smtpPort, string& smtpServer,
                                            bool& initColor) {



    // Temporary variables to hold the new settings
    string tempStr;
    int tempInt;
    char tempChar;

    // Prompt the user to enter new settings

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "===========================" << endl;
    cout << "Edit Configuration Settings" << endl;
    cout << "===========================" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif

    // Edit Peer verification setting
    while (true) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Use SSL? (y/n): ";
        cin >> tempChar;
        ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        if (tempChar == 'y' || tempChar == 'Y') {
            useSSL = true;
            smtpPort = 465; // Automatically set the SMTP port based on SSL selection
            break;
        } else if (tempChar == 'n' || tempChar == 'N') {
            useSSL = false;
            smtpPort = 587;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit SSL Cert Path
    while (true) {
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "SSL Cert Path (if unsure press enter on a new line): ";
        string sslCertPathInput;
        getline(cin, sslCertPathInput);
        ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

        // Remove quotes and trim spaces
        sslCertPathInput.erase(remove(sslCertPathInput.begin(), sslCertPathInput.end(), '\''), sslCertPathInput.end());
        sslCertPathInput = ConsoleUtils::trim(sslCertPathInput);

        // Whitespace check (full whitespace)
        if (all_of(sslCertPathInput.begin(), sslCertPathInput.end(), ::isspace)) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SSL_CERT_PATH_ERROR);
            continue;
        }

        // ANSI escape code check
        if (sslCertPathInput.find("\033") != string::npos) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            continue;
        }

        if (!sslCertPathInput.empty()) {
            // Check if the path exists
            if (!ConsoleUtils::fileExists(sslCertPathInput)) { // check sslCertPathInput
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SSL_CERT_PATH_ERROR);
                continue;
            } else {
                sslCertPath = sslCertPathInput;  // then update sslCertPath
                break;
            }
        } else {
            // If the user left it blank, don't update the SSL cert path
            break;
        }
    }


    // Edit Peer verification setting
    while (true) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Verify Peer? (y/n): ";
        cin >> tempChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        
        ConsoleUtils::clearInputBuffer();

        if (tempChar == 'y' || tempChar == 'Y') {
            verifyPeer = true;
            break;
        } else if (tempChar == 'n' || tempChar == 'N') {
            verifyPeer = false;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit Host verification setting
    while (true) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Verify Host? (y/n): ";
        cin >> tempChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        
        ConsoleUtils::clearInputBuffer();
        
        if (tempChar == 'y' || tempChar == 'Y') {
            verifyHost = true;
            break;
        } else if (tempChar == 'n' || tempChar == 'N') {
            verifyHost = false;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit verbose setting
    while (true) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Verbose? (y/n): ";
        cin >> tempChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        
        ConsoleUtils::clearInputBuffer();
        
        if (tempChar == 'y' || tempChar == 'Y') {
            verbose = true;
            break;
        } else if (tempChar == 'n' || tempChar == 'N') {
            verbose = false;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit sender email
    while (true) {
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Sender Email: ";
        cin >> tempStr;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

        ConsoleUtils::clearInputBuffer();
        if (!EmailManager::isValidEmail(tempStr)) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
            continue;  // Loop back to ask for a valid email
        } else {
            senderEmail = tempStr;
            break;  // Break the loop if a valid email is entered
        }
    }


    // Edit SMTP Username
    while (true) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "SMTP Username (this is usually your email address): ";
        cin >> tempStr;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

        // Check for whitespace and control characters
        if (tempStr.find_first_of(" \t\n\r\f\v") != string::npos) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            continue;
        }

        if (!EmailManager::isValidEmail(tempStr)) {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Your SMTP Username is not an email, is this correct? (y/n): ";
            cin >> tempChar;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();

            if (tempChar == 'y' || tempChar == 'Y') {
                smtpUsername = tempStr;
                break;
            } else if (tempChar == 'n' || tempChar == 'N') {
                continue;
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            }
        } else {
            smtpUsername = tempStr;
            break;
        }
    }

    // Edit SMTP Server
    while (true) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "SMTP Server: ";
        cin >> tempStr;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

        // Length check
        if (tempStr.length() < EmailManager::MIN_SMTP_SERVER_LENGTH || tempStr.length() > EmailManager::MAX_SMTP_SERVER_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_SERVER_LENGTH_ERROR, " Invalid length.");
            continue;
        }

        // Whitespace check
        if (tempStr.find_first_of(" \t\n\r\f\v") != string::npos) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            continue;
        }

        // ANSI escape code check
        if (tempStr.find("\033") != string::npos) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            continue;
        }

        smtpServer = tempStr;
        break;
    }

    // Edit SMTP port
    while (true) {
    #ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    #endif
        cout << "SMTP Port (if unsure press enter on a new line): ";
        string portInput;
        getline(cin, portInput);  // Using getline to capture the full line, which allows empty input
    #ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
    #endif
        ConsoleUtils::clearInputBuffer();

        // Whitespace check (full whitespace)
        if (std::count(portInput.begin(), portInput.end(), ' ') > 1 &&
            all_of(portInput.begin(), portInput.end(), ::isspace)) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_PORT_FORMAT_ERROR);
            continue;
        }

        // ANSI escape code check
        if (portInput.find("\033") != string::npos) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            continue;
        }

        if (!portInput.empty()) {
            try {
                tempInt = stoi(portInput);  // Convert the string to an integer
                if (tempInt <= 0) {  // Check if the port number is positive
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_PORT_FORMAT_ERROR);
                    continue;
                }
                smtpPort = tempInt;
                break;
            } catch (const std::invalid_argument&) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_PORT_FORMAT_ERROR);
                continue;
            }
        } else {
            // If the user left it blank, don't update the SMTP port
            break;
        }
    }

    // Edit Mail password
    mailPass = ConsoleUtils::passwordEntry(initColor);  // Assuming the passwordEntry function is available within the same class or public

    EmailManager::viewEmailSettings(useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
    cout << "--------------------------" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
    cout << "Settings Updated" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    return true;
}

void MenuManager::displaySelectedVenues(const vector<SelectedVenue>& selectedVenues) {
    // Display header
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
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
            ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
            cout << "Name: " << venue.name << endl;
            cout << "Country: " << venue.country << endl;
            cout << "State: " << venue.state << endl;
            cout << "City: " << venue.city << endl;
            cout << "Capacity: " << venue.capacity << endl;
            cout << "Genre: " << venue.genre << endl;

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
