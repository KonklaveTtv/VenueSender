#include "include/mail.h"

// Use the standard namespace
using namespace std;

// Global progress counters
static int successfulSends = 0; // Counter for successful email sends
static int successfulCustomSends = 0; // Counter for successful custom email sends
static int successfulTemplateSends = 0; // Counter for successful template sends

// Global definitions for email limits
int totalEmails;
int totalCustomEmails;
int totalTemplateEmails;

// Global definition to store emails sent to this session
unordered_set<string> EmailManager::sentEmailAddressesForEmails;
unordered_set<string> EmailManager::sentEmailAddressesForTemplates;

// Function to display current email settings
void EmailManager::viewEmailSettings(bool useSSL, const string& sslCertPath, bool verifyPeer, bool verifyHost, bool verbose,
                                     const string& senderEmail, const string& smtpUsername, int smtpPort, const string& smtpServer) {
    // Display the email settings in a structured format
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::EMAIL_SETTINGS_MENU_HEADER);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
    cout << "SMTP Username: " << smtpUsername << endl
         << "SMTP Server: " << smtpServer << endl
         << "SMTP Port: " << smtpPort << endl
         << "Sender Email: " << senderEmail << endl
         << "SSL: " << (useSSL ? "true" : "false") << endl;
    if (!sslCertPath.empty()) {
    cout << "SSL Cert Path: " << sslCertPath << endl;
    }
    cout << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl
         << "verifyHost: " << (verifyHost ? "true" : "false") << endl
         << "verbose: " << (verbose ? "true" : "false") << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif         
}

// Function to validate if the provided string is a valid email address format
bool EmailManager::isValidEmail(const string& email) {
    static const boost::regex emailPattern(R"((?=.{1,256})(?=.{1,64}@.{1,255})[^\s@]+@[^\s@]+\.[^\s@]+)");
    return boost::regex_match(email, emailPattern);
}

// Example function to validate a URL
bool EmailManager::isValidURL(const string& url) {
    // Simple regex pattern for URL validation, you can make it more comprehensive
    static const boost::regex urlPattern("https?://[\\w\\.-]+");
    return boost::regex_match(url, urlPattern);
}

// Function to guide the user in constructing an email with a subject, message, and optional attachment
void EmailManager::constructEmail(string& subject, string& message, string& attachmentName, string& attachmentSize, string& attachmentPath, istream &in) const {
    
    clearAllEmailData(subject, message, attachmentName, attachmentSize, attachmentPath);
    
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::CONSTRUCT_EMAIL_MENU_HEADER);

    // Prompt the user to enter the subject and perform checks
    do {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_SUBJECT_FOR_EMAIL_MESSAGE);
        string line;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        while (getline(in, line)) {
            if (line.empty()) break;
            subject += sanitizeSubject(line) + " ";
        }
        subject = ConsoleUtils::trim(subject);

    } while (subject.empty());

    const string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;

    if (subject.length() > maxSubjectLength) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_LENGTH_ERROR);
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
        ConsoleUtils::clearInputBuffer();
        cin.get();
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        subject.clear();
        return;
    }

    // Prompt the user to enter the message for the email and perform checks
    bool inputProvided;
    do {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_MESSAGE_FOR_EMAIL_MESSAGE);
        string line;
        inputProvided = false;

#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        while (getline(in, line)) {
            if (line.empty()) break;
            if (message.length() + line.length() > maxMessageLength) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_MESSAGE_LENGTH_ERROR);
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
                ConsoleUtils::clearInputBuffer();

                cin.get();
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif

                int charsToAdd = maxMessageLength - message.length();
                message += ConsoleUtils::trim(line).substr(0, charsToAdd);
                break;
            }
            message += ConsoleUtils::trim(line) + "\n";
        }

        if (in.eof()) {
            inputProvided = true;
        } else if (message.empty()) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_BLANK_ERROR);
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
            ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cin.get();
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
        }

        if (!inputProvided) {
            break;
        }

    } while (!inputProvided);

    // Prompt the user to add an attachment
        while (true) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_ADD_ATTACHMENT_MESSAGE);
            char addAttachmentChoice;
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cin >> addAttachmentChoice;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

        if (addAttachmentChoice == ConsoleUtils::NO_UPPER || addAttachmentChoice == ConsoleUtils::NO_LOWER) {
            break;

        } else if (addAttachmentChoice == ConsoleUtils::YES_UPPER || addAttachmentChoice == ConsoleUtils::YES_LOWER) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ATTACHMENT_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            getline(cin, attachmentPath);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

            attachmentPath.erase(remove(attachmentPath.begin(), attachmentPath.end(), '\''), attachmentPath.end());
            attachmentPath = ConsoleUtils::trim(attachmentPath);

            // Check if the user wants to skip adding an attachment
            if (attachmentPath.empty()) {
                break;  // Exit the loop if no attachment is provided
            }

            // Clean the path provided by the user and set the attachment name
            size_t lastSlash = attachmentPath.find_last_of("/\\\\");
            if (lastSlash == string::npos) {
                attachmentName = attachmentPath;
            } else {
                attachmentName = attachmentPath.substr(lastSlash + 1);
            }

            // Calculate and set the size of the attachment
            try {
                if (filesystem::exists(attachmentPath)) {
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                    size_t fileSize = boost::filesystem::file_size(attachmentPath);
                    string sizeUnit = BYTES_STRING;

                    // Convert to KB if size is at least 1 KB (1 KB = 1024 bytes)
                    if (fileSize >= KIBIBYTES) {
                        fileSize /= KIBIBYTES;
                        sizeUnit = KB_STRING;

                        // Convert to MB if size is at least 1 MB (1 MB = 1024 KB)
                        if (fileSize >= KIBIBYTES) {
                            fileSize /= KIBIBYTES;
                            sizeUnit = MB_STRING;
                        }
                    }

                    attachmentSize = to_string(fileSize) + sizeUnit;
                    cout << "File Size: " << fileSize << sizeUnit << endl;

#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
                    // Check the attachment size doesn't exceed 24MB
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        clearEmailAttachmentData(attachmentName, attachmentSize, attachmentPath);
                 
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_DIFFERENT_ATTACHMENT_MESSAGE);

                        char choice;
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                        cin >> choice;
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();
                        if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
                            continue; // Go back to asking for a new file
                        } else {
                            return; // Exit the loop and function
                        }
                    }
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif   
                    // Show the attachment name, size and path to the user
                    cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif  
                    break;  // Exit the loop if the file is valid
                } else {
                    // Error handling for attachment path
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                    return;  // Exit the function if the path is invalid
                }
            } catch (const filesystem::filesystem_error& e) {
                // Error handling for filesystem errors
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
                return;
                }

            } else {
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ATTACHMENT_INVALID_CHOICE_MESSAGE);
                continue;
            }
        }
        // Display a success message
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_CREATION_SUCCESS_MESSAGE);
}

void EmailManager::viewEditEmails(const string& senderEmail, string& subject, string& message, 
                                  string& attachmentName, string& attachmentSize, string& attachmentPath) const {
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::EMAIL_DETAILS_MENU_HEADER);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
    cout << "Subject: " << subject << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif 
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "\n" << message << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "---------------------------\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 

    int attempts = RESET_SEND_COUNT_TO_ZERO;
    bool modified = false;

    while (attempts < 3) {

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MODIFY_EMAIL_CONFIRMATION_MESSAGE);

        char modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cin >> modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();
        
        if (modifyEmailChoice == ConsoleUtils::YES_UPPER || modifyEmailChoice == ConsoleUtils::YES_LOWER) {
            try {
                constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                modified = true;
            } catch (const exception& e) {
                ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                clearSubjectMessageData(subject, message);
                attempts++; // Increment the attempts
                continue; // Loop back to prompt for email details again
            }

            if (subject.empty() || message.empty()) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_AND_SUBJECT_BLANK_ERROR);
                try {
                    constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                } catch (const exception& e) {
                    ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                    clearSubjectMessageData(subject, message);
                    attempts++; // Increment the attempts
                    if (attempts >= 3) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_AND_SUBJECT_WRITE_ATTEMPTS_ERROR);                                
                        return; // Break out of the loop after too many attempts
                    }
                    continue; // Loop back to prompt for email details again
                }
            }

            if (!modified) {
                // Return to the main menu if the email wasn't modified
                return;
            }

        } else {
            MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::EMAIL_SAVED_MENU_HEADER);
            return;
        }
    }
}

