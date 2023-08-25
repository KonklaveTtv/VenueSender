#include "include/mail.h"

// Use the standard namespace and alias for filesystem
using namespace std;

// Define global objects for CURL and error handling
CurlHandleWrapper& curlHandleWrapper = CurlHandleWrapper::getInstance();
ErrorHandler errorHandler;

// Global progress counters
static int successfulSends = 0; // Counter for successful email sends
int totalEmails;

// Function to display current email settings
void EmailManager::viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost, bool verbose,
                                     const string& senderEmail, int smtpPort, const string& smtpServer) {
    // Display the email settings in a structured format
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "============================" << endl
         << "       Email Settings       " << endl
         << "============================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    cout << "SMTP Server: " << smtpServer << endl
         << "SMTP Port: " << smtpPort << endl
         << "Sender Email: " << senderEmail << endl
         << "SSL: " << (useSSL ? "true" : "false") << endl
         << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl
         << "verifyHost: " << (verifyHost ? "true" : "false") << endl
         << "verbose: " << (verbose ? "true" : "false") << endl;
}

// Function to validate if the provided string is a valid email address format
bool EmailManager::isValidEmail(const string& email) {
    static const regex emailPattern(R"((?=.{1,256})(?=.{1,64}@.{1,255})[^\s@]+@[^\s@]+\.[^\s@]+)");
    return regex_match(email, emailPattern);
}

// Function to guide the user in constructing an email with a subject, message, and optional attachment
void EmailManager::constructEmail(string &subject, string &message, string &attachmentName, string &attachmentSize, string &attachmentPath, istream &in) {
    
    clearAllEmailData(subject, message, attachmentName, attachmentSize, attachmentPath);

    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "===========================" << endl;
    cout << "===== Construct Email =====" << endl;
    cout << "===========================" << endl;
    ConsoleUtils::resetColor();
    // Prompt the user to enter the subject and perform checks
    do {
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Enter subject for the email (press Enter on a new line to finish): ";
        ConsoleUtils::resetColor();
        string line;
        while (getline(in, line)) {
            if (line.empty()) break;
            subject += sanitizeSubject(line) + " ";
        }
        subject = ConsoleUtils::trim(subject);

    } while (subject.empty());

    const string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;

    if (subject.length() > maxSubjectLength) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_LENGTH_ERROR);
#ifndef UNIT_TESTING
        cout << "Press return to go back..." << endl;
        cin.ignore();
        ConsoleUtils::clearInputBuffer();
        cin.get();
#endif
        subject.clear();
        return;
    }

    // Prompt the user to enter the message for the email and perform checks
    bool inputProvided;
    do {
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Enter the message for the email (press Enter on a new line to finish):\n";
        ConsoleUtils::resetColor();
        string line;
        inputProvided = false;

        while (getline(in, line)) {
            if (line.empty()) break;
            if (message.length() + line.length() > maxMessageLength) {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_MESSAGE_LENGTH_ERROR);
#ifndef UNIT_TESTING
                cout << "Press return to go back..." << endl;
                cin.ignore();
                ConsoleUtils::clearInputBuffer();
                cin.get();
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
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_BLANK_ERROR);
#ifndef UNIT_TESTING
            cout << "Press return to go back..." << endl;
            cin.ignore();
            ConsoleUtils::clearInputBuffer();
            cin.get();
#endif
        }

        if (!inputProvided) {
            break;
        }

    } while (!inputProvided);

    // Prompt the user to add an attachment
    while (true) {
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Enter the path of the file to attach (or press Enter to skip): ";
        ConsoleUtils::resetColor();
        getline(cin, attachmentPath);
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
                size_t fileSize = filesystem::file_size(attachmentPath);
                attachmentSize = to_string(fileSize) + " bytes";
                ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA); // Setting color for attachment details
                cout << "File Size: " << fileSize << " bytes" << endl;

                // Check the attachment size doesn't exceed 24MB
                if (fileSize > MAX_ATTACHMENT_SIZE) {
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                    clearAttachmentData(attachmentName, attachmentSize, attachmentPath);
                    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
                    cout << "Do you want to add a different attachment? (Y/N): ";
                    ConsoleUtils::resetColor();
                    char choice;
                    cin >> choice;
                    ConsoleUtils::clearInputBuffer();
                    if (choice == 'Y' || choice == 'y') {
                        continue; // Go back to asking for a new file
                    } else {
                        return; // Exit the loop and function
                    }
                }

                // Show the attachment name, size and path to the user
                cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
                ConsoleUtils::resetColor(); // Resetting color after displaying attachment details
                break;  // Exit the loop if the file is valid
            } else {
                // Error handling for attachment path
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                return;  // Exit the function if the path is invalid
            }
        } catch (const filesystem::filesystem_error& e) {
            // Error handling for filesystem errors
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
            return;  // Exit the function if a filesystem error occurs
        }
    }
    // Display a success message
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
    cout << "Email has been successfully created." << endl;
    ConsoleUtils::resetColor();
}

