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
                                bool initColor,
                                CurlHandleWrapper& curlWrapper) {
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
                            EmailManager::viewEmailSettings(useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, smtpPort, smtpServer);
                            continue;
                        case EDIT_EMAIL_SETTINGS_OPTION:
                            MenuManager::editConfigurationSettings(useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, 
                                          mailPass, smtpPort, smtpServer, initColor, curlWrapper);
                            continue;
                        case RETURN_TO_MAIN_MENU_FROM_CONFIGURATION_OPTIONS:
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
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::MAIN_MENU_HEADER);
        cout << VENUE_SELECTION_OPTION << ". Venue Selection" << endl;
        cout << VENUE_OPTIONS_OPTION << ". Venue Options" << endl;
        cout << EMAIL_OPTIONS_OPTION << ". Email" << endl;
        cout << TEMPLATES_OPTION << ". Templates" << endl;
        cout << CONFIGURATION_OPTION << ". Configuration" << endl;
        cout << EXIT_OPTION << ". Exit" << endl;
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_CHOICE_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();

        // Handle exit right here
        if (choice == EXIT_OPTION) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EXIT_CONFIRMATION_MESSAGE);

            char confirmExit;
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cin >> confirmExit;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();

            if (confirmExit == 'Y' || confirmExit == 'y') {
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EXITING_VENUESENDER_MESSAGE);
                // Cleanup before exiting
                CurlHandleWrapper::cleanup(); // Assuming you have a cleanup method in your CurlHandleWrapper class that calls curl_global_cleanup();
                exit(0);  // Exit the program
            } else if (confirmExit == 'N' || confirmExit == 'n') {
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::RETURN_TO_MAIN_MENU_MESSAGE);
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
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::MAIN_MENU_HEADER);
        cout << SELECT_VENUES_OPTION << ". Select Venues" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION << ". Return to Main Menu" << endl;

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_CHOICE_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();

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
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::VENUE_OPTIONS_MENU_HEADER);
        cout << VIEW_SELECTED_VENUES_OPTION << ". View Selected Venues" << endl;
        cout << CLEAR_SELECTED_VENUES_OPTION << ". Clear Selected Venues" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_VENUE_OPTIONS << ". Return to Main Menu" << endl;

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_CHOICE_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::EMAIL_MENU_HEADER);
        cout << CREATE_EMAIL_OPTION << ". Create Email" << endl;
        cout << VIEW_EDIT_EMAILS_OPTION << ". View & Edit Email" << endl;
        cout << EMAIL_CUSTOM_ADDRESS_OPTION << ". Email Custom Address" << endl;
        cout << SEND_EMAILS_OPTION << ". Send Emails" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_EMAIL_OPTIONS << ". Return to Main Menu" << endl;

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_CHOICE_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::TEMPLATES_MENU_HEADER);
        cout << CREATE_VENUE_BOOKING_TEMPLATE_OPTION << ". Create Venue Booking Template" << endl;
        cout << VIEW_EDIT_BOOKING_TEMPLATES_OPTION << ". View & Edit Booking Template" << endl;
        cout << SEND_BOOKING_TEMPLATES_OPTION << ". Send Booking Templates" << endl;
        cout << CLEAR_BOOKING_TEMPLATE_OPTION << ". Clear Booking Template" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_TEMPLATE_OPTIONS << ". Return to Main Menu" << endl;

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_CHOICE_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::CONFIGURATION_MENU_HEADER);
        cout << SHOW_EMAIL_SETTINGS_OPTION << ". Show Email Settings" << endl;
        cout << EDIT_EMAIL_SETTINGS_OPTION << ". Edit Email Settings" << endl;
        cout << RETURN_TO_MAIN_MENU_FROM_CONFIGURATION_OPTIONS << ". Return to Main Menu" << endl;

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_CHOICE_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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
                                            bool& initColor, CurlHandleWrapper& curlWrapper) {



    // Temporary variables to hold the new settings
    string senderEmailTempStr, smtpUsernameTempStr, smtpServerTempStr;
    int smtpPortTempInt;
    char tempSmtpChar, verifyPeerTempChar, verifyHostTempChar, verboseTempChar, smtpUsernameTempChar;

    // Prompt the user to enter new settings

    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::EDIT_CONFIGURATION_MENU_HEADER);

    // Edit Peer verification setting
    while (true) {

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::USE_SSL_CONFIG_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> tempSmtpChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        if (tempSmtpChar == 'y' || tempSmtpChar == 'Y') {
            useSSL = true;
            smtpPort = EmailManager::SSL_TLS_SMTP_PORT; // Automatically set the SMTP port based on SSL selection
            break;
        } else if (tempSmtpChar == 'n' || tempSmtpChar == 'N') {
            useSSL = false;
            smtpPort = EmailManager::DEFAULT_SMTP_PORT;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit SSL Cert Path
    while (true) {

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SSL_CERTIFICATE_PATH_CONFIG_MESSAGE);
        string sslCertPathInput;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        getline(cin, sslCertPathInput);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();


        // Remove quotes and trim spaces
        sslCertPathInput.erase(remove(sslCertPathInput.begin(), sslCertPathInput.end(), '\''), sslCertPathInput.end());
        sslCertPathInput = ConsoleUtils::trim(sslCertPathInput);

        // Whitespace check (full whitespace)
        if (count(sslCertPathInput.begin(), sslCertPathInput.end(), ' ') > 1 &&
            all_of(sslCertPathInput.begin(), sslCertPathInput.end(), ::isspace)) {
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

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::VERIFY_PEER_CONFIG_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> verifyPeerTempChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();

        if (verifyPeerTempChar == 'y' || verifyPeerTempChar == 'Y') {
            verifyPeer = true;
            break;
        } else if (verifyPeerTempChar == 'n' || verifyPeerTempChar == 'N') {
            verifyPeer = false;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit Host verification setting
    while (true) {

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::VERIFY_HOST_CONFIG_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> verifyHostTempChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();

        if (verifyHostTempChar == 'y' || verifyHostTempChar == 'Y') {
            verifyHost = true;
            break;
        } else if (verifyHostTempChar == 'n' || verifyHostTempChar == 'N') {
            verifyHost = false;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit verbose setting
    while (true) {

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::VERBOSE_CONFIG_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> verboseTempChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        
        if (verboseTempChar == 'y' || verboseTempChar == 'Y') {
            verbose = true;
            break;
        } else if (verboseTempChar == 'n' || verboseTempChar == 'N') {
            verbose = false;
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
        }
    }

    // Edit sender email
    while (true) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SENDER_EMAIL_CONFIG_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> senderEmailTempStr;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();

        if (!EmailManager::isValidEmail(senderEmailTempStr)) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
            continue;  // Loop back to ask for a valid email
        } else {
            senderEmail = senderEmailTempStr;
            break;  // Break the loop if a valid email is entered
        }
    }

    // Edit SMTP Username

    // Check if SMTP Username is the same as sender email
    char sameAsSenderEmail;
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_MATCH_SENDER_EMAIL_CHECK_MESSAGE);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cin >> sameAsSenderEmail;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor(); // Reset to default color
#endif
    ConsoleUtils::clearInputBuffer();

    if (sameAsSenderEmail == 'Y' || sameAsSenderEmail == 'y') {
        smtpUsername = senderEmailTempStr;
    } else if (smtpUsernameTempChar == 'n' || smtpUsernameTempChar == 'N') {
        // Edit SMTP Username
        while (true) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_USERNAME_CONFIG_MESSAGE);
    #ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    #endif
            cin >> smtpUsernameTempStr;
    #ifndef UNIT_TESTING
            ConsoleUtils::resetColor(); // Reset to default color
    #endif
            ConsoleUtils::clearInputBuffer();

            // Check for whitespace and control characters
            if (smtpUsernameTempStr.find_first_of(" \t\n\r\f\v") != string::npos) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                continue;
            }

            if (!EmailManager::isValidEmail(smtpUsernameTempStr)) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_USERNAME_NON_MATCH_ERROR);
    #ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    #endif
                cin >> smtpUsernameTempChar;
    #ifndef UNIT_TESTING
            ConsoleUtils::resetColor(); // Reset to default color
    #endif
                if (smtpUsernameTempChar == 'y' || smtpUsernameTempChar == 'Y') {
                    smtpUsername = smtpUsernameTempStr;
                    break;
                } else if (smtpUsernameTempChar == 'n' || smtpUsernameTempChar == 'N') {
                    continue;
                } else {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                }
            } else {
                smtpUsername = smtpUsernameTempStr;
                break;
            }
        }
    } else {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
    }

    // Edit SMTP Server
    while (true) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_SERVER_CONFIG_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> smtpServerTempStr;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();

        // Length check
        if (smtpServerTempStr.length() < EmailManager::MIN_SMTP_SERVER_LENGTH || smtpServerTempStr.length() > EmailManager::MAX_SMTP_SERVER_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_SERVER_LENGTH_ERROR, " Invalid length.");
            continue;
        }

        // Whitespace check
        if (smtpServerTempStr.find_first_of(" \t\n\r\f\v") != string::npos) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            continue;
        }

        // ANSI escape code check
        if (smtpServerTempStr.find("\033") != string::npos) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            continue;
        }

        smtpServer = smtpServerTempStr;
        break;
    }

    // Edit SMTP port
    while (true) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_PORT_CONFIG_MESSAGE);
        string portInput;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        getline(cin, portInput);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();

        // Whitespace check (full whitespace)
        if (count(portInput.begin(), portInput.end(), ' ') > 1 &&
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
                smtpPortTempInt = stoi(portInput);  // Convert the string to an integer
                if (smtpPortTempInt <= 0) {  // Check if the port number is positive
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_PORT_FORMAT_ERROR);
                    continue;
                }
                smtpPort = smtpPortTempInt;
                break;
            } catch (const invalid_argument&) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_PORT_FORMAT_ERROR);
                continue;
            }
        } else {
            // If the user left it blank, don't update the SMTP port
            break;
        }
    }

    // Confirm Edit SMTP Password