void EmailManager::viewEditTemplates(CURL* curl,
                                     vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                                     const string& smtpServer,
                                     int smtpPort,
                                     bool useSSL,
                                     bool verifyPeer,
                                     const string& senderEmail,
                                     map<string, pair<string, string>>& templateForEmail,
                                     string& genre, 
                                     string& performanceType, 
                                     string& performanceName,
                                     string& hometown, 
                                     string& similarArtists, 
                                     string& date,
                                     string& musicLink, 
                                     string& livePerfVideo, 
                                     string& musicVideo,
                                     vector<string>& pressQuotes, 
                                     vector<string>& quoteSources, 
                                     string& socials, 
                                     string& name,
                                     string& templateAttachmentName,
                                     string& templateAttachmentSize,
                                     string& templateAttachmentPath,
                                     bool& templateExists) const {
    char choice;
    bool modifyTemplate = true;

    if (templateForEmail.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_PENDING_ERROR);
        return;
    }

    auto firstElement = templateForEmail.begin();
    string firstEmail = firstElement->first;
    string firstSubject = firstElement->second.first;
    string firstMessage = firstElement->second.second;

    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::TEMPLATE_DETAILS_MENU_HEADER);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "For: " << firstEmail << "\n";
    cout << "Subject: " << firstSubject << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
    cout << "Attachment: " << (templateAttachmentName.empty() ? "None" : templateAttachmentName) << "\n";
    cout << "Size: " << (templateAttachmentSize.empty() ? "None" : templateAttachmentSize) << "\n";
    cout << "Path: " << (templateAttachmentPath.empty() ? "None" : templateAttachmentPath) << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "\n" << firstMessage << "\n";
    cout << "-----------------------------\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    while (modifyTemplate) {
        // Ask user if they want to modify or send
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MODIFY_TEMPLATE_CONFIRMATION_MESSAGE);
    #ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
    #endif
        cin >> choice;
    #ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
    #endif
        ConsoleUtils::clearInputBuffer();
        
        if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::REDO_OR_MODIFY_TEMPLATE_MESSAGE);
            string modifyChoice;
    #ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    #endif
            cin >> modifyChoice;
    #ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
    #endif
            ConsoleUtils::clearInputBuffer();  // Clear the input buffer

            // Convert to upper case for case-insensitive comparison
            transform(modifyChoice.begin(), modifyChoice.end(), modifyChoice.begin(), ::toupper);

            if (modifyChoice == "REDO" || modifyChoice == "redo" || modifyChoice == "R" || modifyChoice == "r") {
                // Clear the existing template and start over
                clearAllBookingTemplateData(templateForEmail, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
                EmailManager emailManager;
                emailManager.createBookingTemplate(curl, selectedVenuesForTemplates, senderEmail, templateForEmail, smtpServer, smtpPort, useSSL, verifyPeer, genre, performanceType, performanceName, hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, 
                                                    pressQuotes, quoteSources, socials, name, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
            } else if (modifyChoice == "MODIFY" || modifyChoice == "modify" || modifyChoice == "M" || modifyChoice == "m") {
                // Display available fields to the user
                cout << "Available fields to modify:" << endl;
                cout << "1. Genre" << endl;
                cout << "2. Performance Type" << endl;
                cout << "3. Performance Name" << endl;
                cout << "4. Hometown" << endl;
                cout << "5. Similar Artists" << endl;
                cout << "6. Date" << endl;
                cout << "7. Music Link" << endl;
                cout << "8. Live Performance Video" << endl;
                cout << "9. Music Video" << endl;
                cout << "10. Press Quotes and Sources" << endl;  // Combined option
                cout << "11. Social Links" << endl;
                cout << "12. Name" << endl;

                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CHOOSE_FIELDS_TO_MODIFY_MESSSAGE);
                string indices;

    #ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    #endif                   
                getline(cin, indices);
    #ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
    #endif
                ConsoleUtils::clearInputBuffer();

                // Convert the comma-separated string into a vector of integers
                stringstream ss(indices);
                vector<int> selectedIndices;
                int i;
                while (ss >> i) {
                    selectedIndices.push_back(i);
                    if (ss.peek() == ',') ss.ignore();
                }

                auto addPressQuotesAndSources = [](vector<string>& pressQuotes, vector<string>& quoteSources) {
                    char addMore = ConsoleUtils::YES_UPPER;
                    while (addMore == ConsoleUtils::YES_UPPER || addMore == ConsoleUtils::YES_LOWER) {
                        string singlePressQuote, singleQuoteSource;
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif                        
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_PRESS_QUOTE_MESSAGE);
                        getline(cin, singlePressQuote);
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_QUOTE_SOURCE_MESSAGE);
                        getline(cin, singleQuoteSource);
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();

                        pressQuotes.push_back(singlePressQuote);
                        quoteSources.push_back(singleQuoteSource);

                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ANOTHER_QUOTE_MESSAGE);
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif            
                        cin >> addMore;
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();
                    }
                };

                // Helper function to modify individual fields
                auto modifyField = [](string& field, const string& fieldName) {
                    string newField;
                    cout << "Current " << fieldName << ": " << field << endl;
                    cout << "Enter new " << fieldName << " (press Enter to keep the current value): ";
                    getline(cin, newField);
                    ConsoleUtils::clearInputBuffer();
                    if (!newField.empty()) {
                        field = newField;
                    }
                };

                // Modify individual fields based on user choice
                for (int index : selectedIndices) {
                    switch (index) {
                        case 1: modifyField(genre, "Genre"); break;
                        case 2: modifyField(performanceType, "Performance Type"); break;
                        case 3: modifyField(performanceName, "Performance Name"); break;
                        case 4: modifyField(hometown, "Hometown"); break;
                        case 5: modifyField(similarArtists, "Similar Artists"); break;
                        case 6: modifyField(date, "Date"); break;
                        case 7: modifyField(musicLink, "Music Link"); break;
                        case 8: modifyField(livePerfVideo, "Live Performance Video"); break;
                        case 9: modifyField(musicVideo, "Music Video"); break;
                        case 10: addPressQuotesAndSources(pressQuotes, quoteSources); break;
                        case 11: modifyField(socials, "Social Links"); break;
                        case 12: modifyField(name, "Name"); break;
                        default: cout << "Invalid index " << index << " skipped." << endl; break;
                    }
                }
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_REDO_MODIFY_INPUT_ERROR);
                continue;
            }
        
            // Update the templates right here after modification
            for (const SelectedVenueForTemplates& venueForTemplates : selectedVenuesForTemplates) {
                EmailManager emailManager;
                emailManager.constructBookingTemplateMessage(venueForTemplates, templateForEmail, genre, performanceType, performanceName,
                                                  hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, 
                                                    pressQuotes, quoteSources, socials, name);
            }

            if (!templateForEmail.empty()) {
                auto firstElement = templateForEmail.begin();
                string firstEmail = firstElement->first;
                string firstSubject = firstElement->second.first;
                string firstMessage = firstElement->second.second;

                // Display the completed template
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
                cout << "=========================================\n";
                cout << "Generated Email Template for: " << firstEmail << "\n";
                cout << "=========================================\n";
                cout << "Subject: " << firstSubject << "\n";
                cout << "=========================================\n";
                cout << firstMessage << endl;
                cout << "=========================================\n";
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                // Check if the user is satisfied
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_TEMPLATE_SATISFIED_MESSAGE);
                char satisfiedChoice;
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                cin >> satisfiedChoice;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                ConsoleUtils::clearInputBuffer();  // Clear the input buffer

                // Validate the third choice
                if (satisfiedChoice == ConsoleUtils::YES_UPPER || satisfiedChoice == ConsoleUtils::YES_LOWER) {
                    modifyTemplate = false;

                    // Ask if the user wants to send the email only when they're satisfied
                    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_TEMPLATE_MESSAGE);
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
                    cin >> choice;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    ConsoleUtils::clearInputBuffer();

                    // Validate the final choice
                    if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
                        templateExists = false; // Reset the flag since we're sending the email
                        // Now, send the email to all venues
                        bool sent = sendBookingTemplateEmails(curl, senderEmail, templateForEmail, smtpServer, smtpPort, useSSL, verifyPeer, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
                        if (!sent) {
                            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_SENDING_FAILED_ERROR);
                        }
                    } else if (choice == ConsoleUtils::NO_UPPER || choice == ConsoleUtils::NO_LOWER) {
                        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::TEMPLATE_SAVED_MENU_HEADER);
                        // If user chooses not to send, the template and subjects stay in the map
                        templateExists = true;
                    } else {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
                        // Here you can decide what to do in case of invalid input.
                        continue;
                    }
                } else if (satisfiedChoice == ConsoleUtils::NO_UPPER || satisfiedChoice == ConsoleUtils::NO_LOWER) {
                    // If the user is not satisfied, regenerate the template
                    for (const SelectedVenueForTemplates& venueForTemplates : selectedVenuesForTemplates) {
                        EmailManager emailManager;
                        emailManager.constructBookingTemplateMessage(venueForTemplates, templateForEmail, genre, performanceType, performanceName,
                                                      hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, 
                                                        pressQuotes, quoteSources, socials, name);
                    }
                } else {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
                    continue;
                }
            }
        } else if (choice == ConsoleUtils::NO_UPPER || choice == ConsoleUtils::NO_LOWER) {
            modifyTemplate = false;  // Exit the loop
            // Ask if the user wants to send the email
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_TEMPLATE_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cin >> choice;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();

            if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
                templateExists = false; // Reset the flag since we're sending the email
                // Now, send the email to all venues
                bool sent = sendBookingTemplateEmails(curl, senderEmail, templateForEmail, smtpServer, smtpPort, useSSL, verifyPeer, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
                if (!sent) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_SENDING_FAILED_ERROR);
                }
            } else if (choice == ConsoleUtils::NO_UPPER || choice == ConsoleUtils::NO_LOWER) {
                MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::TEMPLATE_SAVED_MENU_HEADER);
                // If user chooses not to send, the template and subjects stay in the map
                templateExists = true;
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
            }
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
            continue;
        }
    }
}