void EmailManager::viewEditEmails(CURL* curl, const string& smtpServer, int smtpPort, vector<SelectedVenue>& selectedVenuesForEmail, const string& senderEmail, 
                                  string &subject, string &message, string& attachmentName, string& attachmentSize, string& attachmentPath, bool& templateExists,
                                  map<string, pair<string, string>>& emailToTemplate) {


    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "===========================\n";
    cout << "       EMAIL DETAILS       \n";
    cout << "===========================\n";
    cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
    cout << "Subject: " << subject << "\n";
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "\n" << message << "\n";
    cout << "-------------------------\n";
    ConsoleUtils::resetColor();

    int attempts = 0;
    bool modified = false;

    while (attempts < 3) {

        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);        
        cout << "Do you wish to modify this email? (Y/N): ";
        ConsoleUtils::resetColor();

        char modifyEmailChoice;
        cin >> modifyEmailChoice;
        ConsoleUtils::clearInputBuffer();
        
        if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
            if (templateExists) {
                message.clear();
                createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, 
                                      attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
            } else {
                clearSubjectMessageData(subject, message);                                    
                try {
                    constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                    modified = true;
                } catch (const exception& e) {
                    ErrorHandler errorHandler;
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                    clearSubjectMessageData(subject, message);
                    attempts++; // Increment the attempts
                    continue; // Loop back to prompt for email details again
                }
            }

            if (subject.empty() || message.empty()) {
                ErrorHandler errorHandler;
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_AND_SUBJECT_BLANK_ERROR);
                try {
                    constructEmail(subject, message, attachmentName, attachmentSize, attachmentPath, cin);
                } catch (const exception& e) {
                    ErrorHandler errorHandler;
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                    clearSubjectMessageData(subject, message);
                    attempts++; // Increment the attempts
                    if (attempts >= 3) {
                        ErrorHandler errorHandler;
                        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_AND_SUBJECT_WRITE_ATTEMPTS_ERROR);                                
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
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);              
            cout << "Email saved for sending/editing." << endl;
            ConsoleUtils::resetColor();
            return;
        }
    }
}

void EmailManager::viewEditTemplates(CURL* curl,
                                     const string& smtpServer,
                                     int smtpPort,
                                     vector<SelectedVenue>& selectedVenuesForEmail,
                                     const string& senderEmail,
                                     map<string, pair<string, string>>& emailToTemplate,
                                     string& attachmentName,
                                     string& attachmentSize,
                                     string& attachmentPath,
                                     bool& templateExists) {

    if (emailToTemplate.empty()) {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cerr << "No templates have been created. Please create a template first." << endl;
        ConsoleUtils::resetColor();
        return;
    }

    auto firstElement = emailToTemplate.begin();
    string firstEmail = firstElement->first;
    string firstSubject = firstElement->second.first;
    string firstMessage = firstElement->second.second;

    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "=============================\n";
    cout << "      TEMPLATE DETAILS       \n";
    cout << "=============================\n";
    cout << "For: " << firstEmail << "\n";
    cout << "Subject: " << firstSubject << "\n";
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "\n" << firstMessage << "\n";
    cout << "-----------------------------\n";
    ConsoleUtils::resetColor();

    int attempts = 0;

    while (attempts < 3) {

        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Do you wish to modify this template? (Y/N): ";
        ConsoleUtils::resetColor();

        char modifyTemplateChoice;
        cin >> modifyTemplateChoice;
        ConsoleUtils::clearInputBuffer();

        if (modifyTemplateChoice == 'Y' || modifyTemplateChoice == 'y') {
            clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
            createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort,
                                  attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
        } else {
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
            cout << "Template saved for sending/editing." << endl;
            ConsoleUtils::resetColor();
            return;
        }
    }
}

