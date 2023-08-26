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
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif
    cout << "SMTP Server: " << smtpServer << endl
         << "SMTP Port: " << smtpPort << endl
         << "Sender Email: " << senderEmail << endl
         << "SSL: " << (useSSL ? "true" : "false") << endl
         << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl
         << "verifyHost: " << (verifyHost ? "true" : "false") << endl
         << "verbose: " << (verbose ? "true" : "false") << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif         
}

// Function to validate if the provided string is a valid email address format
bool EmailManager::isValidEmail(const string& email) {
    static const regex emailPattern(R"((?=.{1,256})(?=.{1,64}@.{1,255})[^\s@]+@[^\s@]+\.[^\s@]+)");
    return regex_match(email, emailPattern);
}

// Function to guide the user in constructing an email with a subject, message, and optional attachment
void EmailManager::constructEmail(string& subject, string& message, string& attachmentName, string& attachmentSize, string& attachmentPath, istream &in) {
    
    clearAllEmailData(subject, message, attachmentName, attachmentSize, attachmentPath);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "===========================" << endl;
    cout << "===== Construct Email =====" << endl;
    cout << "===========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif       
    // Prompt the user to enter the subject and perform checks
    do {
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "Press return to go back..." << endl;
        ConsoleUtils::clearInputBuffer();
        cin.get();
        ConsoleUtils::resetColor();
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
                ConsoleUtils::setColor(ConsoleUtils::Color::RED);
                cout << "Press return to go back..." << endl;
                ConsoleUtils::clearInputBuffer();
                cin.get();
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
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_BLANK_ERROR);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
            cout << "Press return to go back..." << endl;
            ConsoleUtils::clearInputBuffer();
            cin.get();
            ConsoleUtils::resetColor();
#endif
        }

        if (!inputProvided) {
            break;
        }

    } while (!inputProvided);

    // Prompt the user to add an attachment
        while (true) {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Do you want to add an attachment? (Y/N): ";
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif  
            char addAttachmentChoice;
            cin >> addAttachmentChoice;
            ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

        if (addAttachmentChoice == 'N' || addAttachmentChoice == 'n') {
            break;

        } else if (addAttachmentChoice == 'Y' || addAttachmentChoice == 'y') {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cout << "Enter the path of the file to attach (or press Enter to skip): ";
            getline(cin, attachmentPath);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif  
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
                    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif                   
                    size_t fileSize = filesystem::file_size(attachmentPath);
                    attachmentSize = to_string(fileSize) + " bytes";
                    cout << "File Size: " << fileSize << " bytes" << endl;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif  
                    // Check the attachment size doesn't exceed 24MB
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        clearAttachmentData(attachmentName, attachmentSize, attachmentPath);
#ifndef UNIT_TESTING
                        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif                  
                        cout << "Do you want to add a different attachment? (Y/N): ";
#ifndef UNIT_TESTING
                        ConsoleUtils::resetColor();
#endif  
                        char choice;
                        cin >> choice;
                        ConsoleUtils::clearInputBuffer();
                        if (choice == 'Y' || choice == 'y') {
                            continue; // Go back to asking for a new file
                        } else {
                            return; // Exit the loop and function
                        }
                    }
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif   
                    // Show the attachment name, size and path to the user
                    cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif  
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
                return;
                }

            } else {
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif  
                cout << "Invalid choice. Please enter Y or N." << endl;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif  
                continue;
            }
        }
        // Display a success message
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif 
        cout << "Email has been successfully created." << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif  
}

