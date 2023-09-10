#include "include/main.h"

// Use relevant namespaces
using namespace std;

// Global objects to be used across different parts of the code
CurlHandleWrapper& curlWrapper = CurlHandleWrapper::getInstance();
VenueFilter venueFilter;

// int main() is disabled during unit testing
#ifndef UNIT_TESTING

int main() {
    // Load Splashscreen
    Init::splashscreen();

    // Initialize necessary variables
    vector<VenueForEmails> venuesForEmails;
    vector<VenueForTemplates> venuesForTemplates;
    vector<SelectedVenueForEmails> selectedVenuesForEmails;
    vector<SelectedVenueForTemplates> selectedVenuesForTemplates;
    string configVenuesCsvPath, smtpServer, smtpUsername, sslCertPath, mailPass, senderEmail, subject, 
           message, genre, performanceType, performanceName, hometown, similarArtists, date, musicLink, livePerfVideo, 
           musicVideo, pressQuotes, quoteSources, socials, name, attachmentName, attachmentPath, attachmentSize, templateAttachmentName, templateAttachmentPath, templateAttachmentSize;
    int smtpPort = 587;
    bool initColor;
    bool templateExists = false;
    bool useSSL = false;
    bool verifyPeer = false;
    bool verifyHost = false;
    bool verbose = false;
try {
    // Set up and initialize CURL
    CurlHandleWrapper::init();
        CURL* curl = setupCurlHandle(curlWrapper, useSSL, sslCertPath, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer);
    if (!curl) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return 1;
    }

    // Load configurations from JSON file
    string venuesPathCopy = confPaths::venuesCsvPath;
    if (!ConfigManager::loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesPathCopy, initColor)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        exit(1);
    }

    // Pass the configuration variables to the MenuManager constructor
    MenuManager menuManager(useSSL, verifyPeer, verifyHost, verbose, senderEmail, mailPass, smtpUsername, smtpPort, smtpServer);

    // Extract unique genres, states, cities, and capacities from the venues data for emails
    set<string> uniqueGenresForEmails = VenueUtilities::getUniqueGenresForEmails(venuesForEmails);
    set<string> uniqueCountriesForEmails = VenueUtilities::getUniqueCountriesForEmails(venuesForEmails);
    set<int> uniqueCapacitiesForEmails = VenueUtilities::getUniqueCapacitiesForEmails(venuesForEmails);
    set<string> uniqueStatesForEmails = VenueUtilities::getUniqueStatesForEmails(venuesForEmails);
    set<string> uniqueCitiesForEmails = VenueUtilities::getUniqueCitiesForEmails(venuesForEmails);

    // Extract unique genres, states, cities, and capacities from the venues data for templates
    set<string> uniqueGenresForTemplates = VenueUtilities::getUniqueGenresForTemplates(venuesForTemplates);
    set<string> uniqueCountriesForTemplates = VenueUtilities::getUniqueCountriesForTemplates(venuesForTemplates);
    set<int> uniqueCapacitiesForTemplates = VenueUtilities::getUniqueCapacitiesForTemplates(venuesForTemplates);
    set<string> uniqueStatesForTemplates = VenueUtilities::getUniqueStatesForTemplates(venuesForTemplates);
    set<string> uniqueCitiesForTemplates = VenueUtilities::getUniqueCitiesForTemplates(venuesForTemplates);

    // Initialize variables for filters and selected venues for emails
    FilterCriteria criteria;
    vector<SelectedVenueForEmails> filteredVenuesForEmails;
    vector<SelectedVenueForEmails> temporaryFilteredVenuesForEmails;
    vector<SelectedVenueForEmails> temporaryCountryBufferForEmails;
    vector<SelectedVenueForEmails> temporaryStateBufferForEmails;
    vector<SelectedVenueForEmails> temporaryCityBufferForEmails;
    vector<SelectedVenueForEmails> temporaryCapacityBufferForEmails;
    vector<SelectedVenueForEmails> temporaryGenreBufferForEmails;

    // Initialize variables for filters and selected venues for templates
    vector<SelectedVenueForTemplates> filteredVenuesForTemplates;
    vector<SelectedVenueForTemplates> temporaryFilteredVenuesForTemplates;
    vector<SelectedVenueForTemplates> temporaryCountryBufferForTemplates;
    vector<SelectedVenueForTemplates> temporaryStateBufferForTemplates;
    vector<SelectedVenueForTemplates> temporaryCityBufferForTemplates;
    vector<SelectedVenueForTemplates> temporaryCapacityBufferForTemplates;
    vector<SelectedVenueForTemplates> temporaryGenreBufferForTemplates;
    
    // Create a VenueDatabaseReader object
    VenueDatabaseReader dbReader;

    if (!dbReader.initializeDatabaseAndReadVenueData(venuesForEmails, venuesForTemplates, confPaths::venuesCsvPath)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DATABASE_OPEN_ERROR);
        exit(1);
    }
    
    // Initialize map for booking templates
    map<string, pair<string, string>> templateForEmail;

    // Initialize the vectors for templates
    vector<string> pressQuotes;
    vector<string> quoteSources;

    // Create an EmailManager object
    EmailManager emailManager;

    // Pass the necessary objects to the navigateMenus function
    menuManager.navigateMenus(emailManager, 
                              curl, 
                              venuesForEmails,
                              venuesForTemplates,
                              selectedVenuesForTemplates,
                              selectedVenuesForEmails,
                              templateForEmail,
                              sslCertPath,
                              subject,
                              message,
                              genre, 
                              performanceType, 
                              performanceName,
                              hometown, 
                              similarArtists, 
                              date,
                              musicLink, 
                              livePerfVideo, 
                              musicVideo,
                              pressQuotes, 
                              quoteSources, 
                              socials, 
                              name,
                              attachmentName,
                              attachmentPath,
                              attachmentSize,
                              templateAttachmentName,
                              templateAttachmentPath,
                              templateAttachmentSize,
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
        cerr << "Caught exception: " << e.what() << endl;
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
