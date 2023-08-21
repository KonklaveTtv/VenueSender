#include "main.h"

using namespace confPaths;
using namespace std;

ConfigManager configManager;
CsvReader csvReader;
CurlHandleWrapper curlWrapper;
EmailManager emailManager;
EncryptionManager encryptionManager;
MenuManager menuManager;
VenueFilter venueFilter;
VenueUtilities venueUtilities;

// Exclude the following code block if UNIT_TESTING is defined
#ifndef UNIT_TESTING

int main() {
    // Initialize data and configuration settings
    vector<Venue> venues;
    string configVenuesCsvPath, smtpServer, smtpUsername, mailPass, mailPassDecrypted, senderEmail, subject, message, attachmentName, attachmentPath, attachmentSize;
    int smtpPort;
    bool useSSL, verifyPeer, verifyHost, verbose;

    // Load the config settings from the JSON file
    std::string venuesPathCopy = confPaths::venuesCsvPath;
    if (!configManager.loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesPathCopy)) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        exit(1); // Handle the error appropriately
    }

    // Attempt to decrypt the stored password
    mailPassDecrypted = encryptionManager.decryptPassword(mailPass);
    if (mailPassDecrypted.empty()) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::DECRYPTION_ERROR);
        exit(1);  // Exit with an error status
    }

    // Set up and initialize CURL
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

    // Main loop for interacting with the user
    while (true) {
        // Display menu options and get user's choice
        
        int choice = menuManager.displayMenuOptions();
            // Handle menu choices
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

                // Display filtered venues
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
                ErrorHandler errorHandler;
                errorHandler.showInfoAndReturn();              
            } else if (choice == MenuManager::SHOW_EMAIL_SETTINGS_OPTION) {
                // View Email Settings
                
                emailManager.viewEmailSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpPort, smtpServer);
            } else if (choice == MenuManager::VIEW_EDIT_EMAILS_OPTION) {
                
                int attempts = 0;
                bool modified = false;

                while (attempts < 3) {
                    cout << "-------------------------\n";
                    cout << "----- EMAIL DETAILS -----\n";
                    cout << "-------------------------\n";
                    cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
                    cout << "Subject: " << subject << "\n";
                    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
                    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
                    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
                    cout << "\n" << message << "\n";
                    cout << "-------------------------\n";

                    cout << "Do you wish to modify this email? (Y/N): ";
                    char modifyEmailChoice;
                    cin >> modifyEmailChoice;
                    ConsoleUtils::clearInputBuffer();
                    
                    if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
                        subject.clear(); // Clear existing subject
                        message.clear(); // Clear existing message
                                                
                        try {
                            emailManager.constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                            modified = true;
                        } catch (const exception& e) {
                            ErrorHandler errorHandler;
                            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            attempts++; // Increment the attempts
                            continue; // Loop back to prompt for email details again
                        }
                    } else {
                        
                        cout << "Email saved for sending/editing." << endl;
                        ErrorHandler errorHandler;
                        errorHandler.showInfoAndReturn();
                        break;
                    }

                    
                    if (subject.empty() || message.empty()) {
                        cout << "Subject and Message are required. Please set them." << endl;
                        ConsoleUtils::clearInputBuffer();
                        try {
                            emailManager.constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                        } catch (const exception& e) {
                            ErrorHandler errorHandler;
                            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            attempts++; // Increment the attempts
                            if (attempts >= 3) {
                                cout << "Too many unsuccessful attempts." << endl;
                                ErrorHandler errorHandler;
                                errorHandler.showInfoAndReturn();
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
                
                if (selectedVenuesForEmail.empty()) {
                    
                    cout << "No venues selected. Please add venues before sending emails." << endl;
                    ErrorHandler errorHandler;
                    errorHandler.showInfoAndReturn();
                    continue; // Return to the main menu
                }

                while (true) {
                    int attempts = 0;
                    if (subject.empty() || message.empty()) {
                        cout << "Subject and Message are required. Please set them." << endl;
                        ConsoleUtils::clearInputBuffer();
                        try {
                            emailManager.constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                        } catch (const exception& e) {
                            ErrorHandler errorHandler;
                            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
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
                    
                    cout << "-------------------------\n";
                    cout << "----- EMAIL DETAILS -----\n";
                    cout << "-------------------------\n";
                    cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
                    cout << "Subject: " << subject << "\n";
                    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
                    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
                    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
                    cout << "\n" << message << "\n";
                    cout << "-------------------------\n";

                    cout << "Do you wish to modify this email? (Y/N): ";
                    char modifyEmailChoice;
                    cin >> modifyEmailChoice;
                    ConsoleUtils::clearInputBuffer();

                    if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
                        subject.clear(); // Clear existing subject
                        message.clear(); // Clear existing message
                        
                        try {
                            emailManager.constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                        } catch (const exception& e) {
                            ErrorHandler errorHandler;
                            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                            subject.clear(); // Clear existing subject
                            message.clear(); // Clear existing message
                            continue; // Loop back to prompt for email details again
                        }
                        continue; // Loop back to show the modified email details
                    } else {
                        
                        cout << "Do you wish to send this email? (Y/N): ";
                        char confirmSend;
                        cin >> confirmSend;
                        ConsoleUtils::clearInputBuffer();

                        if (confirmSend == 'Y' || confirmSend == 'y') {
                            // Proceed to send emails if confirmed

                            for (size_t i = 0; i < selectedVenuesForEmail.size(); ++i) {
                                const SelectedVenue& venue = selectedVenuesForEmail[i];
                                curlWrapper.setEmailBeingSent(venue.email); // Set the value of emailBeingSent
                                cout << "Sending email " << (i + 1) << " of " << selectedVenuesForEmail.size() << " to: " << curlWrapper.getEmailBeingSent() << endl;

                                // Send the individual email with progress tracking
                                emailManager.sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);

                                curlWrapper.clearEmailBeingSent();
                            }

                            // Display email sending progress
                            emailManager.viewEmailSendingProgress(senderEmail);

                            filteredVenues.clear(); // Clear the filtered venues for the next round of emails
                            selectedVenuesForEmail.clear();  // Clear the selected venues after sending the emails
                            subject.clear(); // Reset subject
                            message.clear(); // Reset message
                            break; // Break out of the inner loop after sending
                        } else {
                            
                            cout << "Email saved for sending/editing." << endl;
                            ErrorHandler errorHandler;
                            errorHandler.showInfoAndReturn();
                            break; // Break out of the inner loop without sending
                        }
                    }
                }
            } else if (choice == MenuManager::EXIT_OPTION) {
                
                // Prompt for confirmation before exiting
                cout << "Are you sure you want to exit? (Y/N): ";
                char confirmExit;
                cin >> confirmExit;
                ConsoleUtils::clearInputBuffer();

                if (confirmExit == 'Y' || confirmExit == 'y') {

                    // Exit VenueSender
                    cout << "Exiting the program." << endl;
                    this_thread::sleep_for(chrono::milliseconds(1500));
                    break;
                } else if (confirmExit == 'N' || confirmExit == 'n') {
                    cout << "Returning to the main menu." << endl;
                    this_thread::sleep_for(chrono::milliseconds(1500));
                    // The user chose not to exit, return to the main menu
                } else {
                    cout << "Invalid choice. Press return..." << endl;
                    ConsoleUtils::clearInputBuffer();
                    cin.get();     // This will wait for a key press
                    // The user entered an invalid choice, return to the main menu
                }
            } else {
                cout << "Invalid choice. Press return..." << endl;
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