while (true) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_PASSWORD_CONFIG_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> tempSmtpChar;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset to default color
#endif
        ConsoleUtils::clearInputBuffer();
        
        if (tempSmtpChar == 'y' || tempSmtpChar == 'Y') {
            // Edit SMTP Password
            initColor = false;
            mailPass = ConsoleUtils::passwordEntry(initColor);
            break;
        } 
        else if (tempSmtpChar == 'n' || tempSmtpChar == 'N') {
            break;
        }
    }   
    
    // Update curl handle settings using the provided CurlHandleWrapper
    MenuManager menuManager;
    menuManager.setupCurlHandle(curlWrapper, useSSL, verifyPeer, verifyHost, sslCertPath, smtpUsername,
                    mailPass, senderEmail, smtpPort, smtpServer, verbose);

    EmailManager::viewEmailSettings(useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, smtpPort, smtpServer);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
    cout << "--------------------------" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SETTINGS_UPDATED_MESSAGE);

    return true;
}

void MenuManager::setupCurlHandle(CurlHandleWrapper& curlWrapper,
                                  bool useSSL,
                                  bool verifyPeer,
                                  bool verifyHost,
                                  const string& sslCertPath,
                                  const string& smtpUsername,
                                  const string& mailPass,
                                  const string& senderEmail,
                                  int smtpPort,
                                  const string& smtpServer,
                                  bool verbose) {
    CURL* curl = curlWrapper.get();
    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return;
    }

    // Set SSL options
    curlWrapper.setSSLOptions(useSSL, verifyPeer, verifyHost);

    // Set SSL certificate path if available
    if (!sslCertPath.empty()) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, sslCertPath.c_str());
    }

    // Set other options based on member variables
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUsername.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, mailPass.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);

    // SMTP Server Configuration
    string smtpUrl = "smtp://" + smtpServer + ":" + to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());
}

void MenuManager::displaySelectedVenues(const vector<SelectedVenue>& selectedVenues) {
    // Display menu header
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::EDIT_CONFIGURATION_MENU_HEADER);

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
