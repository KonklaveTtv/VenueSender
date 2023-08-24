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
EmailManager emailManager;
EncryptionManager encryptionManager;
MenuManager menuManager;
VenueFilter venueFilter;
VenueUtilities venueUtilities;

int main() {
    // Initialize necessary variables
    vector<Venue> venues;
    string configVenuesCsvPath, smtpServer, smtpUsername, mailPass, mailPassDecrypted, senderEmail, subject, message, attachmentName, attachmentPath, attachmentSize;
    int smtpPort;
    bool useSSL, verifyPeer, verifyHost, verbose;

    // Load configurations from JSON file
    string venuesPathCopy = confPaths::venuesCsvPath;
    if (!configManager.loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesPathCopy)) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        exit(1);
    }

    // Attempt to decrypt the stored password
    mailPassDecrypted = encryptionManager.decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::DECRYPTION_ERROR);
        exit(1);
    }

    // Set up and initialize CURL
    CurlHandleWrapper::init();
        CURL* curl = setupCurlHandle(curlWrapper, useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPassDecrypted, smtpPort, smtpServer);
    if (!curl) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
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

    // Main loop for interacting with the user
    while (true) {
        // Display menu options and get user's choice
        
        int choice = menuManager.displayMenuOptions();
            // Handle different menu options
            if (choice >= static_cast<int>(MenuManager::MenuOption::FilterByGenre) &&
                choice <= MenuManager::FILTER_BY_CAPACITY_OPTION) {
                // Declare a temporary vector to store filtered venues
                vector<SelectedVenue> temporaryFilteredVenues;
                // Handle filtering options
                if (choice == MenuManager::FILTER_BY_GENRE_OPTION) {
                    // Filter by Genre
                    temporaryFilteredVenues = venueFilter.filterByOption(venues, "Genre", uniqueGenres, temporaryFilteredVenues);
                } else if (choice == MenuManager::FILTER_BY_STATE_OPTION) {
                    // Filter by State
                    temporaryFilteredVenues = venueFilter.filterByOption(venues, "State", uniqueStates, temporaryFilteredVenues);
                } else if (choice == MenuManager::FILTER_BY_CITY_OPTION) {
                    // Filter by City
                    temporaryFilteredVenues = venueFilter.filterByOption(venues, "City", uniqueCities, temporaryFilteredVenues);
                } else if (choice == MenuManager::FILTER_BY_CAPACITY_OPTION) {
                    // Filter by Capacity
                    temporaryFilteredVenues = venueFilter.filterByCapacity(venues, uniqueCapacities, temporaryFilteredVenues);
                }
                // Display venues that have been selected so far
                venueFilter.displayFilteredVenues(temporaryFilteredVenues);

                // Call the new function to process venue selection
                venueFilter.processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail);
            } else if (choice == MenuManager::VIEW_SELECTED_VENUES_OPTION) {
                // View Selected Venues
                menuManager.displaySelectedVenues(selectedVenuesForEmail);
            } else if (choice == MenuManager::CLEAR_SELECTED_VENUES_OPTION) {
                // Clear Selected Venues                
                selectedVenuesForEmail.clear();
                cout << "Selected venues cleared." << endl; 
            } else if (choice == MenuManager::SHOW_EMAIL_SETTINGS_OPTION) {
                // View Email Settings
                // Handle email editing or viewing                
                emailManager.viewEmailSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);
            } else if (choice == MenuManager::VIEW_EDIT_EMAILS_OPTION) {
                    emailManager.viewEditEmails(senderEmail, subject, message, attachmentName, attachmentSize, attachmentPath);
            } else if (choice == MenuManager::EMAIL_CUSTOM_ADDRESS_OPTION) {
                    emailManager.emailCustomAddress(curl, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
            } else if (choice == MenuManager::FINISH_AND_SEND_EMAILS_OPTION) {
                    emailManager.confirmSendEmail(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
            } else if (choice == MenuManager::EXIT_OPTION) {
                if (menuManager.handleExitOption()) {
                    break;
                }
            } else {                
                // Handle invalid menu choice
                ErrorHandler errorHandler;
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHOICE_ERROR);
                cin.get(); // This will wait for a key press
                }
            }
        // Reset configurations and cleanup before exiting
        configManager.resetConfigFile();  
        CurlHandleWrapper::cleanup(); // Assuming you have a cleanup method in your CurlHandleWrapper class that calls curl_global_cleanup();

    return 0;
}
#endif // UNIT_TESTING
