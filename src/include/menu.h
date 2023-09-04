#ifndef MENU_H
#define MENU_H

#include "curl.h"
#include "errorhandler.h"
#include "fileutils.h"
#include "filtercriteria.h"
#include "messagehandler.h"
#include "structs.h"

#include <iostream>
#include <utility>
#include <vector>

// Forward declaration of the EmailManager class
class EmailManager;

// Class to manage menu-related operations
class MenuManager {
private:
    bool useSSL;
    std::string sslCertPath;
    bool verifyPeer;
    bool verifyHost;
    bool verbose;
    std::string senderEmail;
    std::string mailPass;
    std::string smtpUsername;
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
        SelectVenues = 1,
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
        SendBookingTemplates,
        ClearBookingTemplate,
        ReturnToMainMenu
    };

    enum class ConfigurationOptions {
        ShowEmailSettings = 1,
        EditEmailSettings,
        ReturnToMainMenu
    };

    // Constants for Venue Filtering sub-menu options
    static constexpr int SELECT_VENUES_OPTION = static_cast<int>(VenueSelectionOption::SelectVenues);
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
    static constexpr int SEND_BOOKING_TEMPLATES_OPTION = static_cast<int>(TemplateOptions::SendBookingTemplates);
    static constexpr int CLEAR_BOOKING_TEMPLATE_OPTION = static_cast<int>(TemplateOptions::ClearBookingTemplate);
    static constexpr int RETURN_TO_MAIN_MENU_FROM_TEMPLATE_OPTIONS = static_cast<int>(TemplateOptions::ReturnToMainMenu);

    // Constants for Configuration sub-menu options
    static constexpr int SHOW_EMAIL_SETTINGS_OPTION = static_cast<int>(ConfigurationOptions::ShowEmailSettings);
    static constexpr int EDIT_EMAIL_SETTINGS_OPTION = static_cast<int>(ConfigurationOptions::EditEmailSettings);
    static constexpr int RETURN_TO_MAIN_MENU_FROM_CONFIGURATION_OPTIONS = static_cast<int>(ConfigurationOptions::ReturnToMainMenu);

    // Constants for Main Menu options
    static constexpr int VENUE_SELECTION_OPTION = static_cast<int>(MainMenuOption::VenueSelection);
    static constexpr int VENUE_OPTIONS_OPTION = static_cast<int>(MainMenuOption::VenueOptions);
    static constexpr int EMAIL_OPTIONS_OPTION = static_cast<int>(MainMenuOption::Email);
    static constexpr int TEMPLATES_OPTION = static_cast<int>(MainMenuOption::Templates);
    static constexpr int CONFIGURATION_OPTION = static_cast<int>(MainMenuOption::Configuration);
    static constexpr int EXIT_OPTION = static_cast<int>(MainMenuOption::Exit);

    // Constructor for MenuManager;
    MenuManager() = default;  // Default constructor
    MenuManager(bool useSSL, bool verifyPeer, bool verifyHost, bool verbose, std::string senderEmail, std::string mailPass, std::string smtpUsername, int smtpPort, std::string smtpServer)
                : useSSL(useSSL), verifyPeer(verifyPeer), verifyHost(verifyHost), verbose(verbose), senderEmail(std::move(senderEmail)), mailPass(std::move(mailPass)), smtpUsername(std::move(smtpUsername)), smtpPort(smtpPort), smtpServer(std::move(smtpServer)) {}

    // Function to nevigate the menu system
    bool navigateMenus(EmailManager& emailManager, 
                       CURL* curl, 
                       std::vector<Venue>& venues,
                       std::vector<SelectedVenue>& selectedVenuesForTemplates,
                       std::vector<SelectedVenue>& selectedVenuesForEmail,
                       std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate,
                       std::string& sslCertPath,
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
                       bool templateExists,
                       bool initColor,
                       CurlHandleWrapper& curlWrapper);

    // Function to show the main title header of VenueSender
    inline static void mainHeader() {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN); // Blue for headers
#endif
        std::cout << "===========================\n";
        std::cout << "        VenueSender        \n";
      //std::cout << "===========================\n";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    }

    // Functions to display the menu options to the user
    static int displayMenuOptions();
    static int displayVenueSelectionOptions();
    static int displayVenueOptions();
    static int displayEmailOptions();
    static int displayTemplateOptions();
    static int displayConfigurationOptions();

    void setupCurlHandle(CurlHandleWrapper& curlWrapper,
                                bool useSSL,
                                bool verifyPeer,
                                bool verifyHost,
                                const std::string& sslCertPath,
                                const std::string& smtpUsername,
                                const std::string& mailPass,
                                const std::string& senderEmail,
                                int smtpPort,
                                const std::string& smtpServer,
                                bool verbose);

    static bool editConfigurationSettings(bool& useSSL, std::string& sslCertPath, bool& verifyPeer, bool& verifyHost, bool& verbose, 
                                          std::string& senderEmail, std::string& smtpUsername, 
                                          std::string& mailPass, int& smtpPort, std::string& smtpServer, bool& initColor, CurlHandleWrapper& curlWrapper);

    // Function to display the list of venues selected by the user
    static void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

#ifdef UNIT_TESTING
    // Function to validate if the user's choice is a valid menu option
    static inline bool isValidMenuChoice(int choice) {
        return choice >= SELECT_VENUES_OPTION && choice <= EXIT_OPTION;
    }
#endif
};

#endif // MENU_H
