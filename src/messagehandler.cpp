#include "messagehandler.h"

// Use the standard namespace
using namespace std;

void MessageHandler::handleMessageAndReturn(MessageType message) {

    // Switch statement to handle different types of messages
    switch (message) {

    // White Messages
        
        // Splashscreen Messages
        case MessageType::SPLASH_TITLE_VERSION_MESSAGE:
            cout << "                VenueSender                " << "\n" << endl;
            cout << "                  v0.0.1a                  " << endl;
            break;
        case MessageType::COPYRIGHT_INFO_MESSAGE:
            cout << "    Copyright (c) 2023 | Spencer Lievens    " << endl;
            break;
        case MessageType::INITIALIZATION_MESSAGE:
            cout << "              Initiailizing...              " << endl;
            break;
        case MessageType::INIT_ENTER_PASSWORD_MESSAGE:
            cout << "Enter your email password: ";
            break;
        case MessageType::INIT_CONFIRM_PASSWORD_MESSAGE:
            cout << endl << "Confirm your email password: ";
            break;

        // Venue Filtering Messages
        case MessageType::AVAILABLE_COUNTRIES_MESSAGE:
            cout << "Available Countries: ";
        break;
        case MessageType::SELECT_COUNTRY_MESSAGE:
            cout << "\nPlease select a country index: ";
        break;
        case MessageType::FINAL_VENUE_SELECTION_MESSAGE:
            cout << "Please select the final venue using (comma-separated) indices or type 'ALL': ";
        break;

    // end of white messages
    

    // Green Messages
        
        // System Message
        case MessageType::CONFIG_JSON_LOADED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Configuration from config.json Loaded" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;        
        
        //Email Config Messages
        case MessageType::PASSWORD_MATCHES_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << endl << "Password matches!" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::SETTINGS_UPDATED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Settings Updated" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;

        // Email Messages
        case MessageType::EMAIL_CREATION_SUCCESS_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Enter the path of the file to attach (or press Enter to skip): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_SEND_EMAIL_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do you wish to send this email? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_SEND_EMAILS_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do you wish to send this emails? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::EMAILS_SENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "===========================" << endl; 
            cout << "        Emails Sent        " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;

        // Template Messages
        case MessageType::CONFIRM_SEND_TEMPLATE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do you want to send the template? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_SEND_TEMPLATES_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do you wish to send these templates? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
    // end of green messages


    // Orange Messages

        // Menu Messages
        case MessageType::ENTER_CHOICE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter your choice: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::RETURN_TO_MAIN_MENU_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Returning to the Main Menu" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::EXITING_VENUESENDER_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Exiting VenueSender" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;


        // Email Config Messages
        case MessageType::USE_SSL_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Use SSL? (y/n): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::SSL_CERTIFICATE_PATH_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "SSL Cert Path (if unsure press enter on a new line): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::VERIFY_PEER_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Verify Peer? (y/n): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::VERIFY_HOST_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Verify Host? (y/n): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::VERBOSE_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Verbose? (y/n): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::SENDER_EMAIL_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Sender Email: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::SMTP_USERNAME_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "SMTP Username (this is usually your email address): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::SMTP_SERVER_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "SMTP Server: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::SMTP_PORT_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "SMTP Port (if unsure press enter on a new line): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::EMAIL_CONFIG_ENTER_PASSWORD_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter your email password: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::EMAIL_CONFIG_CONFIRM_PASSWORD_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << endl << "Confirm your email password: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::SMTP_PASSWORD_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Edit SMTP Password? (y/n): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;

        // Email Messages
        case MessageType::ENTER_SUBJECT_FOR_EMAIL_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter subject for the email (press Enter on a new line to finish): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::ENTER_MESSAGE_FOR_EMAIL_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter the message for the email (press Enter on a new line to finish):\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_ADD_ATTACHMENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Do you want to add an attachment? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ADD_ATTACHMENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter the path of the file to attach (or press Enter to skip): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ADD_DIFFERENT_ATTACHMENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Do you want to add a different attachment? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::SMTP_AUTH_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Authenticating with SMTP server..." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ENTER_CUSTOM_ADDRESS_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter the custom email address: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
    // end of orange messages


    // Red Messages

        // Menu Messages
        case MessageType::CAPS_LOCK_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Warning: Caps Lock is on!" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::VENUES_NOT_SELECTED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "===========================" << endl;
            cout << "    Venues Not Selected    " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::PRESS_RETURN_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Press return to go back..." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::EXIT_CONFIRMATION_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Are you sure you want to exit? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        

        // Email Messages
        case MessageType::ATTACHMENT_INVALID_CHOICE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Invalid choice. Please enter Y or N." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::EMAIL_NOT_CREATED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cout << "===========================" << endl;
        cout << "     Email Not Created     " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::MODIFY_EMAIL_CONFIRMATION_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif

        cout << "Do you wish to modify this email? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break;
        case MessageType::EMAIL_SENDING_FAILED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cout << "===========================" << endl;
        cout << "    Email Sending Failed   " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break;
        case MessageType::EMAIL_SENDING_ABORTED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cout << "Email sending aborted by user." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break; 

        // Template Messages
        case MessageType::VENUES_NOT_SELECTED_FOR_TEMPLATES_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "No venues have been selected. Please select venues first before attempting to send the templates." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::NO_BOOKING_TEMPLATES_CREATED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif

        cout << "No booking templates have been created yet." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break;
        case MessageType::MODIFY_TEMPLATE_CONFIRMATION_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif

        cout << "Do you wish to modify this template? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break;
        case MessageType::MANDATORY_TEMPLATE_FIELD_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif

        cout << "This field is mandatory. Please provide a value." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break;
        case MessageType::EMPTY_FIELD_CONFIRMATION_TEMPLATE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif

        cout << "You've left this field empty. Was this intentional? (y/n): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break;
        case MessageType::TEMPLATE_SENDING_ABORTED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif

        cout << "Template sending aborted by user." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
            break;
    // end of red messages
    }
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
}