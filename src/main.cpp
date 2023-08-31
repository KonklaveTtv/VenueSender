#include "include/main.h"

// Use relevant namespaces
using namespace std;

// Global objects to be used across different parts of the code
CurlHandleWrapper& curlWrapper = CurlHandleWrapper::getInstance();
VenueFilter venueFilter;

// Exclude the following code block if UNIT_TESTING is defined
#ifndef UNIT_TESTING

int main() {
    // Load Splashscreen
    Init::splashscreen();

    // Initialize necessary variables
    vector<Venue> venues;
    vector<SelectedVenue> selectedVenuesForTemplates;
    string configVenuesCsvPath, smtpServer, smtpUsername, sslCertPath, mailPass, senderEmail, subject, message, attachmentName, attachmentPath, attachmentSize;
    int smtpPort;
    bool templateExists = false;
    bool initColor = true;
    bool useSSL, verifyPeer, verifyHost, verbose;

    // Load configurations from JSON file
    string venuesPathCopy = confPaths::venuesCsvPath;
    if (!ConfigManager::loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesPathCopy, initColor)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        exit(1);
    }

    // Set up and initialize CURL
    CurlHandleWrapper::init();
        CURL* curl = setupCurlHandle(curlWrapper, useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer);
    if (!curl) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return 1;
    }

    // Extract unique genres, states, cities, and capacities from the venues data
    set<string> uniqueGenres = VenueUtilities::getUniqueGenres(venues);
    set<string> uniqueCountries = VenueUtilities::getUniqueCountries(venues);
    set<int> uniqueCapacities = VenueUtilities::getUniqueCapacities(venues);
    set<string> uniqueStates = VenueUtilities::getUniqueStates(venues);
    set<string> uniqueCities = VenueUtilities::getUniqueCities(venues);

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

    // Create a VenueDatabaseReader object
    VenueDatabaseReader dbReader;

    if (!dbReader.initializeDatabaseAndReadVenueData(venues, confPaths::venuesCsvPath)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DATABASE_OPEN_ERROR);
        exit(1);
    }
    
    // Initialize map for booking templates
    map<string, pair<string, string>> emailToTemplate;

    // Pass the configuration variables to the MenuManager constructor
    MenuManager menuManager(useSSL, verifyPeer, verifyHost, verbose, senderEmail, mailPass, smtpUsername, smtpPort, smtpServer);

    // Create an EmailManager object
    EmailManager emailManager;

    // Pass the necessary objects to the navigateMenus function
    try {
        menuManager.navigateMenus(emailManager, 
                                  curl, 
                                  venues,
                                  selectedVenuesForTemplates,
                                  selectedVenuesForEmail,
                                  emailToTemplate,
                                  sslCertPath,
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
                                  templateExists,
                                  initColor,
                                  curlWrapper
                                  );
    } catch (const exception& e) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::MENU_LOAD_ERROR);
    }
    
    // Function load filters and display menu
    Init::Menu();

    // Clear and Empty the Email Password from memory
    fill(mailPass.begin(), mailPass.end(), '\0');
    mailPass.clear();

    return 0;
}
#endif // UNIT_TESTING
