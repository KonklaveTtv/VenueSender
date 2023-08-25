#ifndef MENU_H
#define MENU_H

#include "errorhandler.h"
#include "fileutils.h"
#include "structs.h"

#include <iostream>
#include <vector>

// Class to manage menu-related operations
class MenuManager {
public:
    // Enumeration representing the available options in the main menu
    enum class MenuOption {
        FilterByGenre = 1,
        FilterByState,
        FilterByCity,
        FilterByCapacity,
        ViewSelectedVenues,
        ClearSelectedVenues,
        CreateEmail,
        ViewEditEmail,
        EmailCustomAddress,
        SendEmails,
        CreateVenueBookingTemplate,
        ViewEditBookingTemplates,
        ClearBookingTemplate,
        sendBookingTemplateEmails,        
        ShowEmailSettings,
        Exit
    };

    // Constants for menu options related to filtering venues
    static constexpr int FILTER_BY_GENRE_OPTION = static_cast<int>(MenuOption::FilterByGenre);
    static constexpr int FILTER_BY_STATE_OPTION = static_cast<int>(MenuOption::FilterByState);
    static constexpr int FILTER_BY_CITY_OPTION = static_cast<int>(MenuOption::FilterByCity);
    static constexpr int FILTER_BY_CAPACITY_OPTION = static_cast<int>(MenuOption::FilterByCapacity);

    // Constants for other menu options
    static constexpr int VIEW_SELECTED_VENUES_OPTION = static_cast<int>(MenuOption::ViewSelectedVenues);
    static constexpr int CLEAR_SELECTED_VENUES_OPTION = static_cast<int>(MenuOption::ClearSelectedVenues);
    static constexpr int CREATE_EMAIL_OPTION = static_cast<int>(MenuOption::CreateEmail);
    static constexpr int VIEW_EDIT_EMAILS_OPTION = static_cast<int>(MenuOption::ViewEditEmail);
    static constexpr int EMAIL_CUSTOM_ADDRESS_OPTION = static_cast<int>(MenuOption::EmailCustomAddress);
    static constexpr int SEND_EMAILS_OPTION = static_cast<int>(MenuOption::SendEmails);
    static constexpr int CREATE_VENUE_BOOKING_TEMPLATE_OPTION = static_cast<int>(MenuOption::CreateVenueBookingTemplate);
    static constexpr int VIEW_EDIT_BOOKING_TEMPLATES_OPTION = static_cast<int>(MenuOption::ViewEditBookingTemplates);
    static constexpr int CLEAR_BOOKING_TEMPLATE_OPTION = static_cast<int>(MenuOption::ClearBookingTemplate);
    static constexpr int SEND_BOOKING_TEMPLATES_OPTION = static_cast<int>(MenuOption::sendBookingTemplateEmails);
    static constexpr int SHOW_EMAIL_SETTINGS_OPTION = static_cast<int>(MenuOption::ShowEmailSettings);
    static constexpr int EXIT_OPTION = static_cast<int>(MenuOption::Exit);

    // Function to show the main title header of VenueSender
    inline static void mainHeader() {
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
        std::cout << "===========================\n";
        std::cout << "        VenueSender        \n";
      //std::cout << "===========================\n";
        ConsoleUtils::resetColor();
    }

    // Function to validate if the user's choice is a valid menu option
    inline bool isValidMenuChoice(int choice) const {
        return choice >= FILTER_BY_GENRE_OPTION && choice <= EXIT_OPTION;
    }
    
    // Function to display the available menu options to the user
    int displayMenuOptions();

    // Function to display the list of venues selected by the user
    void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

    // Function to handle the exiting/cleanup of the program
    bool handleExitOption();
};

#endif // MENU_H