// Function to send an individual email to a selected venue with specified configurations
bool EmailManager::sendIndividualEmail(CURL* curl,
                                       const SelectedVenueForEmails& selectedVenueForEmails,
                                       const string& senderEmail,
                                       string& subject,
                                       string& message,
                                       const string& smtpServer,
                                       int smtpPort,
                                       bool useSSL,
                                       bool verifyPeer,
                                       string& attachmentName,
                                       string& attachmentSize,
                                       string& attachmentPath,
                                       vector<SelectedVenueForEmails>& selectedVenuesForEmails) {
    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value
    struct curl_slist* recipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime *mime = nullptr;


    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    if (useSSL && !verifyPeer) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SSL_CONFIGURATION_ERROR);
        return false;
    }

    // Update totalEmails dynamically
    totalEmails = selectedVenuesForEmails.size();

    if (!isValidEmail(senderEmail)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
        return false;
    }

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    // Inform user we are connecting to the SMTP server
    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    // Set Recipient(s)
    recipients = curl_slist_append(recipients, selectedVenueForEmails.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    // cURL headers
    string dateHeader = "Date: " + getCurrentDateRfc2822();
    string toHeader = "To: " + selectedVenueForEmails.email;
    string fromHeader = "From: " + senderEmail;
    string subjectHeader = "Subject: " + subject;

    headers = curl_slist_append(headers, dateHeader.c_str());
    headers = curl_slist_append(headers, toHeader.c_str());
    headers = curl_slist_append(headers, fromHeader.c_str());
    headers = curl_slist_append(headers, subjectHeader.c_str());

    // Set headers
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Initialize MIME
    mime = curl_mime_init(curl);
    if (!mime) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_INIT_ERROR);
        return false;
    }

    // Add the message part
    curl_mimepart* part = curl_mime_addpart(mime);
    if (!part) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_PART_ERROR);
        return false;
    }
    curl_mime_data(part, message.c_str(), CURL_ZERO_TERMINATED);

    if (!attachmentPath.empty()) {
        size_t fileSize = boost::filesystem::file_size(attachmentPath);
        attachmentSize = to_string(fileSize) + " bytes";

        part = curl_mime_addpart(mime);
        if (!part) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_PART_ERROR);
            return false;
        }

        // Add the attachment part
        curl_mime_filedata(part, attachmentPath.c_str());

        // Retrieve attachment filename
        curl_mime_filename(part, attachmentName.c_str());

        // Set MIME type for attachment
        curl_mime_type(part, "application/octet-stream");
    }

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_AUTH_MESSAGE);
    cout.flush();

    // Push the email
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // Update totalEmails dynamically
    totalEmails = selectedVenuesForEmails.size();

    res = curl_easy_perform(curl);
    double sendEmailProgressPercentage = 0.0;
    if (res == 0) { // Check if email was sent successfully
        successfulSends++;
        if (totalEmails != 0) {
            sendEmailProgressPercentage = (static_cast<double>(successfulSends) / totalEmails) * 100;
        }
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::GREEN_BORDER);
        cout << "      Progress: " << sendEmailProgressPercentage << "%" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        cout.flush();
    }

    EmailManager::sentEmailAddressesForEmails.insert(selectedVenueForEmails.email);  // Add to set

    // Free the MIME structure
    if (mime) {
        curl_mime_free(mime);
    }

    if (recipients) {
    curl_slist_free_all(recipients);
    }

    if (headers) {
    curl_slist_free_all(headers);
    }

    if (res == 0 && sendEmailProgressPercentage == 100) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAILS_SENT_MESSAGE);
        
        // Variable to capture user's decision
        string clearVenuesDecision;

        // Loop to ask for valid input
        while (true) {
            // Ask the user if they wish to clear the selected venues
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CLEAR_EMAILS_CONFIRMATION_MESSAGE);
    #ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    #endif
            cin >> clearVenuesDecision;
    #ifndef UNIT_TESTING
            ConsoleUtils::resetColor(); // Reset to default color
    #endif
            ConsoleUtils::clearInputBuffer();

            // Remove any quotes and trim spaces
            clearVenuesDecision.erase(remove(clearVenuesDecision.begin(), clearVenuesDecision.end(), '\''), clearVenuesDecision.end());
            clearVenuesDecision = ConsoleUtils::trim(clearVenuesDecision);

            // ANSI escape code check
            if (clearVenuesDecision.find("\033") != string::npos) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                continue;
            }

            // Whitespace check (full whitespace)
            if (all_of(clearVenuesDecision.begin(), clearVenuesDecision.end(), ::isspace)) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                continue;
            }

            // Check the user's input
            if (clearVenuesDecision == "Y" || clearVenuesDecision == "y") {
                // Clear all selectedVenuesForEmails and selectedVenuesForTemplates
                EmailManager::clearSelectedVenuesForEmails(selectedVenuesForEmails);
                break;  // Exit the loop
            } else if (clearVenuesDecision == "N" || clearVenuesDecision == "n") {
                // Do nothing and just continue
                break;  // Exit the loop
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                // Loop will continue to ask for a valid input
            }
        }
    } else {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_SEND_FAILURE_ERROR);
        return false;
    }
    
    if (!ErrorHandler::handleCurlError(res)) {
        if (res == CURLE_COULDNT_CONNECT) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_CONNECTION_ERROR);
        } else if (res == CURLE_LOGIN_DENIED) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_AUTH_ERROR);
        }
        return false;
    }

    return true;
}