// Function to send an individual email to a selected venue with specified configurations
bool EmailManager::sendIndividualEmail(CURL* curl,
                                       const SelectedVenue& selectedVenue,
                                       const string& senderEmail,
                                       string& subject,
                                       string& message,
                                       const string& smtpServer,
                                       int smtpPort,
                                       string& attachmentName,
                                       string& attachmentSize,
                                       string& attachmentPath,
                                       vector<SelectedVenue>& selectedVenuesForEmail) {

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value

    // Update totalEmails dynamically
    totalEmails = selectedVenuesForEmail.size();

    if (!curl) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    if (!isValidEmail(senderEmail)) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
        cerr << "Please set it correctly in your custom.json file." << endl;
        return false;
    }
    
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
    ConsoleUtils::resetColor();

    struct curl_slist* recipients = nullptr;

    if (selectedVenuesForEmail.size() <= 49) {
        // Reset the count
        successfulSends = 0;

        // Use "To:" for each email if 49 or less venues are selected
        for (const auto& venue : selectedVenuesForEmail) {
            recipients = curl_slist_append(recipients, venue.email.c_str());
        }
    } else {
        // Use "BCC:" for batch sending
        int batchSize = 49;
        for (size_t i = 0; i < selectedVenuesForEmail.size(); i += batchSize) {
            recipients = nullptr;  // Reset the list for each batch
            size_t end = min(i + batchSize, selectedVenuesForEmail.size());
            for (size_t j = i; j < end; ++j) {
                recipients = curl_slist_append(recipients, ("BCC: " + selectedVenuesForEmail[j].email).c_str());
            }
            
            // Send the batch of emails
            curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
            
            string toHeader = "To: " + senderEmail;  // Use senderEmail as the main recipient for BCC batches
            string fromHeader = "From: " + senderEmail;
            string subjectHeader = "Subject: " + subject;

            struct curl_slist* headers = nullptr;
            string dateHeader = "Date: " + getCurrentDateRfc2822();
            headers = curl_slist_append(headers, dateHeader.c_str());
            headers = curl_slist_append(headers, toHeader.c_str());
            headers = curl_slist_append(headers, fromHeader.c_str());
            headers = curl_slist_append(headers, subjectHeader.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_mime *mime = nullptr;

            if (!attachmentPath.empty()) {
                mime = curl_mime_init(curl);

                size_t fileSize = filesystem::file_size(attachmentPath);
                attachmentSize = to_string(fileSize) + " bytes";

                // Add the message part
                curl_mimepart *part = curl_mime_addpart(mime);
                curl_mime_data(part, message.c_str(), CURL_ZERO_TERMINATED);

                // Add the attachment part
                part = curl_mime_addpart(mime);
                curl_mime_filedata(part, attachmentPath.c_str());

                // Retrieve attachment filename
                curl_mime_filename(part, attachmentName.c_str());

                // Set MIME type for attachment
                curl_mime_type(part, "application/octet-stream");

                curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
            } else {
                string payload = dateHeader + "\r\n"
                                 + toHeader + "\r\n"
                                 + fromHeader + "\r\n"
                                 + subjectHeader + "\r\n\r\n"
                                 + message;
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHandleWrapper::readCallback);
                curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
                curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(payload.length()));
            }

            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Authenticating with SMTP server..." << endl;
            ConsoleUtils::resetColor();
            cout.flush();
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            res = curl_easy_perform(curl);
            if (res == 0) { // Check if email was sent successfully
                successfulSends++;
                double progressPercentage = 0.0;
                if (totalEmails != 0) {
                    progressPercentage = (static_cast<double>(successfulSends) / totalEmails) * 100;
                }
                ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
                cout << "Progress: " << progressPercentage << "%" << endl;
                ConsoleUtils::resetColor();
                cout.flush();
            }

            // Free the MIME structure
            if (mime) {
                curl_mime_free(mime);
            }

            curl_slist_free_all(recipients);  // Free the list after each batch
            curl_slist_free_all(headers);
            
            if (!errorHandler.handleCurlError(res)) {
                return false;
            }
        }
        return true;  // Return early after sending all batches
    }

    // Logic for sending individual emails with "To:"
    recipients = curl_slist_append(recipients, selectedVenue.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    string toHeader = "To: " + selectedVenue.email;
    string fromHeader = "From: " + senderEmail;
    string subjectHeader = "Subject: " + subject;

    struct curl_slist* headers = nullptr;
    string dateHeader = "Date: " + getCurrentDateRfc2822();
    headers = curl_slist_append(headers, dateHeader.c_str());
    headers = curl_slist_append(headers, toHeader.c_str());
    headers = curl_slist_append(headers, fromHeader.c_str());
    headers = curl_slist_append(headers, subjectHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_mime *mime = nullptr;

    if (!attachmentPath.empty()) {
        mime = curl_mime_init(curl);

        size_t fileSize = filesystem::file_size(attachmentPath);
        attachmentSize = to_string(fileSize) + " bytes";

        // Add the message part
        curl_mimepart *part = curl_mime_addpart(mime);
        curl_mime_data(part, message.c_str(), CURL_ZERO_TERMINATED);

        // Add the attachment part
        part = curl_mime_addpart(mime);
        curl_mime_filedata(part, attachmentPath.c_str());

        // Retrieve attachment filename
        curl_mime_filename(part, attachmentName.c_str());

        // Set MIME type for attachment
        curl_mime_type(part, "application/octet-stream");

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    } else {
        string payload = dateHeader + "\r\n"
                         + toHeader + "\r\n"
                         + fromHeader + "\r\n"
                         + subjectHeader + "\r\n\r\n"
                         + message;
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHandleWrapper::readCallback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(payload.length()));
    }

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Authenticating with SMTP server..." << endl;
    ConsoleUtils::resetColor();
    cout.flush();
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // Update totalEmails dynamically
    totalEmails = selectedVenuesForEmail.size();

    res = curl_easy_perform(curl);
    if (res == 0) { // Check if email was sent successfully
        successfulSends++;
        double progressPercentage = 0.0;
        if (totalEmails != 0) {
            progressPercentage = (static_cast<double>(successfulSends) / totalEmails) * 100;
        }
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
        cout << "Progress: " << progressPercentage << "%" << endl;
        ConsoleUtils::resetColor();
        cout.flush();
    }

    // Free the MIME structure
    if (mime) {
        curl_mime_free(mime);
    }

    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);

    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
    cout << "Email sending progress completed." << endl;
    ConsoleUtils::resetColor();

    if (!errorHandler.handleCurlError(res)) {
        if (res == CURLE_COULDNT_CONNECT) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_CONNECTION_ERROR);
        } else if (res == CURLE_LOGIN_DENIED) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_AUTH_ERROR);
        }
        return false;
    }

    return true;
}

