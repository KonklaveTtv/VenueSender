#include "include/main.h"

// Use relevant namespaces
using namespace std;

// Global objects to be used across different parts of the code
CurlHandleWrapper& curlWrapper = CurlHandleWrapper::getInstance();
VenueUtilities venueUtilities;
VenueFilter venueFilter;

// Exclude the following code block if UNIT_TESTING is defined
#ifndef UNIT_TESTING

int main() {
    // Load Splashscreen
    Init::splashscreen();

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
    mailPassDecrypted = EncryptionManager::decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DECRYPTION_ERROR);
        exit(1);
    }

    // Set up and initialize CURL
    CurlHandleWrapper::init();
        CURL* curl = setupCurlHandle(curlWrapper, useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPassDecrypted, smtpPort, smtpServer);
    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return 1;
    }

    // Read venues data from CSV file
    CsvReader::readCSV(venues, confPaths::venuesCsvPath);

    // Extract unique genres, states, cities, and capacities from the venues data
    set<string> uniqueGenres = venueUtilities.getUniqueGenres(venues);
    set<string> uniqueCountries = venueUtilities.getUniqueCountries(venues);
    set<int> uniqueCapacities = venueUtilities.getUniqueCapacities(venues);
    set<string> uniqueStates = venueUtilities.getUniqueStates(venues);
    set<string> uniqueCities = venueUtilities.getUniqueCities(venues);

    //Initialize variables for filters and selected venues
    FilterCriteria criteria;
    vector<SelectedVenue> selectedVenuesForEmail;
    vector<SelectedVenue> filteredVenues;
    vector<SelectedVenue> temporaryFilteredVenues;
    vector<SelectedVenue> temporaryCountryBuffer;
    vector<SelectedVenue> temporaryStateBuffer;
    vector<SelectedVenue> temporaryCityBuffer;
    vector<SelectedVenue> temporaryCapacityBuffer;
    vector<SelectedVenue> temporaryGenreBuffer;

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

    // Function load filters and display menu
    Init initInstance;
    initInstance.Menu();

    return 0;
}
#endif // UNIT_TESTING