bool EmailManager::sendBookingTemplateEmails(CURL* curl,
                                             const string& senderEmail,
                                             map<string, pair<string, string>>& templateForEmail,
                                             const string& smtpServer,
                                             int smtpPort,
                                             bool useSSL,
                                             bool verifyPeer,
                                             string& templateAttachmentName,
                                             string& templateAttachmentSize,
                                             string& templateAttachmentPath,
                                             bool templateExists) {
    double templateSendingProgressPercentage = 0.0;

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value
    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    if (!isValidEmail(senderEmail)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
        return false;
    }

    if (useSSL && !verifyPeer) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SSL_CONFIGURATION_ERROR);
        return false;
    }

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    // Loop through each venue and send the email
    for (const auto& kv : templateForEmail) {
        string recipientEmail = kv.first;
        string subject = kv.second.first;
        string message = kv.second.second;

        struct curl_slist* recipients = nullptr;
        struct curl_slist* headers = nullptr;
        curl_mime *mime = nullptr;
        
        // Set Recipient(s)
        recipients = curl_slist_append(recipients, recipientEmail.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);


        string dateHeader = "Date: " + getCurrentDateRfc2822();
        string toHeader = "To: " + recipientEmail;
        string fromHeader = "From: " + senderEmail;
        string subjectHeader = "Subject: " + subject;

        headers = curl_slist_append(headers, dateHeader.c_str());
        headers = curl_slist_append(headers, toHeader.c_str());
        headers = curl_slist_append(headers, fromHeader.c_str());
        headers = curl_slist_append(headers, subjectHeader.c_str());
        
        // Set headers
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Initialize MIME
        mime = curl_mime_init(curl);
        if (!mime) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_INIT_ERROR);
            return false;
        }

        // Add the message part
        curl_mimepart* part = curl_mime_addpart(mime);
        if (!part) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_PART_ERROR);
            return false;
        }
        curl_mime_data(part, message.c_str(), CURL_ZERO_TERMINATED);

        if (!templateAttachmentPath.empty()) {
            size_t fileSize = boost::filesystem::file_size(templateAttachmentPath);
            templateAttachmentSize = to_string(fileSize) + " bytes";

            part = curl_mime_addpart(mime);
            if (!part) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_PART_ERROR);
                return false;
            }

            // Add the attachment part
            curl_mime_filedata(part, templateAttachmentPath.c_str());

            // Retrieve attachment filename
            curl_mime_filename(part, templateAttachmentName.c_str());

            // Set MIME type for attachment
            curl_mime_type(part, "application/octet-stream");
        }
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_AUTH_MESSAGE);
        cout.flush();

        // Push the email
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Update totalEmails dynamically
        totalTemplateEmails = templateForEmail.size();

        res = curl_easy_perform(curl);
        
        if (res == 0) { // Check if email was sent successfully
            successfulTemplateSends++;
            if (totalTemplateEmails != 0) {
                templateSendingProgressPercentage = (static_cast<double>(successfulTemplateSends) / totalTemplateEmails) * 100;
            }
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif      
            MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::GREEN_BORDER);
            cout << "      Progress: " << templateSendingProgressPercentage << "%" << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            cout.flush();
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_SENDING_ERROR);
            continue;  // Skip to the next iteration
        }

        EmailManager::sentEmailAddressesForTemplates.insert(recipientEmail.c_str());  // Add to set

        // Free the MIME structure
        if (mime) {
            curl_mime_free(mime);
        }
        if (recipients) {
        curl_slist_free_all(recipients);

        }
        if (headers) {
        curl_slist_free_all(headers);
        }
    }

    if (res == 0 && templateSendingProgressPercentage == 100) {
        // If all emails sent we clear all the booking data here
        clearAllBookingTemplateData(templateForEmail, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
    } else {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_SEND_FAILURE_ERROR);
        return false;
    }
    
    if (!ErrorHandler::handleCurlError(res)) {
        if (res == CURLE_COULDNT_CONNECT) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_CONNECTION_ERROR);
        } else if (res == CURLE_LOGIN_DENIED) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_AUTH_ERROR);
        }
        return false;
    }

    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAILS_SENT_MESSAGE);
    return true;
}

// Helper function to conditionally append a line to the message
void EmailManager::appendIfNotEmpty(ostringstream& os, const string& label, const string& value) {
    if (!value.empty()) {
        os << label << ": " << value << "\n";
    }
}

void EmailManager::constructBookingTemplateMessage(const SelectedVenueForTemplates& venueForTemplates,
                                                   map<string, pair<string, string>>& templateForEmail,
                                                   string& genre, 
                                                   string& performanceType, 
                                                   string& performanceName,
                                                   string& hometown, 
                                                   string& similarArtists, 
                                                   string& date,
                                                   string& musicLink, 
                                                   string& livePerfVideo, 
                                                   string& musicVideo,
                                                   vector<string>& pressQuotes, 
                                                   vector<string>& quoteSources, 
                                                   string& socials, 
                                                   string& name) {
    ostringstream os;
    string subject = "Booking Inquiry for " + venueForTemplates.name;

    os << "Hi!\n\n"
       << "I am booking a tour for " << performanceName << " a " << genre << " " << performanceType << " from \n\n"
       << hometown << ". The music is similar to " << similarArtists << ".\n\n"
       << "We're planning to be in the " << venueForTemplates.city << " area on " << date << " and are\n\n"
       << "wondering if you might be interested in booking us at " << venueForTemplates.name << ".\n\n";

    // Add optional fields only if they are not empty
    appendIfNotEmpty(os, "- Music", musicLink);
    appendIfNotEmpty(os, "- Live Performance", livePerfVideo);
    appendIfNotEmpty(os, "- Music Video", musicVideo);

    // Loop through each quote/source pair and add it to the message
    if (!pressQuotes.empty() && pressQuotes.size() == quoteSources.size()) {
        os << "\nWhat people are saying about " << performanceName << "\n";
        for (size_t i = 0; i < pressQuotes.size(); ++i) {
            os << "\"" << pressQuotes[i] << "\" - " << quoteSources[i] << "\n";
        }
    }

    os << "\nPlease let me know if you have any questions or need additional information.\n\n"
       << "We appreciate your time and consideration!\n\n"
       << "Best wishes,\n"
       << name << "\n\n";

    appendIfNotEmpty(os, "- Social Links", socials);

    // Map each venue's email to its unique message and subject
    templateForEmail[venueForTemplates.email] = make_pair(subject, os.str());
}