void EmailManager::viewEditEmails(CURL* curl, const string& smtpServer, int smtpPort, vector<SelectedVenue>& selectedVenuesForEmail, const string& senderEmail, 
                                  string& subject, string& message, string& attachmentName, string& attachmentSize, string& attachmentPath, bool& templateExists,
                                  map<string, pair<string, string>>& emailToTemplate) {

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "===========================\n";
    cout << "       EMAIL DETAILS       \n";
    cout << "===========================\n";
    cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
    cout << "Subject: " << subject << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif 
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "\n" << message << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "-------------------------\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 

    int attempts = 0;
    bool modified = false;

    while (attempts < 3) {

#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);    
#endif 
        cout << "Do you wish to modify this email? (Y/N): ";
        ConsoleUtils::resetColor();
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 

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
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);           
#endif   
            cout << "Email saved for sending/editing." << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
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

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "=============================\n";
    cout << "      TEMPLATE DETAILS       \n";
    cout << "=============================\n";
    cout << "For: " << firstEmail << "\n";
    cout << "Subject: " << firstSubject << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "\n" << firstMessage << "\n";
    cout << "-----------------------------\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    int attempts = 0;

    while (attempts < 3) {

#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Do you wish to modify this template? (Y/N): ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        char modifyTemplateChoice;
        cin >> modifyTemplateChoice;
        ConsoleUtils::clearInputBuffer();

        if (modifyTemplateChoice == 'Y' || modifyTemplateChoice == 'y') {
            clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
            createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort,
                                  attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Template saved for sending/editing." << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
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
                                       const vector<SelectedVenue>& selectedVenuesForEmail) {

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value

    if (!curl) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    // Update totalEmails dynamically
    totalEmails = selectedVenuesForEmail.size();

    if (!isValidEmail(senderEmail)) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
        return false;
    }

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    struct curl_slist* recipients = nullptr;

    if (selectedVenuesForEmail.size() <= 49) {
        // Use "To:" for each email
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

            cout << "Authenticating with SMTP server..." << endl;
            cout.flush();
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            res = curl_easy_perform(curl);
            if (res == 0) { // Check if email was sent successfully
                successfulSends++;
                double progressPercentage = 0.0;
                if (totalEmails != 0) {
                    progressPercentage = (static_cast<double>(successfulSends) / totalEmails) * 100;
                }
                cout << "Progress: " << progressPercentage << "%" << endl;
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

    // cURL headers
    struct curl_slist* headers = nullptr;
    string dateHeader = "Date: " + getCurrentDateRfc2822();
    string toHeader = "To: " + selectedVenue.email;
    string fromHeader = "From: " + senderEmail;
    string subjectHeader = "Subject: " + subject;

    headers = curl_slist_append(headers, dateHeader.c_str());
    headers = curl_slist_append(headers, toHeader.c_str());
    headers = curl_slist_append(headers, fromHeader.c_str());
    headers = curl_slist_append(headers, subjectHeader.c_str());

    // Set headers
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_mime *mime = nullptr;

    mime = curl_mime_init(curl);

    // Add the message part
    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_data(part, message.c_str(), CURL_ZERO_TERMINATED);

    if (!attachmentPath.empty()) {

        size_t fileSize = filesystem::file_size(attachmentPath);
        attachmentSize = to_string(fileSize) + " bytes";

        // Add the attachment part
        part = curl_mime_addpart(mime);
        curl_mime_filedata(part, attachmentPath.c_str());

        // Retrieve attachment filename
        curl_mime_filename(part, attachmentName.c_str());

        // Set MIME type for attachment
        curl_mime_type(part, "application/octet-stream");
    }

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Authenticating with SMTP server..." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
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
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
        cout << "Progress: " << progressPercentage << "%" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        cout.flush();
    }

    // Free the MIME structure
    if (mime) {
        curl_mime_free(mime);
    }

    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
    cout << "Email sending progress completed." << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

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
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cerr << "Failed to send email to " << recipientEmail << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
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
                                       const vector<SelectedVenue>& selectedVenuesForEmail,
                                       bool templateExists) {
    // Check if venues are selected
    if (selectedVenuesForEmail.empty()) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cerr << "Error: No venues have been selected. Please select venues first before attempting to send the template." << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        return;  // Exit the function
    }

    ConsoleUtils::clearInputBuffer();

    // Prompt the user for the required data to fill the placeholders
    string genre, bandName, hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, pressQuote, name, socials;

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Genre: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, genre);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Band Name: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, bandName);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Hometown: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, hometown);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Similar Artists: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, similarArtists);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Date: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, date);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Music Link: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, musicLink);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Live Performance Video Link: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, livePerfVideo);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Music Video Link: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, musicVideo);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Press Quote: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, pressQuote);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Your Name: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    getline(cin, name);

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Enter Social Links: ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
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
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Do you want to add an attachment? (Y/N): ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    char addAttachmentChoice;
    cin >> addAttachmentChoice;
    ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

    if (addAttachmentChoice == 'N' || addAttachmentChoice == 'n') {
        break;
        
    } else if (addAttachmentChoice == 'Y' || addAttachmentChoice == 'y') {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Enter the path of the file to attach (or press Enter to skip): ";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        getline(cin, attachmentPath);
        if (attachmentPath.empty()) {
            break; // No attachment provided, move on
        }
        attachmentPath.erase(remove(attachmentPath.begin(), attachmentPath.end(), '\''), attachmentPath.end());
        attachmentPath = ConsoleUtils::trim(attachmentPath);

        size_t lastSlash = attachmentPath.find_last_of("/\\\\");
        if (lastSlash == string::npos) {
            attachmentName = attachmentPath;
        } else {
            attachmentName = attachmentPath.substr(lastSlash + 1);
        }

        try {
            if (filesystem::exists(attachmentPath)) {
                size_t fileSize = filesystem::file_size(attachmentPath);
                attachmentSize = to_string(fileSize) + " bytes";
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif
                cout << "File Size: " << fileSize << " bytes" << endl;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                if (fileSize > MAX_ATTACHMENT_SIZE) {
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                    clearAttachmentData(attachmentName, attachmentSize, attachmentPath);
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                    cout << "Do you want to add a different attachment? (Y/N): ";
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    char choice;
                    cin >> choice;
                    ConsoleUtils::clearInputBuffer();
                    if (choice == 'Y' || choice == 'y') {
                        continue; // Go back to asking for a new file
                    } else {
                        break; // Exit the loop without an attachment
                    }
                }

#ifndef UNIT_TESTING                
                ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif
                cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                break;  // Exit the loop if the file is valid
            } else {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
            }
        } catch (const filesystem::filesystem_error& e) {
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
        }
        
        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            cout << "Invalid choice. Please enter Y or N." << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            continue;
        }
    }


    // Ask user if they want to modify or send
    char choice;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "Do you wish to modify this template? (Y/N): ";
    cin >> choice;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    if (choice == 'Y' || choice == 'y') {
        // Clear the existing template and start over
        clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
        createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
    } else {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cout << "Do you want to send the template? (Y/N): ";
        cin >> choice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        if (choice == 'Y' || choice == 'y') {
            templateExists = false; // Reset the flag since we're sending the email

            // Now, send the email to all venues
            bool sent = sendBookingTemplateEmails(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
            if (!sent) {
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::RED);
                cerr << "Failed to send email templates." << endl;
#endif
            }
            // No need to set templateExists here again, as it's already set to false
        } else {
            // If user chooses not to send, the template and subjects stay in the map
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Template saved for sending/editing." << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            templateExists = true;
        }
    }
}

