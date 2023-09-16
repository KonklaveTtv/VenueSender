#ifndef MENU_H
#define MENU_H

#include "curl.h"
#include "errorhandler.h"
#include "fileutils.h"
#include "filtercriteria.h"
#include "messagehandler.h"
#include "menutitlehandler.h"
#include "structs.h"

#include <iostream>
#include <utility>
#include <vector>

// Forward declaration of the EmailManager class
class EmailManager;

// Class to manage menu-related operations
class MenuManager {
private:
    [[maybe_unused]]bool useSSL;
    std::string sslCertPath;
    [[maybe_unused]]bool verifyPeer;
    [[maybe_unused]]bool verifyHost;
    [[maybe_unused]]bool verbose;
    std::string senderEmail;
    std::string mailPass;
    std::string smtpUsername;
    int smtpPort;
    std::string smtpServer;


public:
    enum class MainMenuOption {
        VenueSelection = 1,
        VenueOptions = 2,
        Email = 3,
        Templates = 4,
        Configuration = 5,
        Exit = 6
    };

    enum class VenueSelectionOption {
        SelectVenuesForEmail = 1,
        SelectVenuesForTemplates = 2,
        ReturnToMainMenu = 3
    };

    enum class VenueOptions {
        ViewSelectedVenuesForEmail = 1,
        ViewSelectedVenuesForTemplates = 2,
        ClearSelectedVenuesForEmail = 3,
        ClearSelectedVenuesForTemplates = 4,
        ReturnToMainMenu = 5
    };

    enum class EmailOptions {
        CreateEmail = 1,
        ViewEditEmail = 2,
        EmailCustomAddress = 3,
        SendEmails = 4,
        ReturnToMainMenu = 5
    };

    enum class TemplateOptions {
        CreateVenueBookingTemplate = 1,
        ViewEditBookingTemplates = 2,
        ClearBookingTemplateAttachment = 3,
        AddBookingTemplateAttachment = 4,
        SendBookingTemplates = 5,
        ClearBookingTemplate = 6,
        ReturnToMainMenu = 7
    };

    enum class ConfigurationOptions {
        ShowEmailSettings = 1,
        EditEmailSettings = 2,
        ReturnToMainMenu = 3
    };

    // Constants for Venue Filtering sub-menu options
    static constexpr int SELECT_VENUES_FOR_EMAILS_OPTION = static_cast<int>(VenueSelectionOption::SelectVenuesForEmail);
    static constexpr int SELECT_VENUES_FOR_TEMPLATES_OPTION = static_cast<int>(VenueSelectionOption::SelectVenuesForTemplates);
    static constexpr int RETURN_TO_MAIN_MENU_FROM_VENUE_SELECTION = static_cast<int>(VenueSelectionOption::ReturnToMainMenu);

    // Constants for Venue Options sub-menu options
    static constexpr int VIEW_SELECTED_VENUES_FOR_EMAIL_OPTION = static_cast<int>(VenueOptions::ViewSelectedVenuesForEmail);
    static constexpr int VIEW_SELECTED_VENUES_FOR_TEMPLATES_OPTION = static_cast<int>(VenueOptions::ViewSelectedVenuesForTemplates);
    static constexpr int CLEAR_SELECTED_VENUES_FOR_EMAIL_OPTION = static_cast<int>(VenueOptions::ClearSelectedVenuesForEmail);
    static constexpr int CLEAR_SELECTED_VENUES_FOR_TEMPLATES_OPTION = static_cast<int>(VenueOptions::ClearSelectedVenuesForTemplates);
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
    static constexpr int CLEAR_BOOKING_TEMPLATE_ATTACHMENT_OPTION = static_cast<int>(TemplateOptions::ClearBookingTemplateAttachment);
    static constexpr int ADD_BOOKING_TEMPLATE_ATTACHMENT_OPTION = static_cast<int>(TemplateOptions::AddBookingTemplateAttachment);
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
                       std::vector<VenueForEmails>& venuesForEmails,
                       std::vector<VenueForTemplates>& venuesForTemplates,
                       std::vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                       std::vector<SelectedVenueForEmails>& selectedVenuesForEmails,
                       std::map<std::string, std::pair<std::string, std::string>>& templateForEmail,
                       std::string& sslCertPath,
                       std::string& subject,
                       std::string& message,
                       std::string& genre, 
                       std::string& performanceType, 
                       std::string& performanceName,
                       std::string& hometown, 
                       std::string& similarArtists, 
                       std::string& date,
                       std::string& musicLink, 
                       std::string& livePerfVideo, 
                       std::string& musicVideo,
                       std::vector<std::string>& pressQuotes, 
                       std::vector<std::string>& quoteSources, 
                       std::string& socials, 
                       std::string& name,
                       std::string& attachmentName,
                       std::string& attachmentPath,
                       std::string& attachmentSize,
                       std::string& templateAttachmentName,
                       std::string& templateAttachmentPath,
                       std::string& templateAttachmentSize,
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
    static void displaySelectedVenuesForEmails(const std::vector<SelectedVenueForEmails>& selectedVenuesForEmails);

    // Function to display the list of venues selected by the user
    static void displaySelectedVenuesForTemplates(const std::vector<SelectedVenueForTemplates>& selectedVenuesForTemplates);

#ifdef UNIT_TESTING
    // Function to validate if the user's choice is a valid menu option
    static inline bool isValidMenuChoice(int choice) {
        return choice >= SELECT_VENUES_FOR_EMAILS_OPTION && choice <= EXIT_OPTION;
    }
#endif
};

#endif // MENU_H
