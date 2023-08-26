#include "include/main.h"

// Use relevant namespaces
using namespace confPaths;
using namespace std;

// Exclude the following code block if UNIT_TESTING is defined
#ifndef UNIT_TESTING

// Declare global objects to be used across different parts of the code
ConfigManager configManager;
CsvReader csvReader;
CurlHandleWrapper& curlWrapper = CurlHandleWrapper::getInstance();
EncryptionManager encryptionManager;
VenueFilter venueFilter;
VenueUtilities venueUtilities;

int main() {
    // Initialize necessary variables
    vector<Venue> venues;
    vector<SelectedVenue> selectedVenuesForTemplates;
    string configVenuesCsvPath, smtpServer, smtpUsername, mailPass, mailPassDecrypted, senderEmail, subject, message, attachmentName, attachmentPath, attachmentSize;
    int smtpPort;
    bool templateExists = false;
    bool useSSL, verifyPeer, verifyHost, verbose;

    // Load configurations from JSON file
    string venuesPathCopy = confPaths::venuesCsvPath;
    if (!ConfigManager::loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesPathCopy)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        exit(1);
    }

    // Attempt to decrypt the stored password
    mailPassDecrypted = encryptionManager.decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DECRYPTION_ERROR);
        exit(1);
    }

    // Set up and initialize CURL
    CurlHandleWrapper::init();
        CURL* curl = setupCurlHandle(curlWrapper, useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPassDecrypted, smtpPort, smtpServer);
    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return 1;  // Return error if CURL setup failed
    }

    // Read venues data from CSV file
    csvReader.readCSV(venues, confPaths::venuesCsvPath);

    // Extract unique genres, states, cities, and capacities from the venues data
    set<string> uniqueGenres = venueUtilities.getUniqueGenres(venues);
    set<string> uniqueStates = venueUtilities.getUniqueStates(venues);
    set<string> uniqueCities = venueUtilities.getUniqueCities(venues);
    set<int> uniqueCapacities = venueUtilities.getUniqueCapacities(venues);

    //Initialize variables for filters and selected venues
    FilterCriteria criteria;
    vector<SelectedVenue> selectedVenuesForEmail;
    vector<SelectedVenue> filteredVenues;

    // Initialize map for booking templates
    map<string, pair<string, string>> emailToTemplate;

    // Pass the configuration variables to the MenuManager constructor
    MenuManager menuManager(useSSL, verifyPeer, verifyHost, senderEmail, smtpPort, smtpServer);

    // Create an EmailManager object
    EmailManager emailManager;

    // Pass the necessary objects to the navigateMenus function
    menuManager.navigateMenus(emailManager, 
                              curl, 
                              venues,
                              selectedVenuesForTemplates,
                              selectedVenuesForEmail,
                              uniqueGenres,
                              uniqueStates,
                              uniqueCities,
                              uniqueCapacities,
                              temporaryFilteredVenues,
                              emailToTemplate,
                              subject,
                              message,
                              attachmentName,
                              attachmentPath,
                              attachmentSize,
                              venueFilter,
                              useSSL, 
                              verifyPeer, 
                              verifyHost, 
                              verbose, 
                              templateExists
                              );

    // Main loop for interacting with the user
    while (true) {
        MenuManager::mainHeader();

        // Display main menu options and get the user's choice
        int mainChoice = menuManager.displayMenuOptions();

        // Main Menu
        switch (mainChoice) {
            case MenuManager::VENUE_SELECTION_OPTION:
                menuManager.displayVenueSelectionOptions();
                break;
            case MenuManager::VENUE_OPTIONS_OPTION:
                menuManager.displayVenueOptions();
                break;
            case MenuManager::EMAIL_OPTIONS_OPTION:
                menuManager.displayEmailOptions();
                break;
            case MenuManager::TEMPLATES_OPTION:
                menuManager.displayTemplateOptions();
                break;
            case MenuManager::CONFIGURATION_OPTION:
                emailManager.viewEmailSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);
                break;
            default:
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
        }
    }

    return 0;
}
#endif // UNIT_TESTING