void EmailManager::emailCustomAddress(CURL* curl,
                                      const std::string& senderEmail,
                                      std::string& customAddressSubject,
                                      std::string& customAddressMessage,
                                      const std::string& smtpServer,
                                      int smtpPort,
                                      std::string& customAddressAttachmentName,
                                      std::string& customAddressAttachmentSize,
                                      std::string& customAddressAttachmentPath) {

    const string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value

    if (!curl) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return;
    }

    // Reset the count
    successfulSends = 0;

    // Declarations
    string customAddressRecipientEmail;

    while(true) {
        string customAddressLine;

        // Prompt the user for the custom email address
        do {
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Enter the custom email address: ";
            ConsoleUtils::resetColor();
            cin >> customAddressRecipientEmail;
            ConsoleUtils::clearInputBuffer();

            if (isValidEmail(customAddressRecipientEmail)) {
                break;
            } else {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::RECIPIENT_EMAIL_FORMAT_ERROR);
                return;
            }
        } while (true);

        do {
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Enter subject for the email (press Enter on a new line to finish): ";
            ConsoleUtils::resetColor();
            string customAddressLine;
            while (getline(cin, customAddressLine)) {
                if (customAddressLine.empty()) break;
                customAddressSubject += sanitizeSubject(customAddressLine) + " ";
            }
            customAddressSubject = ConsoleUtils::trim(customAddressSubject);

        } while (customAddressSubject.empty());

        if (customAddressSubject.length() > maxSubjectLength) {
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_LENGTH_ERROR);
        #ifndef UNIT_TESTING
            cout << "Press return to go back..." << endl;
            cin.ignore();
            ConsoleUtils::clearInputBuffer();
            cin.get();
        #endif
            customAddressSubject.clear();
            return;
        }

        // Read multiple lines for the message body of the email
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Enter the message body of the email (press Enter on a new line to finish):\n";
        ConsoleUtils::resetColor();
        while (getline(cin, customAddressLine) && !customAddressLine.empty()) {
            customAddressMessage += customAddressLine + "\n";
        }

        if (customAddressMessage.length() + customAddressLine.length() > maxMessageLength) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_MESSAGE_LENGTH_ERROR);
        #ifndef UNIT_TESTING
            cout << "Press return to go back..." << endl;
            cin.ignore();
            ConsoleUtils::clearInputBuffer();
            cin.get();
        #endif
            int charsToAdd = maxMessageLength - customAddressMessage.length();
            customAddressMessage += ConsoleUtils::trim(customAddressLine).substr(0, charsToAdd);
            break;
        }

        while (true) {
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Do you want to add an attachment? (Y/N): ";
            ConsoleUtils::resetColor();
            char addAttachmentChoice;
            cin >> addAttachmentChoice;
            ConsoleUtils::clearInputBuffer();  // Assuming this function clears the input buffer

            if (addAttachmentChoice == 'N' || addAttachmentChoice == 'n') {
                break;

        } else if (addAttachmentChoice == 'Y' || addAttachmentChoice == 'y') {
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Enter the path of the file to attach (or press Enter to skip): ";
            ConsoleUtils::resetColor();
            getline(cin, customAddressAttachmentPath);
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
                size_t fileSize = filesystem::file_size(customAddressAttachmentPath);
                customAddressAttachmentSize = to_string(fileSize) + " bytes";
                cout << "File Size: " << fileSize << " bytes" << endl;

                if (fileSize > MAX_ATTACHMENT_SIZE) {
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                    clearAttachmentData(customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);
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

                ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
                cout << "Attachment: " << customAddressAttachmentName << " (" << customAddressAttachmentSize << ")" << endl;
                ConsoleUtils::resetColor();
                break;  // Exit the loop if the file is valid
            } else {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                return;
            }
        } catch (const filesystem::filesystem_error& e) {
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_EMPTY_ERROR);
            return;
        }

        } else {
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
            cout << "Invalid choice. Please enter Y or N." << endl;
            ConsoleUtils::resetColor();
            continue;
            }
        }


        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
        cout << "-------------------------\n";
        cout << "----- EMAIL DETAILS -----\n";
        cout << "-------------------------\n";
        cout << "From: \"Sender Name\" <" << senderEmail << ">\n";
        cout << "To: \"Recipient Email\" <" << customAddressRecipientEmail << ">\n";
        cout << "Subject: " << customAddressSubject << "\n";
        ConsoleUtils::resetColor();
        ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
        cout << "Attachment: " << (customAddressAttachmentName.empty() ? "None" : customAddressAttachmentName) << "\n";
        cout << "Size: " << (customAddressAttachmentSize.empty() ? "None" : customAddressAttachmentSize) << "\n";
        cout << "Path: " << (customAddressAttachmentPath.empty() ? "None" : customAddressAttachmentPath) << "\n";
        ConsoleUtils::resetColor();
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
        cout << "\n" << customAddressMessage << "\n";
        cout << "-------------------------\n";
        ConsoleUtils::resetColor();
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
        cout << "Do you wish to modify this email? (Y/N): ";
        ConsoleUtils::resetColor();
        char modifyEmailChoice;
        cin >> modifyEmailChoice;
        ConsoleUtils::clearInputBuffer();

        if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
            clearAllCustomAddressEmailData(customAddressSubject, customAddressMessage, customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);
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

            if (!curl) {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
                return;
            }

            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
            ConsoleUtils::resetColor();
            
            struct curl_slist* recipients = nullptr;
            recipients = curl_slist_append(recipients, customAddressRecipientEmail.c_str());
            curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

            // cURL headers
            struct curl_slist* headers = nullptr;
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

            curl_mime *mime = nullptr;
            
            mime = curl_mime_init(curl);

            // Add the message part
            curl_mimepart *part = curl_mime_addpart(mime);
            curl_mime_data(part, customAddressMessage.c_str(), CURL_ZERO_TERMINATED);

             if (!customAddressAttachmentPath.empty()) {
                // Add the attachment part conditionally
                part = curl_mime_addpart(mime);
                curl_mime_filedata(part, customAddressAttachmentPath.c_str());
                curl_mime_filename(part, customAddressAttachmentName.c_str());
                curl_mime_type(part, "application/octet-stream");
            }

            curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
            cout << "Authenticating with SMTP server..." << endl;
            ConsoleUtils::resetColor();
            cout.flush();
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            // Perform the operation
            res = curl_easy_perform(curl);
            if (res == 0) { // Check if email was sent successfully
                successfulSends = 1;
                ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
                cout << "Email sent successfully." << endl;
                ConsoleUtils::resetColor();
                cout.flush();
            } else {
                // Handle error
                ConsoleUtils::setColor(ConsoleUtils::Color::RED);
                cout << "Email sending failed. Error code: " << res << endl;
                ConsoleUtils::resetColor();
                cout.flush();
                // You can log the error, display more details, or take appropriate actions here
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
            clearAllCustomAddressEmailData(customAddressSubject, customAddressMessage, customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);
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