bool EmailManager::sendBookingTemplateEmails(CURL* curl,
                                             const string& senderEmail,
                                             map<string, pair<string, string>>& emailToTemplate,
                                             const string& smtpServer,
                                             int smtpPort,
                                             string& attachmentName,
                                             string& attachmentSize,
                                             string& attachmentPath) {

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value

    if (!curl) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    if (!isValidEmail(senderEmail)) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
        cerr << "Please set it correctly in your custom.json file." << endl;
        return false;
    }

    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
    ConsoleUtils::resetColor();

    // Loop through each venue and send the email
    for (const auto& kv : emailToTemplate) {
        string recipientEmail = kv.first;
        string subject = kv.second.first;
        string message = kv.second.second;

        struct curl_slist* recipients = nullptr;
        recipients = curl_slist_append(recipients, recipientEmail.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        string toHeader = "To: " + recipientEmail;
        string fromHeader = "From: " + senderEmail;
        string subjectHeader = "Subject: " + subject;

        struct curl_slist* headers = nullptr;
        string dateHeader = "Date: " + getCurrentDateRfc2822();
        headers = curl_slist_append(headers, dateHeader.c_str());
        headers = curl_slist_append(headers, toHeader.c_str());
        headers = curl_slist_append(headers, fromHeader.c_str());
        headers = curl_slist_append(headers, subjectHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Setting up MIME and other options
        curl_mime *mime = nullptr;
        if (!attachmentPath.empty()) {
            mime = curl_mime_init(curl);

            size_t fileSize = filesystem::file_size(attachmentPath);
            attachmentSize = to_string(fileSize) + " bytes";

            // Add the message part
            curl_mimepart *part = curl_mime_addpart(mime);
            curl_mime_data(part, message.c_str(), CURL_ZERO_TERMINATED);

            // Add the attachment part
            part = curl_mime_addpart(mime);
            curl_mime_filedata(part, attachmentPath.c_str());

            // Retrieve attachment filename
            curl_mime_filename(part, attachmentName.c_str());

            // Set MIME type for attachment
            curl_mime_type(part, "application/octet-stream");

            curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        } else {
            string payload = dateHeader + "\r\n"
                             + toHeader + "\r\n"
                             + fromHeader + "\r\n"
                             + subjectHeader + "\r\n\r\n"
                             + message;
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHandleWrapper::readCallback);
            curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(payload.length()));
        }

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            // Handle errors
            errorHandler.handleCurlError(res);
            cerr << "Failed to send email to " << recipientEmail << endl;
            continue;  // Skip to the next iteration
        }

        // Free the MIME structure
        if (mime) {
            curl_mime_free(mime);
        }

        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);
    }

    return true;
}

