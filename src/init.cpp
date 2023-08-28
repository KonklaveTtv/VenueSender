#include "include/init.h"

using namespace confPaths;
using namespace std;

MenuManager menuManager;

void Init::splashscreen() {
    // Clear the console for the splashscreen
    ConsoleUtils::clearConsole();
    
    // Set color to cyan
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    for (int i = 0; i < 44; ++i) std::cout << '*';
    std::cout << std::endl;

    ConsoleUtils::resetColor();

    // Display splash text centered
    std::cout << "                VenueSender                " << std::endl;
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    std::cout << "********************************************" << std::endl;
    ConsoleUtils::resetColor();

    std::cout << "               Version 1.0.0               " << std::endl;

    // Display copyright and other text in cyan
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    std::cout << "********************************************" << std::endl;
    ConsoleUtils::resetColor();

    std::cout << "    Copyright (c) 2023, Spencer Lievens.    " << std::endl;
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);

    // Display bottom border
    for (int i = 0; i < 44; ++i) std::cout << '*';
    std::cout << std::endl;
    ConsoleUtils::resetColor();
    
    std::cout << "              Initiailizing...              " << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
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
                EmailManager::viewEmailSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);
                break;
            default:
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    }
}