void EmailManager::createBookingTemplate(CURL* curl,
                                         vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                                         const string& senderEmail,
                                         map<string, pair<string, string>>& templateForEmail,
                                         const string& smtpServer,
                                         int smtpPort,
                                         bool useSSL,
                                         bool verifyPeer,
                                         string& genre, 
                                         string& performanceType, 
                                         string& performanceName,
                                         string& hometown, 
                                         string& similarArtists, 
                                         string& date,
                                         string& musicLink, 
                                         string& livePerfVideo, 
                                         string& musicVideo,
                                         vector<string>& pressQuotes, 
                                         vector<string>& quoteSources, 
                                         string& socials, 
                                         string& name,
                                         string& templateAttachmentName,
                                         string& templateAttachmentSize,
                                         string& templateAttachmentPath,
                                         bool templateExists) const {
    char choice;
    bool modifyTemplate = true;
    
    auto getInputWithConfirmation = [&](string& input, const string& prompt, bool isMandatory = false, bool checkURL = false) {        
        bool inputConfirmed = false;
        while (!inputConfirmed) {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << prompt;

            getline(cin, input);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();

            if (input.empty()) {
                if (isMandatory) {
                    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MANDATORY_TEMPLATE_FIELD_MESSAGE);
                    continue; // Go back to the input prompt
                }

                char confirmation;
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMPTY_FIELD_CONFIRMATION_TEMPLATE_MESSAGE);
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
                cin >> confirmation;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                ConsoleUtils::clearInputBuffer();

                if (confirmation == ConsoleUtils::YES_UPPER || confirmation == ConsoleUtils::YES_LOWER) {
                    inputConfirmed = true;
                } else if (confirmation == ConsoleUtils::NO_UPPER || confirmation == ConsoleUtils::NO_LOWER) {
                    // Handle the 'No' case if needed
                } else {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
                    continue;
                }
            } else {
                if (checkURL && !isValidURL(input)) {
                    char confirmation;
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_POSSIBLE_ENTRY_ERROR);
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif 
                    cin >> confirmation;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    ConsoleUtils::clearInputBuffer();
                    
                    if (confirmation == ConsoleUtils::NO_UPPER || confirmation == ConsoleUtils::NO_LOWER) {
                        continue;
                    } else if (confirmation == ConsoleUtils::YES_UPPER || confirmation == ConsoleUtils::YES_LOWER) {
                        // Handle the 'Yes' case if needed
                    } else {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
                        continue;
                    }
                }
                inputConfirmed = true;
            }
        }
    };

        // Check if venues are selected
        if (selectedVenuesForTemplates.empty()) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::NO_VENUES_SELECTED_FOR_TEMPLATE_ERROR);
            return;  // Exit the function
        }

        // Prompt the user for the required data to fill the placeholders
        getInputWithConfirmation(genre, "Enter Genre (press Enter on a new line to finish): ");
        getInputWithConfirmation(performanceType, "What Are You? (Band, DJ, Singer) (press Enter on a new line to finish): ");
        getInputWithConfirmation(performanceName, "Enter Performance Name (press Enter on a new line to finish): ");
        getInputWithConfirmation(hometown, "Enter Hometown (press Enter on a new line to finish): ");
        getInputWithConfirmation(similarArtists, "Enter Similar Artists (press Enter on a new line to finish): ");
        getInputWithConfirmation(date, "Enter Date (press Enter on a new line to finish): ");
        
        // Check these for valid urls and give user a friendly warning
        getInputWithConfirmation(musicLink, "Enter Music Link (press Enter on a new line to finish): ", false, true);
        getInputWithConfirmation(livePerfVideo, "Enter Live Performance Video Link (press Enter on a new line to finish): ", false, true);
        getInputWithConfirmation(musicVideo, "Enter Music Video Link (press Enter on a new line to finish): ", false, true);
        
        // Add this to collect multiple quotes
        char addMoreQuotes = ConsoleUtils::YES_UPPER;
        while (addMoreQuotes == ConsoleUtils::YES_UPPER || addMoreQuotes == ConsoleUtils::YES_LOWER) {
            string singlePressQuote, singleQuoteSource;
            getInputWithConfirmation(singlePressQuote, "Enter Press Quote (press Enter on a new line to finish): ");
            getInputWithConfirmation(singleQuoteSource, "Enter Quote Source (press Enter on a new line to finish): ");
            pressQuotes.push_back(singlePressQuote);
            quoteSources.push_back(singleQuoteSource);

            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ANOTHER_QUOTE_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cin >> addMoreQuotes;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();
        }

        // Ask for social links
        getInputWithConfirmation(socials, "Enter Social Links (press Enter on a new line to finish): ", false, true);

        // Special case for Name due to it being mandatory
        bool nameConfirmed = false;
        while (!nameConfirmed) {
            getInputWithConfirmation(name, "Enter Your Name: ", true);
            if (name.empty()) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_NAME_FIELD_EMPTY_FIELD_ERROR);
            } else {
                nameConfirmed = true;
            }
        }

        for (const SelectedVenueForTemplates& venueForTemplates : selectedVenuesForTemplates) {
            EmailManager emailManager;
            emailManager.constructBookingTemplateMessage(venueForTemplates, templateForEmail, genre, performanceType, performanceName,
                                              hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, 
                                                pressQuotes, quoteSources, socials, name);
        }

        if (!templateForEmail.empty()) {
        auto firstElement = templateForEmail.begin();
        string firstEmail = firstElement->first;
        string firstSubject = firstElement->second.first;
        string firstMessage = firstElement->second.second;

        // Display the completed template
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
        cout << "=========================================\n";
        cout << "Generated Email Template for: " << firstEmail << "\n";
        cout << "=========================================\n";
        cout << "Subject: " << firstSubject << "\n";
        cout << "=========================================\n";
        cout << firstMessage << endl;
        cout << "=========================================\n";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        }

    while (true) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_ADD_ATTACHMENT_MESSAGE);
        char addAttachmentChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> addAttachmentChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

        if (addAttachmentChoice == ConsoleUtils::NO_UPPER || addAttachmentChoice == ConsoleUtils::NO_LOWER) {
            break;
        } else if (addAttachmentChoice == ConsoleUtils::YES_UPPER || addAttachmentChoice == ConsoleUtils::YES_LOWER) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ATTACHMENT_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            getline(cin, templateAttachmentPath);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();
            if (templateAttachmentPath.empty()) {
                break; // No attachment provided, move on
            }
            templateAttachmentPath.erase(remove(templateAttachmentPath.begin(), templateAttachmentPath.end(), '\''), templateAttachmentPath.end());
            templateAttachmentPath = ConsoleUtils::trim(templateAttachmentPath);

            size_t lastSlash = templateAttachmentPath.find_last_of("/\\\\");
            if (lastSlash == string::npos) {
                templateAttachmentName = templateAttachmentPath;
            } else {
                templateAttachmentName = templateAttachmentPath.substr(lastSlash + 1);
            }

            try {
                if (filesystem::exists(templateAttachmentPath)) {
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                    size_t fileSize = boost::filesystem::file_size(templateAttachmentPath);
                    string sizeUnit = BYTES_STRING;

                    // Convert to KB if size is at least 1 KB (1 KB = 1024 bytes)
                    if (fileSize >= KIBIBYTES) {
                        fileSize /= KIBIBYTES;
                        sizeUnit = KB_STRING;

                        // Convert to MB if size is at least 1 MB (1 MB = 1024 KB)
                        if (fileSize >= KIBIBYTES) {
                            fileSize /= KIBIBYTES;
                            sizeUnit = MB_STRING;
                        }
                    }

                    templateAttachmentSize = to_string(fileSize) + sizeUnit;
                    cout << "File Size: " << fileSize << sizeUnit << endl;

#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        clearTemplateAttachmentData(templateAttachmentName, templateAttachmentSize, templateAttachmentPath);
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_DIFFERENT_ATTACHMENT_MESSAGE);

                        char choice;
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                        cin >> choice;
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();
                        if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
                            continue; // Go back to asking for a new file
                        } else {
                            break; // Exit the loop without an attachment
                        }
                    }

#ifndef UNIT_TESTING                
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                    cout << "Attachment: " << templateAttachmentName << " (" << templateAttachmentSize << ")" << endl;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    break;  // Exit the loop if the file is valid
                } else {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                }
            } catch (const filesystem::filesystem_error& e) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
            } 
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_MENU_CHOICE_ERROR);
            continue;
        }
    }