void EmailManager::createBookingTemplate(CURL* curl,
                                       const string& senderEmail,
                                       map<string, pair<string, string>>& emailToTemplate,
                                       const string& smtpServer,
                                       int smtpPort,
                                       string& attachmentName,
                                       string& attachmentSize,
                                       string& attachmentPath,
                                       vector<SelectedVenue>& selectedVenuesForEmail,
                                       bool templateExists) {
    // Check if venues are selected
    if (selectedVenuesForEmail.empty()) {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cerr << "Error: No venues have been selected. Please select venues first before attempting to send the template." << endl;
        ConsoleUtils::resetColor();
        return;  // Exit the function
    }

    ConsoleUtils::clearInputBuffer();

    // Prompt the user for the required data to fill the placeholders
    string genre, bandName, hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, pressQuote, name, socials;

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Genre: ";
    ConsoleUtils::resetColor();
    getline(cin, genre);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Band Name: ";
    ConsoleUtils::resetColor();
    getline(cin, bandName);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Hometown: ";
    ConsoleUtils::resetColor();
    getline(cin, hometown);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Similar Artists: ";
    ConsoleUtils::resetColor();
    getline(cin, similarArtists);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Date: ";
    ConsoleUtils::resetColor();
    getline(cin, date);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Music Link: ";
    ConsoleUtils::resetColor();
    getline(cin, musicLink);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Live Performance Video Link: ";
    ConsoleUtils::resetColor();
    getline(cin, livePerfVideo);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Music Video Link: ";
    ConsoleUtils::resetColor();
    getline(cin, musicVideo);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Press Quote: ";
    ConsoleUtils::resetColor();
    getline(cin, pressQuote);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Your Name: ";
    ConsoleUtils::resetColor();
    getline(cin, name);

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter Social Links: ";
    ConsoleUtils::resetColor();
    getline(cin, socials);

    // Construct the email template for each venue without sending it
    for (const SelectedVenue& venue : selectedVenuesForEmail) {
        
        string subject = "--- Booking Inquiry for " + venue.name + " ---";
        string templateMessage = 
        string("Hi!,\n\n") +
        "I am booking a tour for my " + genre + " band, " + bandName + ", from \n\n" 
        + hometown + ". " + "The music is in a similar vein as " + similarArtists + ".\n\n"
        "We're planning to be in the " + venue.city + " area on " + date + " and are\n\n" 
        " wondering if you might be interested in booking us at " + venue.name + ".\n\n" +
        "Here are some resources to familiarize you with our work:\n\n" +
        "- Music: " + musicLink + "\n" +
        "- Live Performance: " + livePerfVideo + "\n" +
        "- Music Video: " + musicVideo + "\n\n" +
        "What people are saying about us:\n" +
        "\""+ pressQuote + "\"\n\n" +
        "Please let me know if you have any questions or need additional information.\n\n"
        "We appreciate your time and consideration!\n\n" +
        "Best wishes,\n" +
        name + "\n\n" +
        "-- Social Links --\n" + socials + "\n";
        // Map each venue's email to its unique message and subject
        emailToTemplate[venue.email] = make_pair(subject, templateMessage);
    }

    if (!emailToTemplate.empty()) {
    auto firstElement = emailToTemplate.begin();
    string firstEmail = firstElement->first;
    string firstSubject = firstElement->second.first;
    string firstMessage = firstElement->second.second;

    // Display the completed template
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "=========================================\n";
    cout << "Generated Email Template for: " << firstEmail << "\n";
    cout << "=========================================\n";
    cout << "Subject: " << firstSubject << "\n";
    cout << "=========================================\n";
    cout << firstMessage << endl;
    cout << "=========================================\n";
    ConsoleUtils::resetColor();
    }

    // Prompt the user to add an attachment
    while (true) {
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Enter the path of the file to attach (or press Enter to skip): ";
        ConsoleUtils::resetColor();
        getline(cin, attachmentPath);
        if (attachmentPath.empty()) {
            break; // No attachment provided, move on
        }
        attachmentPath.erase(remove(attachmentPath.begin(), attachmentPath.end(), '\''), attachmentPath.end());
        attachmentPath = ConsoleUtils::trim(attachmentPath);

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
                size_t fileSize = filesystem::file_size(attachmentPath);
                attachmentSize = to_string(fileSize) + " bytes";
                ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
                cout << "File Size: " << fileSize << " bytes" << endl;
                ConsoleUtils::resetColor();
                // Check the attachment size doesn't exceed 24MB
                if (fileSize > MAX_ATTACHMENT_SIZE) {
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                    clearAttachmentData(attachmentName, attachmentSize, attachmentPath);
                    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
                    cout << "Do you want to add a different attachment? (Y/N): ";
                    ConsoleUtils::resetColor();
                    char choice;
                    cin >> choice;
                    ConsoleUtils::clearInputBuffer();
                    if (choice == 'Y' || choice == 'y') {
                        continue; // Go back to asking for a new file
                    } else {
                        break; // Exit the loop without an attachment
                    }
                }

                // Show the attachment name, size and path to the user
                ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
                cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
                ConsoleUtils::resetColor(); // Resetting color after displaying attachment details
                break;  // Exit the loop if the file is valid
            } else {
                // Error handling for attachment path
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
            }
        } catch (const filesystem::filesystem_error& e) {
            // Error handling for filesystem errors
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
        }
    }

    // Ask user if they want to modify or send
    char choice;
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Do you wish to modify this template? (Y/N): ";
    cin >> choice;
    ConsoleUtils::resetColor();

    if (choice == 'Y' || choice == 'y') {
        // Clear the existing template and start over
        clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
        createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
    } else {
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Do you want to send the template? (Y/N): ";
        ConsoleUtils::resetColor();
        cin >> choice;
        if (choice == 'Y' || choice == 'y') {
            templateExists = false; // Reset the flag since we're sending the email

            // Now, send the email to all venues
            bool sent = sendBookingTemplateEmails(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
            if (!sent) {
                cerr << "Failed to send email templates." << endl;
            }
            // No need to set templateExists here again, as it's already set to false
        } else {
            // If user chooses not to send, the template and subjects stay in the map
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
            cout << "Template saved for sending/editing." << endl;
            ConsoleUtils::resetColor();
            templateExists = true;
        }
    }
}

