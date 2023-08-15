#include "main.h"

using namespace confPaths;

// remove if UnitTesting configuration is active
#ifndef UNIT_TESTING
int main() {
    // Initialize data and configuration settings
    vector<Venue> venues;
    string venuesCsvPath;
    string smtpServer;
    int smtpPort;
    string smtpPass;
    string smtpUsername;
    string mailPass;
    string smtpPassDecrypted;
    string mailPassDecrypted;
    string senderEmail;
    int senderSmtpPort;
    string subject;
    string message;
    double progress;
    bool useSSL;
    bool verifyPeer;
    bool verifyHost;

    initializeEncryptionParams();

    // Load the config settings from the JSON file
    if (!loadConfigSettings(smtpServer, smtpPort, smtpUsername, smtpPass, venuesCsvPath, mailPass, senderEmail, senderSmtpPort, useSSL, verifyPeer, verifyHost)) {
        cerr << "Failed to load configuration settings from config.json." << endl;
        exit(1); // Handle the error appropriately
    }
    cout << "useSSL in main: " << useSSL << endl;
    cout << "verifyPeer from config: " << verifyPeer << endl;
    cout << "verifyHost from config: " << verifyHost << endl;

    smtpPassDecrypted = decryptPassword(smtpPass);
    mailPassDecrypted = decryptPassword(mailPass);


    // Check if decryption was successful
    if (smtpPassDecrypted.empty() || mailPassDecrypted.empty()) {
        cerr << "Failed to decrypt passwords. Ensure they are correctly encrypted in config.json." << endl;
        exit(1); // Handle the error appropriately
    }

    // Initialize libcurl
    CurlHandleWrapper::init();

    // Create and manage CURL handle using CurlHandleWrapper
    CurlHandleWrapper curlWrapper;
    CURL* curl = curlWrapper.get();
    
    // Handle libcurl errors and display enhanced error messages
    if (!curl) {
        cerr << "Failed to initialize libcurl easy handle." << endl;
        return 1;
    }

    // Set SSL to True or False
    CURLcode res;
    res = curl_easy_setopt(curl, CURLOPT_USE_SSL, useSSL);
    if (res != CURLE_OK) {
        cerr << "Failed to set useSSL option." << endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Set verifyPeer to True or False
    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verifyPeer);
    if (res != CURLE_OK) {
        cerr << "Failed to set verifyPeer option." << endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Set verifyHost to True or False
    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verifyHost);
    if (res != CURLE_OK) {
        cerr << "Failed to set verifyHost option." << endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Connect to the SMTP server
    string smtpUrl = "smtp://" + smtpServer + ":" + to_string(smtpPort);
    res = curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());
    if (res != CURLE_OK) {
        cerr << "Failed to set libcurl URL option." << endl;
        return 1;
    }

    // Set the sender email address
    res = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());
    if (res != CURLE_OK) {
        cerr << "Failed to set libcurl sender email option." << endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Set the email password for authentication
    res = curl_easy_setopt(curl, CURLOPT_PASSWORD, mailPassDecrypted.c_str());
    if (res != CURLE_OK) {
        cerr << "Failed to set libcurl email password option." << endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Read venues data from CSV file
    readCSV(venues, venuesCsvPath);

    // Extract unique genres, states, cities, and capacities from the venues data
    set<string> uniqueGenres = getUniqueGenres(venues);
    set<string> uniqueStates = getUniqueStates(venues);
    set<string> uniqueCities = getUniqueCities(venues);
    set<int> uniqueCapacities = getUniqueCapacities(venues);

    FilterCriteria criteria;
    vector<SelectedVenue> selectedVenuesForEmail;
    vector<SelectedVenue> filteredVenues;

    // Set up progress callback using the progressCallback function
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &curlWrapper);

    int totalSelectedVenues = selectedVenuesForEmail.size();
    int emailSendProgress = 0;

    // Main loop for interacting with the user
    while (true) {
        // Display menu options and get user's choice
        int choice = displayMenuOptions();

            // Handle menu choices
            if (choice >= static_cast<int>(MenuOption::FilterByGenre) &&
                choice <= FILTER_BY_CAPACITY_OPTION) {
                // Declare a temporary vector to store filtered venues
                vector<SelectedVenue> temporaryFilteredVenues;

                // Handle filtering options
                if (choice == FILTER_BY_GENRE_OPTION) {
                    // Filter by Genre
                    temporaryFilteredVenues = filterByOption(venues, "Genre", uniqueGenres, temporaryFilteredVenues);
                } else if (choice == FILTER_BY_STATE_OPTION) {
                    // Filter by State
                    temporaryFilteredVenues = filterByOption(venues, "State", uniqueStates, temporaryFilteredVenues);
                } else if (choice == FILTER_BY_CITY_OPTION) {
                    // Filter by City
                    temporaryFilteredVenues = filterByOption(venues, "City", uniqueCities, temporaryFilteredVenues);
                } else if (choice == FILTER_BY_CAPACITY_OPTION) {
                    // Filter by Capacity
                    temporaryFilteredVenues = filterByCapacity(venues, uniqueCapacities, temporaryFilteredVenues);
                }

                // Display filtered venues
                displayFilteredVenues(temporaryFilteredVenues);

                // Call the new function to process venue selection
                processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail);
            } else if (choice == VIEW_SELECTED_VENUES_OPTION) {
                // View Selected Venues
                displaySelectedVenues(selectedVenuesForEmail);
            } else if (choice == CLEAR_SELECTED_VENUES_OPTION) {
                // Clear Selected Venues
                selectedVenuesForEmail.clear();
                cout << "Selected venues cleared." << endl;
            } else if (choice == static_cast<int>(MenuOption::ShowEmailSettings)) {
                // View Email Settings
                viewEmailSettings(smtpServer, smtpPort, senderEmail, senderSmtpPort, smtpPassDecrypted, mailPassDecrypted, useSSL, verifyHost, verifyPeer);
            } else if (choice == FINISH_AND_SEND_EMAILS_OPTION) {
            // Finish and Send Emails

                // Check if selectedVenuesForEmail is empty
                if (selectedVenuesForEmail.empty()) {
                    cout << "No venues selected. Please add venues before sending emails." << endl;
                    continue; // Return to the main menu
                }

                // Check if subject and message are empty
                if (subject.empty() || message.empty()) {
                    cout << "Subject and Message are required. Please set them before sending emails." << endl;

                    try {
                        constructEmail(subject, message, cin);
                    } catch (const exception& e) {
                        cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                        continue; // Return to the main menu
                    }

                    if (subject.empty() || message.empty()) {
                        cout << "Subject and Message are still empty. Returning to the main menu." << endl;
                        continue; // Return to the main menu
                    }
                }
                        
                // Check if SMTP password is empty
                if (smtpPassDecrypted.empty()) {
                    cout << "SMTP Password is required. Please set it before sending emails." << endl;
                    continue; // Return to the main menu
                }
            
            // Prompt for confirmation
            cout << "Confirm sending the email (Y/N): ";
            char confirmSend;
            cin >> confirmSend;
            clearInputBuffer();

            if (confirmSend == 'Y' || confirmSend == 'y') {
                // Proceed to send emails if confirmed
                emailSendProgress = 0; // Reset progress
                    for (const SelectedVenue& venue : selectedVenuesForEmail) {
                        sendIndividualEmail(curlWrapper.get(), venue, senderEmail, subject, message,
                                            smtpServer, smtpPort, smtpUsername, smtpPassDecrypted, progress);
                        ++emailSendProgress;

                    // Update the progress
                    curlWrapper.progressCallback(nullptr, emailSendProgress, totalSelectedVenues, 0, 0);

                    // Display email sending progress
                    viewEmailSendingProgress(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort, smtpUsername, smtpPassDecrypted, progress);
                }

                filteredVenues.clear(); // Clear the filtered venues for the next round of emails

                // Reset subject and message after sending emails
                subject.clear();
                message.clear();

            } else if (confirmSend == 'N' || confirmSend == 'n') {
                cout << "Email not sent. Returning to the main menu." << endl;
                // The user chose not to send the email, return to the main menu without clearing the selectedVenuesForEmail vector
            } else {
                cout << "Invalid choice. Please try again." << endl;
                // The user entered an invalid choice, return to the main menu without clearing the selectedVenuesForEmail vector
                }
            } else if (choice == EXIT_OPTION) {
                // Prompt for confirmation before exiting
                cout << "Are you sure you want to exit? (Y/N): ";
                char confirmExit;
                cin >> confirmExit;
                clearInputBuffer();

                if (confirmExit == 'Y' || confirmExit == 'y') {

                    // Exit VenueSender
                    cout << "Exiting the program." << endl;
                    break;
                } else if (confirmExit == 'N' || confirmExit == 'n') {
                    cout << "Returning to the main menu." << endl;
                    // The user chose not to exit, return to the main menu
                } else {
                    cout << "Invalid choice. Please try again." << endl;
                    // The user entered an invalid choice, return to the main menu
                }
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        }
        // Before the program exits, call the resetConfigFile function
        // to reset the flags and passwords in the config.json file
        resetConfigFile();   

    return 0;
}
#endif // UNIT_TESTING
