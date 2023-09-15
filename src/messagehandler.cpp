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
            cout << "                v0.1.0 Beta                " << endl;
        break;
        case MessageType::COPYRIGHT_INFO_MESSAGE:
            cout << "    Copyright (c) 2023 | Spencer Lievens   " << endl;
        break;
        case MessageType::INITIALIZATION_MESSAGE:
            cout << "              Initializing...              " << endl;
        break;
        case MessageType::INIT_ENTER_PASSWORD_MESSAGE:
            cout << "Enter Your Email Password: ";
        break;
        case MessageType::INIT_CONFIRM_PASSWORD_MESSAGE:
            cout << endl << "Confirm Your Email Password: ";
        break;

        // Venue Filtering Messages
        case MessageType::AVAILABLE_COUNTRIES_MESSAGE:
            cout << "Available Countries: ";
        break;
        case MessageType::SELECT_COUNTRY_MESSAGE:
            cout << "\nPlease Select A Country Index: ";
        break;
        case MessageType::FINAL_VENUE_SELECTION_MESSAGE:
            cout << "Please Select The Final Venue Using (comma-separated) Indices Or Type 'ALL': ";
        break;

    // end of white messages
    

    // Green Messages
        
        // System Message
        case MessageType::CONFIG_JSON_LOADED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "config.json Configuration Loaded" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;  
        case MessageType::REGISTRATION_KEY_VALID_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Registration Key Valid" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::CSV_DATABASE_LOADED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "CSV Venue Database Loaded" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::SQLITE_DATABASE_LOADED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "SQLite Venues Database Loaded" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;       

        // Venue Filtering Messages
        case MessageType::SELECTED_VENUES_CLEARED:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "========================" << endl;
            cout << "Venues For Email Cleared" << endl;
            cout << "========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::SELECTED_VENUES_FOR_TEMPLATES_CLEARED:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "========================" << endl;
            cout << "Templates Venues Cleared " << endl;
            cout << "========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;      

        //Email Config Messages
        case MessageType::PASSWORD_MATCHES_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << endl << "Password Matches!" << endl;
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
            cout << "Email Created Successfully" "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_SEND_EMAIL_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do You Wish To Send This Email? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_SEND_EMAILS_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do You Wish To Send These Emails? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::EMAIL_SENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "===========================" << endl; 
            cout << "        Email Sent         " << endl;
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
        case MessageType::EMAILS_SAVED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "===========================" << endl;
            cout << "        Email Saved        " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;

        // Template Messages
        case MessageType::CONFIRM_SEND_TEMPLATE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do You Want To Send The Template? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_SEND_TEMPLATES_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Do You Wish To Send These Templates? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::BOOKING_TEMPLATES_CLEARED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "===========================" << endl;
            cout << " Booking Templates Cleared " << endl;
            cout << "===========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::BOOKING_TEMPLATE_ATTACHMENTS_CLEARED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "===========================" << endl;
            cout << "    Attachments Cleared    " << endl;
            cout << "===========================" << endl;
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
            cout << "Enter Your Choice: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::RETURN_TO_MAIN_MENU_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Returning To The Main Menu" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;

        // Email Config Messages
        case MessageType::USE_SSL_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Use SSL? (Y/N): ";
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
            cout << "Verify Peer? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::VERIFY_HOST_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Verify Host? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::VERBOSE_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Verbose? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;

        case MessageType::SMTP_MATCH_SENDER_EMAIL_CHECK_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Is The SMTP Username The Same As Sender Email? (Y/N): ";
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
            cout << "Enter Your Email Password: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::EMAIL_CONFIG_CONFIRM_PASSWORD_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << endl << "Confirm Your Email Password: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::SMTP_PASSWORD_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Edit SMTP Password? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;

        // Email Messages
        case MessageType::ENTER_SUBJECT_FOR_EMAIL_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter Subject For The Email (press enter on a new line to finish): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::ENTER_MESSAGE_FOR_EMAIL_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter The Message For The Email (press enter on a new line to finish):\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_ADD_ATTACHMENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Do You Want To Add An Attachment? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ADD_ATTACHMENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter The Path Of The File To Attach (or press enter to skip): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ADD_DIFFERENT_ATTACHMENT_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Do You Want To Add A Different Attachment? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::SMTP_AUTH_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Authenticating With SMTP Server..." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ENTER_CUSTOM_ADDRESS_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter The Custom Email Address: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CLEAR_EMAILS_CONFIRMATION_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Do you wish to clear the selected venues now you have emailed them? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;

        // Template Messages
        case MessageType::ENTER_PRESS_QUOTE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter Press Quote (press Enter on a new line to finish): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ENTER_QUOTE_SOURCE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter Quote Source (press Enter on a new line to finish): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::ADD_ANOTHER_QUOTE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Would you like to add another quote? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;        
    // end of orange messages


    // Red Messages

        // System Messages
        case MessageType::SMTP_SEND_LIMITS_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "============================================" << endl;
            cout << "          EMAIL/SMTP LIMIT WARNING          " << endl;
            cout << "============================================" << endl;
            cout << "   Check Daily Limits With Email Provider   " << endl;
            cout << "       (Press 'Enter' to continue...)       " << endl;                            
            cout << "============================================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;

        // Menu Messages
        case MessageType::VENUES_NOT_SELECTED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "===========================" << endl;
            cout << "    VENUES NOT SELECTED    " << endl;
            cout << "===========================" << endl;
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
            cout << "Are You Sure You Want To Exit? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::EXITING_VENUESENDER_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Exiting VenueSender" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;        

        // Email Messages
        case MessageType::ATTACHMENT_INVALID_CHOICE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "INVALID CHOICE PLEASE ENTER (Y/N)." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::EMAIL_NOT_CREATED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "===========================" << endl;
            cout << "     EMAIL NOT CREATED     " << endl;
            cout << "===========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::MODIFY_EMAIL_CONFIRMATION_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Do You Wish To Modify This Email? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break;
        case MessageType::EMAIL_SENDING_FAILED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "===========================" << endl;
            cout << "    EMAIL SENDING FAILED   " << endl;
            cout << "===========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break;
        case MessageType::EMAIL_SENDING_ABORTED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "EMAIL SENDING ABORTED BY USER" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break; 

        // Template Messages
        case MessageType::VENUES_NOT_SELECTED_FOR_TEMPLATES_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "NO VENUES SELECTED: Please Select Venues First Before Attempting To Send Templates." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
        break;
        case MessageType::NO_BOOKING_TEMPLATES_CREATED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "===========================" << endl;
            cout << "    NO TEMPLATE CREATED    " << endl;
            cout << "===========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break;
        case MessageType::MODIFY_TEMPLATE_CONFIRMATION_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Do You Wish To Modify This Template? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break;
        case MessageType::REDO_OR_MODIFY_TEMPLATE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "REDO The Entire Template, Or MODIFY Specific Fields? (REDO/MODIFY): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CHOOSE_FIELDS_TO_MODIFY_MESSSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter The Fields You Wish To Modify Using (comma-separated) Indices: ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;     
        case MessageType::CONFIRM_TEMPLATE_SATISFIED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Are You Satisfied With The Edited Template? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif      
        break;
        case MessageType::CONFIRM_TEMPLATE_FOR_SENDING_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Are You Satisfied With The Edited Template? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif  
        break;  
        case MessageType::MANDATORY_TEMPLATE_FIELD_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "THIS FIELD IS MANDATORY, PLEASE ENTER A VALUE" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break;
        case MessageType::EMPTY_FIELD_CONFIRMATION_TEMPLATE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "You've Left This Field Empty. Was This Intentional? (Y/N): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break;
        case MessageType::TEMPLATE_SENDING_ABORTED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "TEMPLATE SENDING ABORTED BY USER" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif               
        break;
    // end of red messages
  
        // Default Message
#ifdef UNIT_TESTING
        default:
            cout << "" << endl;
            break;
#else      
        default:
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "UNKNOWN MESSAGE TYPE" << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            break;
#endif
}
}