void EmailManager::emailCustomAddress(CURL* curl,
                                      const std::string& senderEmail,
                                      std::string& subject,
                                      std::string& message,
                                      const std::string& smtpServer,
                                      int smtpPort,
                                      std::string& attachmentName,
                                      std::string& attachmentSize,
                                      std::string& attachmentPath) {

    const string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;


    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value

    // Reset the count
    successfulSends = 0;

    // Declarations
    string recipientEmail;

    while(true) {
        string recipientEmail, line;

        // Prompt the user for the custom email address
        do {
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Enter the custom email address: ";
            ConsoleUtils::resetColor();
            cin >> recipientEmail;
            ConsoleUtils::clearInputBuffer();

            if (isValidEmail(recipientEmail)) {
                break;
            } else {
                cout << "Invalid email address format. Please enter a valid email address." << endl;
            }
        } while (true);

        // Construct the email directly in this function
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Enter the subject of the email (press Enter on a new line to finish):\n";
        ConsoleUtils::resetColor();
        while (getline(cin, line) && !line.empty()) {
            subject += line + "\n";
        }

        if (subject.length() > maxSubjectLength) {
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_LENGTH_ERROR);
        #ifndef UNIT_TESTING
            cout << "Press return to go back..." << endl;
            cin.ignore();
            ConsoleUtils::clearInputBuffer();
            cin.get();
        #endif
            subject.clear();
            return;
        }

        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Enter the message body of the email (press Enter on a new line to finish):\n";
        ConsoleUtils::resetColor();
        while (getline(cin, line) && !line.empty()) {
            message += line + "\n";
        }

        if (message.length() + line.length() > maxMessageLength) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_MESSAGE_LENGTH_ERROR);
        #ifndef UNIT_TESTING
            cout << "Press return to go back..." << endl;
            cin.ignore();
            ConsoleUtils::clearInputBuffer();
            cin.get();
        #endif
            int charsToAdd = maxMessageLength - message.length();
            message += ConsoleUtils::trim(line).substr(0, charsToAdd);
            break;
        }

        // Prompt the user to add an attachment
        while (true) {
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Enter the path of the file to attach (or press Enter to skip): ";
            ConsoleUtils::resetColor();
            getline(cin, attachmentPath);
            attachmentPath.erase(remove(attachmentPath.begin(), attachmentPath.end(), '\''), attachmentPath.end());
            attachmentPath = ConsoleUtils::trim(attachmentPath);

            if (attachmentPath.empty()) {
                break;
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
                    size_t fileSize = filesystem::file_size(attachmentPath);
                    attachmentSize = to_string(fileSize) + " bytes";
                    cout << "File Size: " << fileSize << " bytes" << endl;

                    // Check the attachment size doesn't exceed 24MB
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        clearAttachmentData(attachmentName, attachmentSize, attachmentPath);
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
                        cout << "Do you want to add a different attachment? (Y/N): ";
                        ConsoleUtils::resetColor();
                        char choice;
                        cin >> choice;
                        ConsoleUtils::clearInputBuffer();
                        if (choice == 'Y' || choice == 'y') {
                            continue; // Go back to asking for a new file
                        } else {
                            return; // Exit the loop and function
                        }
                    }

                    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA); // Setting color for attachment details
                    // Show the attachment name, size and path to the user
                    cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
                    ConsoleUtils::resetColor();
                    break;  // Exit the loop if the file is valid
                } else {
                    // Error handling for attachment path
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                    return;  // Exit the function if the path is invalid
                }
            } catch (const filesystem::filesystem_error& e) {
                // Error handling for filesystem errors
                ErrorHandler errorHandler;
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
                return;  // Exit the function if a filesystem error occurs
            }
        }

        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
        cout << "-------------------------\n";
        cout << "----- EMAIL DETAILS -----\n";
        cout << "-------------------------\n";
        cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
        cout << "Subject: " << subject << "\n";
        ConsoleUtils::resetColor();
        ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
        cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
        cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
        cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
        ConsoleUtils::resetColor();
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
        cout << "\n" << message << "\n";
        cout << "-------------------------\n";
        ConsoleUtils::resetColor();
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Do you wish to modify this email? (Y/N): ";
        ConsoleUtils::resetColor();
        char modifyEmailChoice;
        cin >> modifyEmailChoice;
        ConsoleUtils::clearInputBuffer();

        if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
            clearAllEmailData(subject, message, attachmentName, attachmentSize, attachmentPath);
            continue; // Loop back to the start of the function
        } else {
            break; // Exit the loop if the user is satisfied
        }
    }

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Do you wish to send this email? (Y/N): ";
    ConsoleUtils::resetColor();
    char confirmSend;
    cin >> confirmSend;
    ConsoleUtils::clearInputBuffer();

    // Proceed to send emails if confirmed
    if (confirmSend == 'Y' || confirmSend == 'y') {

        // Prepare a dummy SelectedVenue object to use with sendIndividualEmail function
        SelectedVenue customVenue;
        customVenue.email = recipientEmail;

        if (!curl) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
            return;
        }

        if (!isValidEmail(senderEmail)) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
            cerr << "Please set it correctly in your custom.json file." << endl;
            return;
        }

        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
        ConsoleUtils::resetColor();

        struct curl_slist* recipients = nullptr;
        recipients = curl_slist_append(recipients, customVenue.email.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        string toHeader = "To: " + customVenue.email;
        string fromHeader = "From: " + senderEmail;
        string subjectHeader = "Subject: " + subject;

        struct curl_slist* headers = nullptr;
        string dateHeader = "Date: " + getCurrentDateRfc2822();
        headers = curl_slist_append(headers, dateHeader.c_str());
        headers = curl_slist_append(headers, toHeader.c_str());
        headers = curl_slist_append(headers, fromHeader.c_str());
        headers = curl_slist_append(headers, subjectHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_mime *mime = nullptr;

        if (!attachmentPath.empty()) {
            mime = curl_mime_init(curl);

            size_t fileSize = filesystem::file_size(attachmentPath);
            attachmentSize = to_string(fileSize) + " bytes";

            // Add the message part
            curl_mimepart *part = curl_mime_addpart(mime);
            curl_mime_data(part, message.c_str(), CURL_ZERO_TERMINATED);

            // Add the attachment part
            part = curl_mime_addpart(mime);
            curl_mime_filedata(part, attachmentPath.c_str());

            // Retrieve attachment filename
            curl_mime_filename(part, attachmentName.c_str());

            // Set MIME type for attachment
            curl_mime_type(part, "application/octet-stream");

            curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        } else {
            string payload = dateHeader + "\r\n"
                             + toHeader + "\r\n"
                             + fromHeader + "\r\n"
                             + subjectHeader + "\r\n\r\n"
                             + message;
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHandleWrapper::readCallback);
            curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(payload.length()));
            }

            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Authenticating with SMTP server..." << endl;
            ConsoleUtils::resetColor();
            cout.flush();
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            // Set totalEmails to 1 as we are only sending 1 email
            totalEmails = 1;

            res = curl_easy_perform(curl);
            if (res == 0) { // Check if email was sent successfully
                successfulSends = 1;
                int progressPercentage;
                if (totalEmails != 0) {
                    progressPercentage = 100;
                }
                ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
                cout << "Progress: " << progressPercentage << "%" << endl;
                ConsoleUtils::resetColor();
                cout.flush();
            }

            // Free the MIME structure
            if (mime) {
                curl_mime_free(mime);
            }

            curl_slist_free_all(recipients);
            curl_slist_free_all(headers);

            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
            cout << "Email sending progress completed." << endl;
            ConsoleUtils::resetColor();

            // Clear the subject, message, and attachment strings
            clearAllEmailData(subject, message, attachmentName, attachmentSize, attachmentPath);
            return;

        if (!errorHandler.handleCurlError(res)) {
            if (res == CURLE_COULDNT_CONNECT) {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_CONNECTION_ERROR);
            } else if (res == CURLE_LOGIN_DENIED) {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_AUTH_ERROR);
            }
        }

    } else {
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);              
        cout << "Email saved for sending/editing." << endl;
        ConsoleUtils::resetColor();
        return;
    }
}