while (modifyTemplate) {
        // Ask user if they want to modify or send
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MODIFY_TEMPLATE_CONFIRMATION_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

        if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::REDO_OR_MODIFY_TEMPLATE_MESSAGE);
            string modifyChoice;
            cin >> modifyChoice;
            ConsoleUtils::clearInputBuffer();  // Clear the input buffer
            
            // Convert to upper case for case-insensitive comparison
            transform(modifyChoice.begin(), modifyChoice.end(), modifyChoice.begin(), ::toupper);
            
            if (modifyChoice == "REDO" || modifyChoice == "redo" || modifyChoice == "R" || modifyChoice == "r") {
                // Clear the existing template and start over
                clearAllBookingTemplateData(templateForEmail, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
                EmailManager emailManager;
                emailManager.createBookingTemplate(curl, selectedVenuesForTemplates, senderEmail, templateForEmail, smtpServer, smtpPort, useSSL, verifyPeer, genre, performanceType, performanceName, hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, 
                                                                pressQuotes, quoteSources, socials, name, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
            } else if (modifyChoice == "MODIFY" || modifyChoice == "modify" || modifyChoice == "M" || modifyChoice == "m") {
                // Display available fields to the user
                cout << "Available fields to modify:" << endl;
                cout << "1. Genre" << endl;
                cout << "2. Performance Type" << endl;
                cout << "3. Performance Name" << endl;
                cout << "4. Hometown" << endl;
                cout << "5. Similar Artists" << endl;
                cout << "6. Date" << endl;
                cout << "7. Music Link" << endl;
                cout << "8. Live Performance Video" << endl;
                cout << "9. Music Video" << endl;
                cout << "10. Press Quotes and Sources" << endl;  // Combined option
                cout << "11. Social Links" << endl;
                cout << "12. Name" << endl;
 
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CHOOSE_FIELDS_TO_MODIFY_MESSSAGE);
                string indices;

#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif                   
                getline(cin, indices);
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                ConsoleUtils::clearInputBuffer();
                
                // Convert the comma-separated string into a vector of integers
                stringstream ss(indices);
                vector<int> selectedIndices;
                int i;
                while (ss >> i) {
                    selectedIndices.push_back(i);
                    if (ss.peek() == ',') ss.ignore();
                }
                
                // Helper function to modify vector<string> Press Quotes & Quote Sources
                auto addPressQuotesAndSources = [](vector<string>& pressQuotes, vector<string>& quoteSources) {
                    char addMore = ConsoleUtils::YES_UPPER;
                    while (addMore == ConsoleUtils::YES_UPPER || addMore == ConsoleUtils::YES_LOWER) {
                        string singlePressQuote, singleQuoteSource;
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif                        
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_PRESS_QUOTE_MESSAGE);
                        getline(cin, singlePressQuote);
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_QUOTE_SOURCE_MESSAGE);
                        getline(cin, singleQuoteSource);
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();

                        pressQuotes.push_back(singlePressQuote);
                        quoteSources.push_back(singleQuoteSource);

     
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ANOTHER_QUOTE_MESSAGE);
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif       
                        cin >> addMore;
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();
                    }
                };

                // Helper function to modify individual string fields
                auto modifyField = [](string& field, const string& fieldName) {
                    string newField;
                    cout << "Current " << fieldName << ": " << field << endl;
                    cout << "Enter new " << fieldName << " (press Enter to keep the current value): ";
                    getline(cin, newField);
                    ConsoleUtils::clearInputBuffer();
                    if (!newField.empty()) {
                        field = newField;
                    }
                };

                // Modify individual fields based on user choice
                for (int index : selectedIndices) {
                    switch (index) {
                        case 1: modifyField(genre, "Genre"); break;
                        case 2: modifyField(performanceType, "Performance Type"); break;
                        case 3: modifyField(performanceName, "Performance Name"); break;
                        case 4: modifyField(hometown, "Hometown"); break;
                        case 5: modifyField(similarArtists, "Similar Artists"); break;
                        case 6: modifyField(date, "Date"); break;
                        case 7: modifyField(musicLink, "Music Link"); break;
                        case 8: modifyField(livePerfVideo, "Live Performance Video"); break;
                        case 9: modifyField(musicVideo, "Music Video"); break;
                        case 10: addPressQuotesAndSources(pressQuotes, quoteSources); break;
                        case 11: modifyField(socials, "Social Links"); break;
                        case 12: modifyField(name, "Name"); break;
                        default: cout << "Invalid index " << index << " skipped." << endl; break;
                    }
                }
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_REDO_MODIFY_INPUT_ERROR);
                continue;
            }
        
            // Update the templates right here after modification
            for (const SelectedVenueForTemplates& venueForTemplates : selectedVenuesForTemplates) {
                EmailManager emailManager;
                emailManager.constructBookingTemplateMessage(venueForTemplates, templateForEmail, genre, performanceType, performanceName,
                                                  hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, 
                                                    pressQuotes, quoteSources, socials, name);
            }

            if (!templateForEmail.empty()) {
                auto firstElement = templateForEmail.begin();
                string firstEmail = firstElement->first;
                string firstSubject = firstElement->second.first;
                string firstMessage = firstElement->second.second;

                // Display the completed template
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
                cout << "=========================================\n";
                cout << "Generated Email Template for: " << firstEmail << "\n";
                cout << "=========================================\n";
                cout << "Subject: " << firstSubject << "\n";
                cout << "=========================================\n";
                cout << firstMessage << endl;
                cout << "=========================================\n";
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                // Check if the user is satisfied
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_TEMPLATE_SATISFIED_MESSAGE);
                char satisfiedChoice;
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                cin >> satisfiedChoice;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                ConsoleUtils::clearInputBuffer();  // Clear the input buffer

                // Validate the third choice
                if (satisfiedChoice == ConsoleUtils::YES_UPPER || satisfiedChoice == ConsoleUtils::YES_LOWER) {
                    modifyTemplate = false;

                    // Ask if the user wants to send the email only when they're satisfied
                    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_TEMPLATE_MESSAGE);
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
                    cin >> choice;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    ConsoleUtils::clearInputBuffer();

                    // Validate the final choice
                    if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
                        templateExists = false; // Reset the flag since we're sending the email
                        // Now, send the email to all venues
                        bool sent = sendBookingTemplateEmails(curl, senderEmail, templateForEmail, smtpServer, smtpPort, useSSL, verifyPeer, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
                        if (!sent) {
                            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_SENDING_FAILED_ERROR);
                        }
                    } else if (choice == ConsoleUtils::NO_UPPER || choice == ConsoleUtils::NO_LOWER) {
                        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::TEMPLATE_SAVED_MENU_HEADER);
                        // If user chooses not to send, the template and subjects stay in the map
                        templateExists = true;
                    } else {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
                        continue;
                    }
                } else if (satisfiedChoice == ConsoleUtils::NO_UPPER || satisfiedChoice == ConsoleUtils::NO_LOWER) {
                    continue;  // Continue modifying the template if the user is not satisfied
                } else {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
                    continue;
                }
            }            
        } else if (choice == ConsoleUtils::NO_UPPER || choice == ConsoleUtils::NO_LOWER) {
        modifyTemplate = false;  // Exit the loop
        // Ask if the user wants to send the email
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_TEMPLATE_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

        if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
            templateExists = false; // Reset the flag since we're sending the email
            // Now, send the email to all venues
            bool sent = sendBookingTemplateEmails(curl, senderEmail, templateForEmail, smtpServer, smtpPort, useSSL, verifyPeer, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);
            if (!sent) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_SENDING_FAILED_ERROR);
            }
        } else if (choice == ConsoleUtils::NO_UPPER || choice == ConsoleUtils::NO_LOWER) {
            MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::TEMPLATE_SAVED_MENU_HEADER);
            // If user chooses not to send, the template and subjects stay in the map
            templateExists = true;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
        }
    } else {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_CHAR_INPUT_ERROR);
        continue;
        }
    }
}

