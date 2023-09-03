#include "include/init.h"

using namespace confPaths;
using namespace std;

void Init::splashscreen() {
    // Clear the console for the splashscreen
    ConsoleUtils::clearConsole();
    
    // Set color to cyan
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    for (int i = 0; i < 44; ++i) cout << '*';
    cout << endl;

#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    // Display splash text centered
    cout << "                VenueSender                " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "********************************************" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    cout << "               Version 1.0.0               " << endl;

    // Display copyright and other text in cyan
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "********************************************" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    cout << "     Copyright (c)2023, Spencer Lievens     " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif

    // Display bottom border
    for (int i = 0; i < 44; ++i) cout << '*';
    cout << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    
    cout << "              Initiailizing...              " << endl;
    this_thread::sleep_for(chrono::seconds(2));
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
        cout << "============================================" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
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
        }
    }
}