void EmailManager::confirmSendEmail(CURL* curl,
                      vector<SelectedVenue>& selectedVenuesForEmail,
                      const string& senderEmail,
                      string& subject,
                      string& message,
                      const string& smtpServer,
                      int smtpPort,
                      string& attachmentName,
                      string& attachmentSize,
                      string& attachmentPath) {

    if (subject.empty() && message.empty()) {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "Email not created yet." << endl;
        ConsoleUtils::resetColor();
        return;
    }

    // Check if any venues are selected for email sending
    if (selectedVenuesForEmail.empty()) {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "No venues have been selected. Please select venues first before attempting to send the email." << endl;
        ConsoleUtils::resetColor();
        return;  // Exit the function
    }

    // Display a summary of all emails to be sent.
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "===========================" << endl;
    cout << "===== Email Summary ======" << endl;
    cout << "===========================" << endl;
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA); // Setting color for attachment details
    cout << "Number of emails to send: " << selectedVenuesForEmail.size() << endl;
    cout << "Subject: " << subject << endl;
    cout << "Message: " << message << endl;
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
    ConsoleUtils::resetColor();
    cout << "===========================" << endl;

    // Confirm with the user that they want to proceed.
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Do you wish to send these emails? (Y/N): ";
    ConsoleUtils::resetColor();
    char confirmSend;
    cin >> confirmSend;
    ConsoleUtils::clearInputBuffer();

    if (confirmSend != 'Y' && confirmSend != 'y') {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "Email sending aborted by user." << endl;
        ConsoleUtils::resetColor();
        return;
    }

    // Send each email, displaying progress as it goes.
    int sendCount = 0;
    for (const auto& venue : selectedVenuesForEmail) {
        if (sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail)) {
            sendCount++;
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
            cout << "Sent email " << sendCount << " of " << selectedVenuesForEmail.size() << " to " << venue.email << endl;
            ConsoleUtils::resetColor();
        } else {
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
            cout << "Failed to send email to " << venue.email << endl;
            ConsoleUtils::resetColor();
        }
    }

    // Display a summary once all emails are sent.
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
    cout << "===========================" << endl;
    cout << "Email sending completed. " << endl;
    cout << "Sent: " << sendCount << "/" << selectedVenuesForEmail.size() << endl;
    cout << "===========================" << endl;
    ConsoleUtils::resetColor();
}

