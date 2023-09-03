#include "messagehandler.h"

// Use the standard namespace
using namespace std;

void MessageHandler::handleMessageAndReturn(MessageType message) {

    // Switch statement to handle different types of messages
    switch (message) {
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

        case MessageType::SETTINGS_UPDATED_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Settings Updated" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
        case MessageType::ENTER_CHOICE_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter your choice: ";
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
        case MessageType::EXITING_VENUESENDER_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Exiting VenueSender" << endl;
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
        case MessageType::SMTP_PASSWORD_CONFIG_MESSAGE:
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Edit SMTP Password? (y/n): ";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
            break;
    }
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
}