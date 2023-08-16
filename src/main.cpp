#include "main.h"

using namespace confPaths;
using namespace std;

// Exclude the following code block if UNIT_TESTING is defined
#ifndef UNIT_TESTING

int main() {
    // Initialize data and configuration settings
    vector<Venue> venues;
    string venuesCsvPath, smtpServer, smtpUsername, mailPass, mailPassDecrypted, senderEmail, subject, message;
    int smtpPort;
    double progress;
    bool useSSL, verifyPeer, verifyHost;

    initializeEncryptionParams();

    // Load the config settings from the JSON file
    if (!loadConfigSettings(useSSL, verifyPeer, verifyHost, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesCsvPath)) {
        cerr << "Failed to load configuration settings from config.json." << endl;
        exit(1); // Handle the error appropriately
    }

    // Attempt to decrypt the stored password
    mailPassDecrypted = decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        cerr << "Failed to decrypt passwords. Ensure they are correctly encrypted in config.json." << endl;
        exit(1);  // Exit with an error status
    }

    // Set up and initialize CURL
    CurlHandleWrapper curlWrapper;
    CurlHandleWrapper::init();
    CURL* curl = setupCurlHandle(curlWrapper, useSSL, verifyPeer, verifyHost, senderEmail, smtpUsername, mailPassDecrypted, smtpPort, smtpServer);
    if (!curl) {
        return 1;  // Return error if CURL setup failed
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

    int totalSelectedVenues = selectedVenuesForEmail.size();
    int emailSendProgress = 0;


    // Main loop for interacting with the user
    while (true) {
        // Display menu options and get user's choice
        clearConsole();
        int choice = displayMenuOptions();

            // Handle menu choices
            if (choice >= static_cast<int>(MenuOption::FilterByGenre) &&
                choice <= FILTER_BY_CAPACITY_OPTION) {
                // Declare a temporary vector to store filtered venues
                vector<SelectedVenue> temporaryFilteredVenues;

                // Handle filtering options
                if (choice == FILTER_BY_GENRE_OPTION) {
                    // Filter by Genre
                    clearConsole();
                    temporaryFilteredVenues = filterByOption(venues, "Genre", uniqueGenres, temporaryFilteredVenues);
                } else if (choice == FILTER_BY_STATE_OPTION) {
                    // Filter by State
                    clearConsole();
                    temporaryFilteredVenues = filterByOption(venues, "State", uniqueStates, temporaryFilteredVenues);
                } else if (choice == FILTER_BY_CITY_OPTION) {
                    // Filter by City
                    clearConsole();
                    temporaryFilteredVenues = filterByOption(venues, "City", uniqueCities, temporaryFilteredVenues);
                } else if (choice == FILTER_BY_CAPACITY_OPTION) {
                    // Filter by Capacity
                    clearConsole();
                    temporaryFilteredVenues = filterByCapacity(venues, uniqueCapacities, temporaryFilteredVenues);
                }

                // Display filtered venues
                displayFilteredVenues(temporaryFilteredVenues);

                // Call the new function to process venue selection
                processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail);
            } else if (choice == VIEW_SELECTED_VENUES_OPTION) {
                // View Selected Venues
                clearConsole();
                displaySelectedVenues(selectedVenuesForEmail);
            } else if (choice == CLEAR_SELECTED_VENUES_OPTION) {
                // Clear Selected Venues
                clearConsole();
                selectedVenuesForEmail.clear();
                cout << "Selected venues cleared." << endl;
                cout << "Press any key to return to the Main Menu..." << endl;
                cin.ignore();  // If there's a chance you might have used cin before this point
                clearInputBuffer();
                cin.get();     // This will wait for a key press                
            } else if (choice == SHOW_EMAIL_SETTINGS_OPTION) {
                // View Email Settings
                clearConsole();
                viewEmailSettings(useSSL, verifyPeer, verifyHost, senderEmail, mailPassDecrypted, smtpPort, smtpServer);
            } else if (choice == VIEW_EDIT_EMAILS_OPTION) {
                clearConsole();
                int attempts = 0;
                bool modified = false;

                while (attempts < 3) {
                    cout << "----- EMAIL DETAILS -----\n";
                    cout << "Subject: " << subject << endl;
                    cout << "-------------------------\n";
                    cout << "Message: \n" << message << endl;
                    cout << "-------------------------\n";

                    cout << "Do you wish to modify this email? (Y/N): ";
                    char modifyEmailChoice;
                    cin >> modifyEmailChoice;
                    clearInputBuffer();
                    clearConsole();
                    if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
                        subject.clear(); // Clear existing subject
                        message.clear(); // Clear existing message
                        clearConsole();                        
                        try {
                            constructEmail(subject, message, cin);
                            modified = true;
                        } catch (const exception& e) {
                            cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            continue; // Loop back to prompt for email details again
                        }
                    } else {
                        clearConsole();
                        cout << "Email saved for sending/editing." << endl;
                        cout << "Press any key to return to the Main Menu..." << endl;
                        cin.ignore();  // If there's a chance you might have used cin before this point
                        clearInputBuffer();
                        cin.get();
                        break;
                    }

                    clearConsole();
                    if (subject.empty() || message.empty()) {
                        cout << "Subject and Message are required. Please set them." << endl;
                        clearInputBuffer();
                        try {
                            constructEmail(subject, message, cin);
                        } catch (const exception& e) {
                            cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            attempts++; // Increment the attempts
                            if (attempts >= 3) {
                                cout << "Too many unsuccessful attempts. Press any key to return to main menu." << endl;
                                cin.ignore();  // If there's a chance you might have used cin before this point
                                clearInputBuffer();
                                cin.get();     // This will wait for a key press
                                break; // Break out of the loop after too many attempts
                            }
                            continue; // Loop back to prompt for email details again
                        }
                    }
                }

                if (!modified) {
                    // Return to the main menu if the email wasn't modified
                    continue;
                }
            } else if (choice == FINISH_AND_SEND_EMAILS_OPTION) {
                clearConsole();
                if (selectedVenuesForEmail.empty()) {
                    clearConsole();
                    cout << "No venues selected. Please add venues before sending emails." << endl;
                    cout << "Press any key to return to Main Menu..." << endl;
                    cin.ignore();  // If there's a chance you might have used cin before this point
                    clearInputBuffer();
                    cin.get();     // This will wait for a key press
                    continue; // Return to the main menu
                }

                while (true) {
                    int attempts = 0;
                    if (subject.empty() || message.empty()) {
                        cout << "Subject and Message are required. Please set them." << endl;
                        clearInputBuffer();
                        try {
                            constructEmail(subject, message, cin);
                        } catch (const exception& e) {
                            cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            attempts++; // Increment the attempts
                            if (attempts >= 3) { 
                                cout << "Too many unsuccessful attempts. Returning to main menu." << endl;
                                break; // Break out of the inner loop to go back to the main menu
                            }
                            continue; // Loop back to prompt for email details again
                        }
                    }
                    clearConsole();
                    cout << "----- EMAIL DETAILS -----\n";
                    cout << "Subject: " << subject << endl;
                    cout << "-------------------------\n";
                    cout << "Message: \n" << message << endl;
                    cout << "-------------------------\n";
                    cout << "Do you wish to modify this email? (Y/N): ";
                    char modifyEmailChoice;
                    cin >> modifyEmailChoice;
                    clearInputBuffer();

                    if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
                        subject.clear(); // Clear existing subject
                        message.clear(); // Clear existing message
                        clearConsole();
                        try {
                            constructEmail(subject, message, cin);
                        } catch (const exception& e) {
                            cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            continue; // Loop back to prompt for email details again
                        }
                        continue; // Loop back to show the modified email details
                    } else {
                        clearConsole();
                        cout << "Do you wish to send this email? (Y/N): ";
                        char confirmSend;
                        cin >> confirmSend;
                        clearInputBuffer();

                        if (confirmSend == 'Y' || confirmSend == 'y') {
                            // Proceed to send emails if confirmed
                            totalSelectedVenues = selectedVenuesForEmail.size();
                            emailSendProgress = 0; // Reset progress
                            for (const SelectedVenue& venue : selectedVenuesForEmail) {
                                sendIndividualEmail(curlWrapper.get(), venue, senderEmail, subject, message,
                                                    smtpServer, smtpPort, smtpUsername, mailPassDecrypted, progress);
                                ++emailSendProgress;

                                // Update the progress
                                curlWrapper.progressCallback(nullptr, emailSendProgress, totalSelectedVenues, 0, 0);

                                // Display email sending progress
                                viewEmailSendingProgress(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort, smtpUsername, mailPassDecrypted, progress);
                            }

                            filteredVenues.clear(); // Clear the filtered venues for the next round of emails
                            subject.clear(); // Reset subject
                            message.clear(); // Reset message
                            break; // Break out of the inner loop after sending

                        } else {
                            clearConsole();
                            cout << "Email saved for sending/editing." << endl;
                            cout << "Press any key to return to the Main Menu..." << endl;
                            cin.ignore();  // If there's a chance you might have used cin before this point
                            clearInputBuffer();
                            cin.get();     // This will wait for a key press
                            break; // Break out of the inner loop without sending
                        }
                    }
                }
            } else if (choice == EXIT_OPTION) {
                clearConsole();
                // Prompt for confirmation before exiting
                cout << "Are you sure you want to exit? (Y/N): ";
                char confirmExit;
                cin >> confirmExit;
                clearInputBuffer();

                if (confirmExit == 'Y' || confirmExit == 'y') {

                    // Exit VenueSender
                    cout << "Exiting the program. Press any key..." << endl;
                    cin.ignore();  // If there's a chance you might have used cin before this point
                    clearInputBuffer();
                    cin.get();     // This will wait for a key press
                    break;
                } else if (confirmExit == 'N' || confirmExit == 'n') {
                    cout << "Returning to the main menu." << endl;
                    // The user chose not to exit, return to the main menu
                } else {
                    cout << "Invalid choice. Press any key..." << endl;
                    cin.ignore();  // If there's a chance you might have used cin before this point
                    clearInputBuffer();
                    cin.get();     // This will wait for a key press
                    // The user entered an invalid choice, return to the main menu
                }
            } else {
                cout << "Invalid choice. Press any key..." << endl;
                cin.ignore();  // If there's a chance you might have used cin before this point
                clearInputBuffer();
                cin.get();     // This will wait for a key press
            }
        }
        // Before the program exits, call the resetConfigFile function
        // to reset the flags and passwords in the config.json file
        resetConfigFile();   

    return 0;
}
#endif // UNIT_TESTING