void EmailManager::confirmSendBookingTemplates(CURL* curl,
                                               vector<SelectedVenue>& selectedVenuesForTemplates,
                                               const string& senderEmail,
                                               map<string, pair<string, string>>& emailToTemplate,
                                               const string& smtpServer,
                                               int smtpPort,
                                               string& attachmentName,
                                               string& attachmentSize,
                                               string& attachmentPath) {

    if (emailToTemplate.empty()) {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "No booking templates have been created yet." << endl;
        ConsoleUtils::resetColor();
        return;
    }

    // Check if any venues are selected for sending templates
    if (emailToTemplate.empty()) {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "No venues have been selected. Please select venues first before attempting to send the templates." << endl;
        ConsoleUtils::resetColor();
        return;  // Exit the function
    }

    // Display a summary of all templates to be sent.
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "===============================" << endl;
    cout << "===== Booking Template Summary =====" << endl;
    cout << "===============================" << endl;
    ConsoleUtils::resetColor();

    // Display the first template as an example
    auto firstElement = emailToTemplate.begin();
    string firstEmail = firstElement->first;
    string firstSubject = firstElement->second.first;
    string firstMessage = firstElement->second.second;

    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "Example Template for: " << firstEmail << "\n";
    cout << "Subject: " << firstSubject << "\n";
    cout << "Message: " << firstMessage << "\n";
    ConsoleUtils::resetColor();

    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA); // Setting color for attachment details
    cout << "Number of templates to send: " << selectedVenuesForTemplates.size() << endl;
    // More details about the templates here...
    ConsoleUtils::resetColor();
    cout << "===============================" << endl;

    // Confirm with the user that they want to proceed.
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Do you wish to send these templates? (Y/N): ";
    ConsoleUtils::resetColor();
    char confirmSend;
    cin >> confirmSend;
    ConsoleUtils::clearInputBuffer();

    if (confirmSend != 'Y' && confirmSend != 'y') {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "Template sending aborted by user." << endl;
        ConsoleUtils::resetColor();
        return;
    }

    // Send each template, displaying progress as it goes.
    bool sendStatus = sendBookingTemplateEmails(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);

    // Display a summary once all templates are sent.
    ConsoleUtils::setColor(sendStatus ? ConsoleUtils::Color::GREEN : ConsoleUtils::Color::RED);
    cout << "===============================" << endl;
    cout << "Template sending " << (sendStatus ? "completed." : "failed.") << endl;
    cout << "===============================" << endl;
    ConsoleUtils::resetColor();
}

