#include "include/init.h"

// Use the standard namespace and confPaths
using namespace confPaths;
using namespace std;

void Init::splashscreen() {
    // Clear the console for the splashscreen
    ConsoleUtils::clearConsole();
    
    // Set color to cyan
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    for (int i = 0; i < BORDER_LENGTH; ++i) cout << '*';
    cout << endl;

#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    // Display splash text centered
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SPLASH_TITLE_VERSION_MESSAGE);
    cin.clear();

    // Display copyright and other text in cyan
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    for (int i = 0; i < BORDER_LENGTH; ++i) cout << '*';
    cout << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::COPYRIGHT_INFO_MESSAGE);
    cin.clear();
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif

    // Display bottom border
    for (int i = 0; i < BORDER_LENGTH; ++i) cout << '*';
    cout << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::INITIALIZATION_MESSAGE);
    cin.clear();
    this_thread::sleep_for(chrono::seconds(1));
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    for (int i = 0; i < BORDER_LENGTH; ++i) cout << '=';
    cout << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
}

void Init::limitsWarning() {
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_SEND_LIMITS_MESSAGE);
    ConsoleUtils::clearInputBuffer();
    ConsoleUtils::clearConsole();
}

void Init::Menu() {
    // Main loop for interacting with the user
    while (true) {
        MenuManager::mainHeader();

        // Display main menu options and get the user's choice
        int mainChoice = MenuManager::displayMenuOptions();

        // Main Menu
        switch (mainChoice) {
            case MenuManager::VENUE_SELECTION_OPTION:
                MenuManager::displayVenueSelectionOptions();
                break;
            case MenuManager::VENUE_OPTIONS_OPTION:
                MenuManager::displayVenueOptions();
                break;
            case MenuManager::EMAIL_OPTIONS_OPTION:
                MenuManager::displayEmailOptions();
                break;
            case MenuManager::TEMPLATES_OPTION:
                MenuManager::displayTemplateOptions();
                break;
            case MenuManager::CONFIGURATION_OPTION:
                MenuManager::displayConfigurationOptions();
                break;
            default:
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
                cin.clear();
        }
    }
}