void EmailManager::emailCustomAddress(CURL* curl,
                                      const string& senderEmail,
                                      string& customAddressSubject,
                                      string& customAddressMessage,
                                      const string& smtpServer,
                                      int smtpPort,
                                      bool useSSL,
                                      bool verifyPeer,
                                      string& customAddressAttachmentName,
                                      string& customAddressAttachmentSize,
                                      string& customAddressAttachmentPath) const {

    const string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;
    double customEmailSendingProgressPercentage = 0.0;

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value
    struct curl_slist* recipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime *mime = nullptr;

    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return;
    }

    if (!isValidEmail(senderEmail)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
        return;
    }

    if (useSSL && !verifyPeer) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SSL_CONFIGURATION_ERROR);
        return;
    }

    // Reset the count
    successfulSends = RESET_SEND_COUNT_TO_ZERO;

    // Clear the subject, message, attachment fields
    clearAllCustomAddressEmailData(customAddressSubject, customAddressMessage, customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);

    // Declarations
    string customAddressRecipientEmail;
    string customAddressLine;
    
    while(true) {
    
        // Prompt the user for the custom email address
        do {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_CUSTOM_ADDRESS_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cin >> customAddressRecipientEmail;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();

            if (isValidEmail(customAddressRecipientEmail)) {
                break;
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::RECIPIENT_EMAIL_FORMAT_ERROR);
                return;
            }
        } while (true);

        do {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_SUBJECT_FOR_EMAIL_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            while (getline(cin, customAddressLine)) {
                if (customAddressLine.empty()) break;
                customAddressSubject += sanitizeSubject(customAddressLine) + " ";
            }
            customAddressSubject = ConsoleUtils::trim(customAddressSubject);
        } while (customAddressSubject.empty());

        if (customAddressSubject.length() > maxSubjectLength) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_LENGTH_ERROR);
        #ifndef UNIT_TESTING
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
            ConsoleUtils::clearInputBuffer();
            cin.get();
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
        #endif
            customAddressSubject.clear();
            return;
        }

        // Read multiple lines for the message body of the email
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_MESSAGE_FOR_EMAIL_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        while (getline(cin, customAddressLine) && !customAddressLine.empty()) {
            customAddressMessage += customAddressLine + "\n";
        }

        if (customAddressMessage.length() + customAddressLine.length() > maxMessageLength) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_MESSAGE_LENGTH_ERROR);
        #ifndef UNIT_TESTING
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
            ConsoleUtils::clearInputBuffer();
            cin.get();
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
        #endif
            int charsToAdd = maxMessageLength - customAddressMessage.length();
            customAddressMessage += ConsoleUtils::trim(customAddressLine).substr(0, charsToAdd);
            break;
        }

        while (true) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_ADD_ATTACHMENT_MESSAGE);

            char addAttachmentChoice;
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cin >> addAttachmentChoice;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

            if (addAttachmentChoice == ConsoleUtils::NO_UPPER || addAttachmentChoice == ConsoleUtils::NO_LOWER) {
                break;
        } else if (addAttachmentChoice == ConsoleUtils::YES_UPPER || addAttachmentChoice == ConsoleUtils::YES_LOWER) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ATTACHMENT_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            getline(cin, customAddressAttachmentPath);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            customAddressAttachmentPath.erase(remove(customAddressAttachmentPath.begin(), customAddressAttachmentPath.end(), '\''), customAddressAttachmentPath.end());
            customAddressAttachmentPath = ConsoleUtils::trim(customAddressAttachmentPath);

            if (customAddressAttachmentPath.empty()) {
                break;
            }
            size_t lastSlash = customAddressAttachmentPath.find_last_of("/\\\\");

            if (lastSlash == string::npos) {
                customAddressAttachmentName = customAddressAttachmentPath;
            } else {
                customAddressAttachmentName = customAddressAttachmentPath.substr(lastSlash + 1);
            }

            try {
                if (filesystem::exists(customAddressAttachmentPath)) {
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                    size_t fileSize = boost::filesystem::file_size(customAddressAttachmentPath);
                    string sizeUnit = BYTES_STRING;

                    // Convert to KB if size is at least 1 KB (1 KB = 1024 bytes)
                    if (fileSize >= KIBIBYTES) {
                        fileSize /= KIBIBYTES;
                        sizeUnit = KB_STRING;

                        // Convert to MB if size is at least 1 MB (1 MB = 1024 KB)
                        if (fileSize >= KIBIBYTES) {
                            fileSize /= KIBIBYTES;
                            sizeUnit = MB_STRING;
                        }
                    }

                    customAddressAttachmentSize = to_string(fileSize) + sizeUnit;
                    cout << "File Size: " << fileSize << sizeUnit << endl;

#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
                if (fileSize > MAX_ATTACHMENT_SIZE) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                    clearCustomAddressAttachmentData(customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);
                    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_DIFFERENT_ATTACHMENT_MESSAGE);
                    char choice;
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                    cin >> choice;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    ConsoleUtils::clearInputBuffer();
                    if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
                        continue; // Go back to asking for a new file
                    } else {
                        return; // Exit the loop and function
                    }
                }
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                cout << "Attachment: " << customAddressAttachmentName << " (" << customAddressAttachmentSize << ")" << endl;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                break;  // Exit the loop if the file is valid
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                return;
            }
        } catch (const filesystem::filesystem_error& e) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
            return;
        }

        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ATTACHMENT_INVALID_CHOICE_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            continue;
            }
        }

        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::CYAN_THIN_BORDER);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
        cout << "----- EMAIL DETAILS -----\n";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::CYAN_THIN_BORDER);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
        cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
        cout << "To: \"Recipient Email\" <" << customAddressRecipientEmail << ">\n";
        cout << "Subject: " << customAddressSubject << "\n";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
        ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
        cout << "Attachment: " << (customAddressAttachmentName.empty() ? "None" : customAddressAttachmentName) << "\n";
        cout << "Size: " << (customAddressAttachmentSize.empty() ? "None" : customAddressAttachmentSize) << "\n";
        cout << "Path: " << (customAddressAttachmentPath.empty() ? "None" : customAddressAttachmentPath) << "\n";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        cout << "\n" << customAddressMessage << "\n";
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::CYAN_THIN_BORDER);
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MODIFY_EMAIL_CONFIRMATION_MESSAGE);

        char modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cin >> modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

        if (modifyEmailChoice == ConsoleUtils::YES_UPPER || modifyEmailChoice == ConsoleUtils::YES_LOWER) {
            clearAllCustomAddressEmailData(customAddressSubject, customAddressMessage, customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);
            continue; // Loop back to the start of the function
        } else {
            break; // Exit the loop if the user is satisfied
        }
    }

    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_EMAIL_MESSAGE);
    char confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
    cin >> confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    ConsoleUtils::clearInputBuffer();

        // Proceed to send emails if confirmed
        if (confirmSend == ConsoleUtils::YES_UPPER || confirmSend == ConsoleUtils::YES_LOWER) {
            if (!curl) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
                return;
            }
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif       
            // Set Recipient     
            recipients = curl_slist_append(recipients, customAddressRecipientEmail.c_str());
            curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

            // cURL headers
            string dateHeader = "Date: " + getCurrentDateRfc2822();
            string toHeader = "To: " + customAddressRecipientEmail;
            string fromHeader = "From: " + senderEmail;
            string subjectHeader = "Subject: " + customAddressSubject;

            headers = curl_slist_append(headers, dateHeader.c_str());
            headers = curl_slist_append(headers, toHeader.c_str());
            headers = curl_slist_append(headers, fromHeader.c_str());
            headers = curl_slist_append(headers, subjectHeader.c_str());

            // Set headers
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Initialize MIME
            mime = curl_mime_init(curl);
            if (!mime) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_INIT_ERROR);
                return;
            }

            // Add the message part
            curl_mimepart* part = curl_mime_addpart(mime);
            if (!part) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_PART_ERROR);
                return;
            }
            curl_mime_data(part, customAddressMessage.c_str(), CURL_ZERO_TERMINATED);

            if (!customAddressAttachmentPath.empty()) {
                // Add the attachment part conditionally
                size_t fileSize = boost::filesystem::file_size(customAddressAttachmentPath);
                customAddressAttachmentSize = to_string(fileSize) + " bytes";
                part = curl_mime_addpart(mime);
                if (!part) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::MIME_PART_ERROR);
                    return;
                }

                // Add the attachment part
                curl_mime_filedata(part, customAddressAttachmentPath.c_str());
        
                // Retrieve attachment filename
                curl_mime_filename(part, customAddressAttachmentName.c_str());

                // Set MIME type for attachment
                curl_mime_type(part, "application/octet-stream");
            }

            curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SMTP_AUTH_MESSAGE);
            cout.flush();

            // Push the email
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            // Total Emails Is Always One
            totalCustomEmails = CUSTOM_EMAIL_TO_SEND_COUNT;

            // Perform the operation
            res = curl_easy_perform(curl);
            if (res == 0) { // Check if email was sent successfully
                successfulCustomSends = CUSTOM_EMAIL_TO_SEND_COUNT;
                if (totalCustomEmails != 0 || totalCustomEmails == 1) {
                    customEmailSendingProgressPercentage = (static_cast<double>(successfulCustomSends) / totalCustomEmails) * 100;
                }
        #ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
        #endif
                MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::GREEN_BORDER);
                cout << "      Progress: " << customEmailSendingProgressPercentage << "%" << endl;
        #ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
        #endif
                cout.flush();
            }

            // Free the MIME structure
            if (mime) {
                curl_mime_free(mime);
            }

            if (recipients) {
                curl_slist_free_all(recipients);
            }

            if (headers) {
                curl_slist_free_all(headers);
            }

            if (res == 0 && customEmailSendingProgressPercentage == 100) {
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_SENT_MESSAGE);
            } else {
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_SENDING_FAILED_MESSAGE);
            }

            // Clear the subject, message, and attachment strings
            clearAllCustomAddressEmailData(customAddressSubject, customAddressMessage, customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);
            return;

            if (!ErrorHandler::handleCurlError(res)) {
                if (res == CURLE_COULDNT_CONNECT) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_CONNECTION_ERROR);
                } else if (res == CURLE_LOGIN_DENIED) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_AUTH_ERROR);
                }
            }
        } else {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAILS_SAVED_MESSAGE);
            return;
    }
}

