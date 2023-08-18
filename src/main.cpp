#include "main.h"

using namespace confPaths;
using namespace std;

// Exclude the following code block if UNIT_TESTING is defined
#ifndef UNIT_TESTING

int main() {
    // Initialize data and configuration settings
    vector<Venue> venues;
    string configVenuesCsvPath, smtpServer, smtpUsername, mailPass, mailPassDecrypted, senderEmail, subject, message;
    int smtpPort;
    bool useSSL, verifyPeer, verifyHost, verbose;

    ConfigManager configManager;
    CsvReader csvReader;
    EncryptionManager encryptionManager;
    EmailManager emailManager;
    MenuManager menuManager;
    VenueFilter venueFilter;
    VenueUtilities venueUtilities;

    // Load the config settings from the JSON file
    std::string venuesPathCopy = confPaths::venuesCsvPath;
    if (!configManager.loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesPathCopy)) {
        cerr << "Failed to load configuration settings from config.json." << endl;
        exit(1); // Handle the error appropriately
    }

    // Attempt to decrypt the stored password
    mailPassDecrypted = encryptionManager.decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        cerr << "Failed to decrypt passwords. Ensure they are correctly encrypted in config.json." << endl;
        exit(1);  // Exit with an error status
    }

    // Set up and initialize CURL
    CurlHandleWrapper curlWrapper;
    CurlHandleWrapper::init();
    CURL* curl = setupCurlHandle(curlWrapper, useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPassDecrypted, smtpPort, smtpServer);
    if (!curl) {
        return 1;  // Return error if CURL setup failed
    }

    // Read venues data from CSV file
    csvReader.readCSV(venues, confPaths::venuesCsvPath);

    // Extract unique genres, states, cities, and capacities from the venues data
    set<string> uniqueGenres = venueUtilities.getUniqueGenres(venues);
    set<string> uniqueStates = venueUtilities.getUniqueStates(venues);
    set<string> uniqueCities = venueUtilities.getUniqueCities(venues);
    set<int> uniqueCapacities = venueUtilities.getUniqueCapacities(venues);

    FilterCriteria criteria;
    vector<SelectedVenue> selectedVenuesForEmail;
    vector<SelectedVenue> filteredVenues;

    int totalSelectedVenues = selectedVenuesForEmail.size();
    int emailSendProgress = 0;

    // Main loop for interacting with the user
    while (true) {
        // Display menu options and get user's choice
        ConsoleUtils::clearConsole();
        int choice = menuManager.displayMenuOptions();
            // Handle menu choices
            if (choice >= static_cast<int>(MenuManager::MenuOption::FilterByGenre) &&
                choice <= MenuManager::FILTER_BY_CAPACITY_OPTION) {
                // Declare a temporary vector to store filtered venues
                vector<SelectedVenue> temporaryFilteredVenues;

                // Handle filtering options
                if (choice == MenuManager::FILTER_BY_GENRE_OPTION) {
                    // Filter by Genre
                    ConsoleUtils::clearConsole();
                    temporaryFilteredVenues = venueFilter.filterByOption(venues, "Genre", uniqueGenres, temporaryFilteredVenues);
                } else if (choice == MenuManager::FILTER_BY_STATE_OPTION) {
                    // Filter by State
                    ConsoleUtils::clearConsole();
                    temporaryFilteredVenues = venueFilter.filterByOption(venues, "State", uniqueStates, temporaryFilteredVenues);
                } else if (choice == MenuManager::FILTER_BY_CITY_OPTION) {
                    // Filter by City
                    ConsoleUtils::clearConsole();
                    temporaryFilteredVenues = venueFilter.filterByOption(venues, "City", uniqueCities, temporaryFilteredVenues);
                } else if (choice == MenuManager::FILTER_BY_CAPACITY_OPTION) {
                    // Filter by Capacity
                    ConsoleUtils::clearConsole();
                    temporaryFilteredVenues = venueFilter.filterByCapacity(venues, uniqueCapacities, temporaryFilteredVenues);
                }

                // Display filtered venues
                venueFilter.displayFilteredVenues(temporaryFilteredVenues);

                // Call the new function to process venue selection
                venueFilter.processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail);
            } else if (choice == MenuManager::VIEW_SELECTED_VENUES_OPTION) {
                // View Selected Venues
                ConsoleUtils::clearConsole();
                menuManager.displaySelectedVenues(selectedVenuesForEmail);
            } else if (choice == MenuManager::CLEAR_SELECTED_VENUES_OPTION) {
                // Clear Selected Venues
                ConsoleUtils::clearConsole();
                selectedVenuesForEmail.clear();
                cout << "Selected venues cleared." << endl;
                cout << "Press return to go back to the Main Menu..." << endl;
                cin.ignore();  // If there's a chance you might have used cin before this point
                ConsoleUtils::clearInputBuffer();
                cin.get();     // This will wait for a key press                
            } else if (choice == MenuManager::SHOW_EMAIL_SETTINGS_OPTION) {
                // View Email Settings
                ConsoleUtils::clearConsole();
                emailManager.viewEmailSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);
            } else if (choice == MenuManager::VIEW_EDIT_EMAILS_OPTION) {
                ConsoleUtils::clearConsole();
                int attempts = 0;
                bool modified = false;

                while (attempts < 3) {
                    cout << "-------------------------\n";
                    cout << "----- EMAIL DETAILS -----\n";
                    cout << "-------------------------\n";
                    cout << "From: <" << senderEmail << ">\n";
                    cout << "Subject: " << subject << "\n";
                    cout << "\n" << message << "\n";
                    cout << "-------------------------\n";

                    cout << "Do you wish to modify this email? (Y/N): ";
                    char modifyEmailChoice;
                    cin >> modifyEmailChoice;
                    ConsoleUtils::clearInputBuffer();
                    ConsoleUtils::clearConsole();
                    if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
                        subject.clear(); // Clear existing subject
                        message.clear(); // Clear existing message
                        ConsoleUtils::clearConsole();                        
                        try {
                            emailManager.constructEmail(subject, message, cin);
                            modified = true;
                        } catch (const exception& e) {
                            cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            attempts++; // Increment the attempts
                            continue; // Loop back to prompt for email details again
                        }
                    } else {
                        ConsoleUtils::clearConsole();
                        cout << "Email saved for sending/editing." << endl;
                        cout << "Press return to go back to the Main Menu..." << endl;
                        cin.ignore();  // If there's a chance you might have used cin before this point
                        ConsoleUtils::clearInputBuffer();
                        cin.get();
                        break;
                    }

                    ConsoleUtils::clearConsole();
                    if (subject.empty() || message.empty()) {
                        cout << "Subject and Message are required. Please set them." << endl;
                        ConsoleUtils::clearInputBuffer();
                        try {
                            emailManager.constructEmail(subject, message, cin);
                        } catch (const exception& e) {
                            cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            attempts++; // Increment the attempts
                            if (attempts >= 3) {
                                cout << "Too many unsuccessful attempts. Press return to go back to main menu." << endl;
                                cin.ignore();  // If there's a chance you might have used cin before this point
                                ConsoleUtils::clearInputBuffer();
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
            } else if (choice == MenuManager::FINISH_AND_SEND_EMAILS_OPTION) {
                ConsoleUtils::clearConsole();
                if (selectedVenuesForEmail.empty()) {
                    ConsoleUtils::clearConsole();
                    cout << "No venues selected. Please add venues before sending emails." << endl;
                    cout << "Press return to go back to Main Menu..." << endl;
                    cin.ignore();  // If there's a chance you might have used cin before this point
                    ConsoleUtils::clearInputBuffer();
                    cin.get();     // This will wait for a key press
                    continue; // Return to the main menu
                }

                while (true) {
                    int attempts = 0;
                    if (subject.empty() || message.empty()) {
                        cout << "Subject and Message are required. Please set them." << endl;
                        ConsoleUtils::clearInputBuffer();
                        try {
                            emailManager.constructEmail(subject, message, cin);
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
                    ConsoleUtils::clearConsole();
                    cout << "-------------------------\n";
                    cout << "----- EMAIL DETAILS -----\n";
                    cout << "-------------------------\n";
                    cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
                    cout << "Subject: " << subject << "\n";
                    cout << "\n" << message << "\n";
                    cout << "-------------------------\n";

                    cout << "Do you wish to modify this email? (Y/N): ";
                    char modifyEmailChoice;
                    cin >> modifyEmailChoice;
                    ConsoleUtils::clearInputBuffer();

                    if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
                        subject.clear(); // Clear existing subject
                        message.clear(); // Clear existing message
                        ConsoleUtils::clearConsole();
                        try {
                            emailManager.constructEmail(subject, message, cin);
                        } catch (const exception& e) {
                            cerr << "An error occurred while entering subject and message: " << e.what() << endl;
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            continue; // Loop back to prompt for email details again
                        }
                        continue; // Loop back to show the modified email details
                    } else {
                        ConsoleUtils::clearConsole();
                        cout << "Do you wish to send this email? (Y/N): ";
                        char confirmSend;
                        cin >> confirmSend;
                        ConsoleUtils::clearInputBuffer();

                        if (confirmSend == 'Y' || confirmSend == 'y') {
                            // Proceed to send emails if confirmed
                            totalSelectedVenues = selectedVenuesForEmail.size();
                            emailSendProgress = 0; // Reset progress
                            for (const SelectedVenue& venue : selectedVenuesForEmail) {
                                emailManager.sendIndividualEmail(curlWrapper.get(), venue, senderEmail, subject, message,
                                                    smtpServer, smtpPort);
                                ++emailSendProgress;

                                // Update the progress
                                curlWrapper.progressCallback(nullptr, emailSendProgress, totalSelectedVenues, 0, 0);
                            }
                            // Display email sending progress
                            emailManager.viewEmailSendingProgress(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort);

                            filteredVenues.clear(); // Clear the filtered venues for the next round of emails
                            selectedVenuesForEmail.clear();  // Clear the selected venues after sending the emails
                            subject.clear(); // Reset subject
                            message.clear(); // Reset message
                            break; // Break out of the inner loop after sending
                        } else {
                            ConsoleUtils::clearConsole();
                            cout << "Email saved for sending/editing." << endl;
                            cout << "Press return to go back to the Main Menu..." << endl;
                            cin.ignore();  // If there's a chance you might have used cin before this point
                            ConsoleUtils::clearInputBuffer();
                            cin.get();     // This will wait for a key press
                            break; // Break out of the inner loop without sending
                        }
                    }
                }
            } else if (choice == MenuManager::EXIT_OPTION) {
                ConsoleUtils::clearConsole();
                // Prompt for confirmation before exiting
                cout << "Are you sure you want to exit? (Y/N): ";
                char confirmExit;
                cin >> confirmExit;
                ConsoleUtils::clearInputBuffer();

                if (confirmExit == 'Y' || confirmExit == 'y') {

                    // Exit VenueSender
                    cout << "Exiting the program. Press return..." << endl;
                    cin.ignore();  // If there's a chance you might have used cin before this point
                    ConsoleUtils::clearInputBuffer();
                    cin.get();     // This will wait for a key press
                    break;
                } else if (confirmExit == 'N' || confirmExit == 'n') {
                    cout << "Returning to the main menu." << endl;
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    // The user chose not to exit, return to the main menu
                } else {
                    cout << "Invalid choice. Press return..." << endl;
                    cin.ignore();  // If there's a chance you might have used cin before this point
                    ConsoleUtils::clearInputBuffer();
                    cin.get();     // This will wait for a key press
                    // The user entered an invalid choice, return to the main menu
                }
            } else {
                cout << "Invalid choice. Press return..." << endl;
                cin.ignore();  // If there's a chance you might have used cin before this point
                ConsoleUtils::clearInputBuffer();
                cin.get();     // This will wait for a key press
            }
        }
        // Before the program exits, call the resetConfigFile function
        // to reset the flags and passwords in the config.json file
        configManager.resetConfigFile();  


        // cURL library cleanup
        CurlHandleWrapper::cleanup(); // Assuming you have a cleanup method in your CurlHandleWrapper class that calls curl_global_cleanup();

    return 0;
}
#endif // UNIT_TESTING
