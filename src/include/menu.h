#ifndef MENU_H
#define MENU_H

#include "errorhandler.h"
#include "fileutils.h"
#include "filtercriteria.h"
#include "structs.h"

#include <iostream>
#include <vector>

// Forward declaration of the EmailManager class
class EmailManager;

// Class to manage menu-related operations
class MenuManager {
private:
    bool useSSL;
    bool verifyPeer;
    bool verifyHost;
    bool verbose;
    std::string senderEmail;
    int smtpPort;
    std::string smtpServer;


public:
    enum class MainMenuOption {
        VenueSelection = 1,
        VenueOptions,
        Email,
        Templates,
        Configuration,
        Exit
    };

    enum class VenueSelectionOption {
        FilterByGenre = 1,
        FilterByCountry,
        FilterByCapacity,
        FilterByState,
        FilterByCity,
        ReturnToMainMenu
    };

    enum class VenueOptions {
        ViewSelectedVenues = 1,
        ClearSelectedVenues,
        ReturnToMainMenu
    };

    enum class EmailOptions {
        CreateEmail = 1,
        ViewEditEmail,
        EmailCustomAddress,
        SendEmails,
        ReturnToMainMenu
    };

    enum class TemplateOptions {
        CreateVenueBookingTemplate = 1,
        ViewEditBookingTemplates,
        ClearBookingTemplate,
        SendBookingTemplates,
        ReturnToMainMenu
    };

    // Constants for Venue Filtering sub-menu options
    static constexpr int FILTER_BY_GENRE_OPTION = static_cast<int>(VenueSelectionOption::FilterByGenre);
    static constexpr int FILTER_BY_COUNTRY_OPTION = static_cast<int>(VenueSelectionOption::FilterByCountry);
    static constexpr int FILTER_BY_CAPACITY_OPTION = static_cast<int>(VenueSelectionOption::FilterByCapacity);
    static constexpr int FILTER_BY_STATE_OPTION = static_cast<int>(VenueSelectionOption::FilterByState);
    static constexpr int FILTER_BY_CITY_OPTION = static_cast<int>(VenueSelectionOption::FilterByCity);
    static constexpr int RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION = static_cast<int>(VenueSelectionOption::ReturnToMainMenu);

    // Constants for Venue Options sub-menu options
    static constexpr int VIEW_SELECTED_VENUES_OPTION = static_cast<int>(VenueOptions::ViewSelectedVenues);
    static constexpr int CLEAR_SELECTED_VENUES_OPTION = static_cast<int>(VenueOptions::ClearSelectedVenues);
    static constexpr int RETURN_TO_MAIN_MENU_FROM_VENUE_OPTIONS = static_cast<int>(VenueOptions::ReturnToMainMenu);

    // Constants for Email sub-menu options
    static constexpr int CREATE_EMAIL_OPTION = static_cast<int>(EmailOptions::CreateEmail);
    static constexpr int VIEW_EDIT_EMAILS_OPTION = static_cast<int>(EmailOptions::ViewEditEmail);
    static constexpr int EMAIL_CUSTOM_ADDRESS_OPTION = static_cast<int>(EmailOptions::EmailCustomAddress);
    static constexpr int SEND_EMAILS_OPTION = static_cast<int>(EmailOptions::SendEmails);
    static constexpr int RETURN_TO_MAIN_MENU_FROM_EMAIL_OPTIONS = static_cast<int>(EmailOptions::ReturnToMainMenu);

    // Constants for Templates sub-menu options
    static constexpr int CREATE_VENUE_BOOKING_TEMPLATE_OPTION = static_cast<int>(TemplateOptions::CreateVenueBookingTemplate);
    static constexpr int VIEW_EDIT_BOOKING_TEMPLATES_OPTION = static_cast<int>(TemplateOptions::ViewEditBookingTemplates);
    static constexpr int CLEAR_BOOKING_TEMPLATE_OPTION = static_cast<int>(TemplateOptions::ClearBookingTemplate);
    static constexpr int SEND_BOOKING_TEMPLATES_OPTION = static_cast<int>(TemplateOptions::SendBookingTemplates);
    static constexpr int RETURN_TO_MAIN_MENU_FROM_TEMPLATE_OPTIONS = static_cast<int>(TemplateOptions::ReturnToMainMenu);

    // Constants for Main Menu options
    static constexpr int VENUE_SELECTION_OPTION = static_cast<int>(MainMenuOption::VenueSelection);
    static constexpr int VENUE_OPTIONS_OPTION = static_cast<int>(MainMenuOption::VenueOptions);
    static constexpr int EMAIL_OPTIONS_OPTION = static_cast<int>(MainMenuOption::Email);
    static constexpr int TEMPLATES_OPTION = static_cast<int>(MainMenuOption::Templates);
    static constexpr int CONFIGURATION_OPTION = static_cast<int>(MainMenuOption::Configuration);
    static constexpr int EXIT_OPTION = static_cast<int>(MainMenuOption::Exit);

    // Constructor for MenuManager;
    MenuManager() = default;  // Default constructor
    MenuManager(bool useSSL, bool verifyPeer, bool verifyHost, const std::string& senderEmail, int smtpPort, const std::string& smtpServer)
        : useSSL(useSSL), verifyPeer(verifyPeer), verifyHost(verifyHost), senderEmail(senderEmail), smtpPort(smtpPort), smtpServer(smtpServer) {}

    // Function to nevigate the menu system
    void navigateMenus(EmailManager& emailManager, 
                       CURL* curl, 
                       std::vector<Venue>& venues,
                       std::vector<SelectedVenue>& selectedVenuesForTemplates,
                       std::vector<SelectedVenue>& selectedVenuesForEmail,
                       std::set<std::string>& uniqueGenres,
                       std::set<std::string>& uniqueCountries,
                       std::set<int>& uniqueCapacities,
                       std::set<std::string>& uniqueStates,
                       std::set<std::string>& uniqueCities,                                              
                       std::vector<SelectedVenue>& temporaryFilteredVenues,
                       std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate,
                       std::string& subject,
                       std::string& message,
                       std::string& attachmentName,
                       std::string& attachmentPath,
                       std::string& attachmentSize,
                       VenueFilter& venueFilter,
                       bool useSSL, 
                       bool verifyPeer, 
                       bool verifyHost, 
                       bool verbose, 
                       bool templateExists);

    // Function to show the main title header of VenueSender
    inline static void mainHeader() {
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
        std::cout << "===========================\n";
        std::cout << "        VenueSender        \n";
      //std::cout << "===========================\n";
        ConsoleUtils::resetColor();
    }

    // Functions to display the menu options to the user
    static int displayMenuOptions() ;
    static int displayVenueSelectionOptions() ;
    static int displayVenueOptions() ;
    static int displayEmailOptions() ;
    static int displayTemplateOptions() ;

    // Function to display the list of venues selected by the user
    static void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

#ifdef UNIT_TESTING
    // Function to validate if the user's choice is a valid menu option
    static inline bool isValidMenuChoice(int choice) {
        return choice >= FILTER_BY_GENRE_OPTION && choice <= EXIT_OPTION;
    }
#endif
};

#endif // MENU_H