void EmailManager::confirmSendEmail(CURL* curl,
                                    vector<SelectedVenueForEmails>& selectedVenuesForEmail,
                                    const string& senderEmail,
                                    string& subject,
                                    string& message,
                                    const string& smtpServer,
                                    int smtpPort,
                                    bool useSSL,
                                    bool verifyPeer,
                                    string& attachmentName,
                                    string& attachmentSize,
                                    string& attachmentPath) {

    if (subject.empty() && message.empty()) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_NOT_CREATED_MESSAGE);
        return;
    }

    // Check if any venues are selected for email sending
    if (selectedVenuesForEmail.empty()) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::VENUES_NOT_SELECTED_MESSAGE);
        return;  // Exit the function
    }

    // Display a summary of all emails to be sent.
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::EMAIL_SUMMARY_MENU_HEADER);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE); // Setting color for attachment details
#endif
    cout << "Number of emails to send: " << selectedVenuesForEmail.size() << endl;
    cout << "Subject: " << subject << endl;
    cout << "Message: " << message << endl;
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::LIGHT_BLUE_BORDER);

    // Confirm with the user that they want to proceed.
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_EMAILS_MESSAGE);
    char confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
    cin >> confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    ConsoleUtils::clearInputBuffer();

    if (confirmSend != ConsoleUtils::YES_UPPER && confirmSend != ConsoleUtils::YES_LOWER) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_SENDING_ABORTED_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        return;
    }

    // Send each email, displaying progress as it goes.
    int sendCount = RESET_SEND_COUNT_TO_ZERO;
    for (const auto& venue : selectedVenuesForEmail) {
        if (sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, useSSL, verifyPeer, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail)) {
            sendCount++;
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Sent email " << sendCount << " of " << selectedVenuesForEmail.size() << " to " << venue.email << endl;

#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Failed to send email to " << venue.email << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
        }
    }

    // Display a summary once all emails are sent.
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::GREEN_BORDER);
    cout << "  Email Sending Completed  "<< endl;
    cout << " Sent: " << sendCount << "/"<< selectedVenuesForEmail.size() << endl;
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::GREEN_BORDER);
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
}

void EmailManager::confirmSendBookingTemplates(CURL* curl,
                                               vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                                               const string& senderEmail,
                                               map<string, pair<string, string>>& templateForEmail,
                                               const string& smtpServer,
                                               int smtpPort,
                                               bool useSSL,
                                               bool verifyPeer,
                                               string& templateAttachmentName,
                                               string& templateAttachmentSize,
                                               string& templateAttachmentPath,
                                               bool templateExists) {

    if (templateForEmail.empty()) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::NO_BOOKING_TEMPLATES_CREATED_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        return;
    }

    // Check if any venues are selected for sending templates
    if (selectedVenuesForTemplates.empty()) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::VENUES_NOT_SELECTED_FOR_TEMPLATES_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        return;  // Exit the function
    }

    // Display a summary of all templates to be sent.
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::BOOKING_TEMPLATE_SUMMARY_MENU_HEADER);

    // Display the first template as an example
    auto firstElement = templateForEmail.begin();
    string firstEmail = firstElement->first;
    string firstSubject = firstElement->second.first;
    string firstMessage = firstElement->second.second;

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "Example Template for: " << firstEmail << "\n";
    cout << "Subject: " << firstSubject << "\n";
    cout << "Message: " << firstMessage << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE); // Setting color for attachment details
#endif
    cout << "Number of templates to send: " << selectedVenuesForTemplates.size() << endl;
    // More details about the templates here...
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::CYAN_BORDER);

    // Confirm with the user that they want to proceed.
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_TEMPLATES_MESSAGE);
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    char confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cin >> confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    ConsoleUtils::clearInputBuffer();

    if (confirmSend != ConsoleUtils::YES_UPPER && confirmSend != ConsoleUtils::YES_LOWER) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::TEMPLATE_SENDING_ABORTED_MESSAGE);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        return;
    }

    // Send each template, displaying progress as it goes.
    bool sendStatus = sendBookingTemplateEmails(curl, senderEmail, templateForEmail, smtpServer, smtpPort, useSSL, verifyPeer, templateAttachmentName, templateAttachmentSize, templateAttachmentPath, templateExists);

    // Set the initial color to Orange
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::ORANGE_BORDER);
    // Set the color based on sendStatus (Green if completed, Red if failed)
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(sendStatus ? ConsoleUtils::Color::GREEN : ConsoleUtils::Color::RED);
#endif
    cout << "Template sending " << (sendStatus ? "completed." : "failed.") << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::ORANGE_BORDER);
}

void EmailManager::addAttachmentToTemplate(string& templateAttachmentName, 
                                           string& templateAttachmentSize, 
                                           string& templateAttachmentPath) const {
    while (true) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_ADD_ATTACHMENT_MESSAGE);
        char addAttachmentChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> addAttachmentChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

        if (addAttachmentChoice == ConsoleUtils::NO_UPPER || addAttachmentChoice == ConsoleUtils::NO_LOWER) {
            break;
        } else if (addAttachmentChoice == ConsoleUtils::YES_UPPER || addAttachmentChoice == ConsoleUtils::YES_LOWER) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ATTACHMENT_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            getline(cin, templateAttachmentPath);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();
            if (templateAttachmentPath.empty()) {
                break; // No attachment provided, move on
            }
            templateAttachmentPath.erase(remove(templateAttachmentPath.begin(), templateAttachmentPath.end(), '\''), templateAttachmentPath.end());
            templateAttachmentPath = ConsoleUtils::trim(templateAttachmentPath);

            size_t lastSlash = templateAttachmentPath.find_last_of("/\\\\");
            if (lastSlash == string::npos) {
                templateAttachmentName = templateAttachmentPath;
            } else {
                templateAttachmentName = templateAttachmentPath.substr(lastSlash + 1);
            }

            try {
                if (filesystem::exists(templateAttachmentPath)) {
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                    size_t fileSize = boost::filesystem::file_size(templateAttachmentPath);
                    string sizeUnit = BYTES_STRING;

                    // Convert to KB if size is at least 1 KB (1 KB = 1024 bytes)
                    if (fileSize >= KIBIBYTES) {
                        fileSize /= KIBIBYTES;
                        sizeUnit = KB_STRING;

                        // Convert to MB if size is at least 1 MB (1 MB = 1024 KB)
                        if (fileSize >= KIBIBYTES) {
                            fileSize /= KIBIBYTES;
                            sizeUnit = MB_STRING;
                        }
                    }

                    templateAttachmentSize = to_string(fileSize) + sizeUnit;
                    cout << "File Size: " << fileSize << sizeUnit << endl;

#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        clearTemplateAttachmentData(templateAttachmentName, templateAttachmentSize, templateAttachmentPath);
                        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_DIFFERENT_ATTACHMENT_MESSAGE);

                        char choice;
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                        cin >> choice;
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif
                        ConsoleUtils::clearInputBuffer();
                        if (choice == ConsoleUtils::YES_UPPER || choice == ConsoleUtils::YES_LOWER) {
                            continue; // Go back to asking for a new file
                        } else {
                            break; // Exit the loop without an attachment
                        }
                    }

#ifndef UNIT_TESTING                
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                    cout << "Attachment: " << templateAttachmentName << " (" << templateAttachmentSize << ")" << endl;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    break;  // Exit the loop if the file is valid
                } else {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                }
            } catch (const filesystem::filesystem_error& e) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
            } 
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_MENU_CHOICE_ERROR);
            continue;
        }
    }
}
