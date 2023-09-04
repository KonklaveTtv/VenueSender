#include "include/mail.h"

// Use the standard namespace and alias for filesystem
using namespace std;

// Global progress counters
static int successfulSends = 0; // Counter for successful email sends
static int successfulCustomSends = 0; // Counter for successful custom email sends
static int successfulTemplateSends = 0; // Counter for successful template sends

// Global definitions for email limits
int totalEmails;
int totalCustomEmails;
int totalTemplateEmails;

// Function to display current email settings
void EmailManager::viewEmailSettings(bool useSSL, const string& sslCertPath, bool verifyPeer, bool verifyHost, bool verbose,
                                     const string& senderEmail, int smtpPort, const string& smtpServer) {
    // Display the email settings in a structured format
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "============================"<< endl
         << "       Email Settings       "<< endl
         << "============================"<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
    cout << "SMTP Server: " << smtpServer << endl
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
bool EmailManager::isValidURL(const std::string& url) {
    // Simple regex pattern for URL validation, you can make it more comprehensive
    boost::regex urlPattern("https?://[\\w\\.-]+");
    return boost::regex_match(url, urlPattern);
}

// Function to guide the user in constructing an email with a subject, message, and optional attachment
void EmailManager::constructEmail(string& subject, string& message, string& attachmentName, string& attachmentSize, string& attachmentPath, istream &in) const {
    
    clearAllEmailData(subject, message, attachmentName, attachmentSize, attachmentPath);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "==========================="<< endl;
    cout << "===== Construct Email ====="<< endl;
    cout << "==========================="<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    // Prompt the user to enter the subject and perform checks
    do {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ENTER_SUBJECT_FOR_EMAIL_MESSAGE);
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
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SUBJECT_LENGTH_ERROR);
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
        ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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

        while (getline(in, line)) {
            if (line.empty()) break;
            if (message.length() + line.length() > maxMessageLength) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_MESSAGE_LENGTH_ERROR);
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
                ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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

        if (addAttachmentChoice == 'N' || addAttachmentChoice == 'n') {
            break;

        } else if (addAttachmentChoice == 'Y' || addAttachmentChoice == 'y') {
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
                    attachmentSize = to_string(fileSize) + " bytes";
                    cout << "File Size: " << fileSize << " bytes" << endl;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif  
                    // Check the attachment size doesn't exceed 24MB
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        clearAttachmentData(attachmentName, attachmentSize, attachmentPath);
                 
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
                        if (choice == 'Y' || choice == 'y') {
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

void EmailManager::viewEditEmails(CURL* curl, const string& smtpServer, int smtpPort, vector<SelectedVenue>& selectedVenuesForEmail, const string& senderEmail, 
                                  string& subject, string& message, string& attachmentName, string& attachmentSize, string& attachmentPath, bool& templateExists,
                                  map<string, pair<string, string>>& emailToTemplate) const {

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
    cout << "-------------------------\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif 

    int attempts = RESET_SEND_COUNT_TO_ZERO;
    bool modified = false;

    while (attempts < 3) {

        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MODIFY_EMAIL_CONFIRMATION_MESSAGE);

        char modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
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
                    ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::SUBJECT_MESSAGE_ERROR);
                    clearSubjectMessageData(subject, message);
                    attempts++; // Increment the attempts
                    continue; // Loop back to prompt for email details again
                }
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
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);           
#endif      
            cout << "===========================" << endl;
            cout << "        Email saved        " << endl;
            cout << "===========================" << endl;
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
                                     bool& templateExists) const {

    if (emailToTemplate.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_PENDING_ERROR);
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
    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
    cout << "Attachment: " << (attachmentName.empty() ? "None" : attachmentName) << "\n";
    cout << "Size: " << (attachmentSize.empty() ? "None" : attachmentSize) << "\n";
    cout << "Path: " << (attachmentPath.empty() ? "None" : attachmentPath) << "\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "\n" << firstMessage << "\n";
    cout << "-----------------------------\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    int attempts = RESET_SEND_COUNT_TO_ZERO;

    while (attempts < 3) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MODIFY_TEMPLATE_CONFIRMATION_MESSAGE);
        char modifyTemplateChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> modifyTemplateChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

        if (modifyTemplateChoice == 'Y' || modifyTemplateChoice == 'y') {
            clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
            createBookingTemplate(curl, senderEmail, emailToTemplate, smtpServer, smtpPort,
                                  attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail, templateExists);
        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "===========================" << endl;
            cout << "      Template Saved       " << endl;
            cout << "===========================" << endl;
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
    struct curl_slist* recipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime *mime = nullptr;


    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    // Update totalEmails dynamically
    totalEmails = selectedVenuesForEmail.size();

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
    recipients = curl_slist_append(recipients, selectedVenue.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    // cURL headers
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
    if (recipients) {
    curl_slist_free_all(recipients);

    }
    if (headers) {
    curl_slist_free_all(headers);
    }

    if (res == 0) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
        cout << "===========================" << endl; 
        cout << "        Emails Sent        " << endl;
        cout << "===========================" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    } else {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cout << "===========================" << endl;
        cout << "    Email Sending Failed   " << endl;
        cout << "===========================" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
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
                                             map<string, pair<string, string>>& emailToTemplate,
                                             const string& smtpServer,
                                             int smtpPort,
                                             string& attachmentName,
                                             string& attachmentSize,
                                             string& attachmentPath) {

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value
        struct curl_slist* recipients = nullptr;
        struct curl_slist* headers = nullptr;
        curl_mime *mime = nullptr;

    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    if (!isValidEmail(senderEmail)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR);
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SENDER_EMAIL_FORMAT_ERROR, senderEmail);
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
        totalTemplateEmails = emailToTemplate.size();

        res = curl_easy_perform(curl);
        if (res == 0) { // Check if email was sent successfully
            successfulTemplateSends++;
            double progressPercentage = 0.0;
            if (totalTemplateEmails != 0) {
                progressPercentage = (static_cast<double>(successfulTemplateSends) / totalTemplateEmails) * 100;
            }
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
            cout << "Progress: " << progressPercentage << "%" << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            cout.flush();
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_SENDING_ERROR);
            continue;  // Skip to the next iteration
        }

        // Free the MIME structure
        if (mime) {
            curl_mime_free(mime);
        }

        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);
    }

    if (res == 0) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
        cout << "===========================" << endl; 
        cout << "        Emails Sent        " << endl;
        cout << "===========================" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    } else {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_SEND_FAILURE_ERROR);
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

// Helper function to conditionally append a line to the message
void EmailManager::appendIfNotEmpty(std::ostringstream& os, const std::string& label, const std::string& value) {
    if (!value.empty()) {
        os << label << ": " << value << "\n";
    }
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
                                         bool templateExists) const {

    // String declarations for the booking template
    string genre, performanceType, performanceName, hometown, similarArtists, date, musicLink, livePerfVideo, musicVideo, pressQuote, name, socials;

    char choice;
    bool modifyTemplate = true;

    auto getInputWithConfirmation = [&](string& input, const string& prompt, bool isMandatory = false, bool checkURL = false) {        bool inputConfirmed = false;
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
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                cin >> confirmation;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                ConsoleUtils::clearInputBuffer();

                if (confirmation == 'y' || confirmation == 'Y') {
                    inputConfirmed = true;
                }
            } else {
                if (checkURL && !isValidURL(input)) {
                    char confirmation;
                    cout << "The URL seems to be invalid. Is this correct? (Y/N): ";
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif 
                    cin >> confirmation;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                    ConsoleUtils::clearInputBuffer();
                    if (confirmation == 'N' || confirmation == 'n') {
                        continue;
                    }
                }
                inputConfirmed = true;
            }
        }
    };

    while (modifyTemplate) {
        // Check if venues are selected
        if (selectedVenuesForEmail.empty()) {
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
        getInputWithConfirmation(pressQuote, "Enter Press Quote (press Enter on a new line to finish): ", false, true);  // Added this line
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

        // Construct the email template for each venue without sending it
        for (const SelectedVenue& venue : selectedVenuesForEmail) {
            std::ostringstream os;
            
            // Declare and initialize mandatory parts of the email
            string subject = "Booking Inquiry for " + venue.name;

            os << "Hi!,\n\n"
               << "I am booking a tour for " << performanceName << " a " << genre << " " << performanceType << ", from \n\n"
               << hometown << ". The music is similar to " << similarArtists << ".\n\n"
               << "We're planning to be in the " << venue.city << " area on " << date << " and are\n\n"
               << "wondering if you might be interested in booking us at " << venue.name << ".\n\n";

            // Add optional fields only if they are not empty
            appendIfNotEmpty(os, "- Music", musicLink);
            appendIfNotEmpty(os, "- Live Performance", livePerfVideo);
            appendIfNotEmpty(os, "- Music Video", musicVideo);

            if (!pressQuote.empty()) {
                os << "\nWhat people are saying about " << performanceName << "\n"
                   << "\"" << pressQuote << "\"\n";
            }

            os << "\nPlease let me know if you have any questions or need additional information.\n\n"
               << "We appreciate your time and consideration!\n\n"
               << "Best wishes,\n"
               << name << "\n\n";

            appendIfNotEmpty(os, "-- Social Links", socials);

            // Map each venue's email to its unique message and subject
            emailToTemplate[venue.email] = make_pair(subject, os.str());
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

        if (addAttachmentChoice == 'N' || addAttachmentChoice == 'n') {
            break;
            
        } else if (addAttachmentChoice == 'Y' || addAttachmentChoice == 'y') {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ADD_ATTACHMENT_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            getline(cin, attachmentPath);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
            ConsoleUtils::clearInputBuffer();
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
                    size_t fileSize = boost::filesystem::file_size(attachmentPath);
                    attachmentSize = to_string(fileSize) + " bytes";
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::MAGENTA);
#endif
                    cout << "File Size: " << fileSize << " bytes" << endl;
#ifndef UNIT_TESTING
                    ConsoleUtils::resetColor();
#endif
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        clearAttachmentData(attachmentName, attachmentSize, attachmentPath);
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
                        if (choice == 'Y' || choice == 'y') {
                            continue; // Go back to asking for a new file
                        } else {
                            break; // Exit the loop without an attachment
                        }
                    }

#ifndef UNIT_TESTING                
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                    cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
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
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::ATTACHMENT_INVALID_CHOICE_MESSAGE);
                continue;
            }
        }

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


        if (choice == 'Y' || choice == 'y') {
            // Clear the existing template and start over
            clearBookingTemplate(emailToTemplate, attachmentName, attachmentSize, attachmentPath, templateExists);
        } else {
            modifyTemplate = false;
    
            // Ask the user if they want to send the template
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_TEMPLATE_MESSAGE);
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            cin >> choice;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

            if (choice == 'Y' || choice == 'y') {
                templateExists = false; // Reset the flag since we're sending the email

                // Now, send the email to all venues
                bool sent = sendBookingTemplateEmails(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);
                if (!sent) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::TEMPLATE_SENDING_FAILED_ERROR);
                }
            } else {
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
                cout << "===========================" << endl;
                cout << "      Template Saved       " << endl;
                cout << "===========================" << endl;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                // If user chooses not to send, the template and subjects stay in the map
                templateExists = true;
            }
        }
    }
}

void EmailManager::emailCustomAddress(CURL* curl,
                                      const string& senderEmail,
                                      string& customAddressSubject,
                                      string& customAddressMessage,
                                      const string& smtpServer,
                                      int smtpPort,
                                      string& customAddressAttachmentName,
                                      string& customAddressAttachmentSize,
                                      string& customAddressAttachmentPath) const {

    const string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value
    struct curl_slist* recipients = nullptr;
    struct curl_slist* headers = nullptr;
    curl_mime *mime = nullptr;

    if (!curl) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
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
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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

        while (getline(cin, customAddressLine) && !customAddressLine.empty()) {
            customAddressMessage += customAddressLine + "\n";
        }

        if (customAddressMessage.length() + customAddressLine.length() > maxMessageLength) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_MESSAGE_LENGTH_ERROR);
        #ifndef UNIT_TESTING
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PRESS_RETURN_MESSAGE);
            ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
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

            if (addAttachmentChoice == 'N' || addAttachmentChoice == 'n') {
                break;

        } else if (addAttachmentChoice == 'Y' || addAttachmentChoice == 'y') {
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
                size_t fileSize = boost::filesystem::file_size(customAddressAttachmentPath);
                customAddressAttachmentSize = to_string(fileSize) + " bytes";
#ifndef UNIT_TESTING
                    ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif
                cout << "File Size: " << fileSize << " bytes" << endl;
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
                    if (choice == 'Y' || choice == 'y') {
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

#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
        cout << "-------------------------\n";
        cout << "----- EMAIL DETAILS -----\n";
        cout << "-------------------------\n";
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
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
        cout << "\n" << customAddressMessage << "\n";
        cout << "-------------------------\n";
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::MODIFY_EMAIL_CONFIRMATION_MESSAGE);

        char modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        cin >> modifyEmailChoice;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        ConsoleUtils::clearInputBuffer();

        if (modifyEmailChoice == 'Y' || modifyEmailChoice == 'y') {
            clearAllCustomAddressEmailData(customAddressSubject, customAddressMessage, customAddressAttachmentName, customAddressAttachmentSize, customAddressAttachmentPath);
            continue; // Loop back to the start of the function
        } else {
            break; // Exit the loop if the user is satisfied
        }
    }

    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_EMAIL_MESSAGE);
    char confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cin >> confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    ConsoleUtils::clearInputBuffer();

        // Proceed to send emails if confirmed
        if (confirmSend == 'Y' || confirmSend == 'y') {

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
                double progressPercentage = 0.0;
                if (totalCustomEmails != 0 || totalCustomEmails == 1) {
                    progressPercentage = (static_cast<double>(successfulCustomSends) / totalCustomEmails) * 100;
                }
        #ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
        #endif
                cout << "Progress: " << progressPercentage << "%" << endl;
        #ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
        #endif
                cout.flush();
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
                cout << "Email sending failed. Error code: " << res << endl;
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor();
#endif
                cout.flush();
                // You can log the error, display more details, or take appropriate actions here
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

            if (res == 0) {
                MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAILS_SENT_MESSAGE);
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
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif           
            cout << "===========================" << endl;
            cout << "        Email Saved        " << endl;
            cout << "===========================" << endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif
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
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_NOT_CREATED_MESSAGE);
        return;
    }

    // Check if any venues are selected for email sending
    if (selectedVenuesForEmail.empty()) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::VENUES_NOT_SELECTED_MESSAGE);
        return;  // Exit the function
    }

    // Display a summary of all emails to be sent.
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "===========================" << endl;
    cout << "       Email Summary       " << endl;
    cout << "==========================="<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
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
    cout << "===========================" << endl;

    // Confirm with the user that they want to proceed.
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIRM_SEND_EMAILS_MESSAGE);
    char confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cin >> confirmSend;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    ConsoleUtils::clearInputBuffer();

    if (confirmSend != 'Y' && confirmSend != 'y') {
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
        if (sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail)) {
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
    cout << "==========================="<< endl;
    cout << "  Email Sending Completed  "<< endl;
    cout << " Sent: " << sendCount << "/"<< selectedVenuesForEmail.size() << endl;
    cout << "==========================="<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
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
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    cout << "============================"<< endl;
    cout << "  Booking Template Summary  "<< endl;
    cout << "============================"<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    // Display the first template as an example
    auto firstElement = emailToTemplate.begin();
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
    cout << "==============================="<< endl;

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

    if (confirmSend != 'Y' && confirmSend != 'y') {
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
    bool sendStatus = sendBookingTemplateEmails(curl, senderEmail, emailToTemplate, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath);

    // Set the initial color to Orange
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "==============================="<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    // Set the color based on sendStatus (Green if completed, Red if failed)
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(sendStatus ? ConsoleUtils::Color::GREEN : ConsoleUtils::Color::RED);
#endif
    cout << "Template sending " << (sendStatus ? "completed." : "failed.") << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    // Set the color back to Orange
#ifndef UNIT_TESTING    
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    cout << "==============================="<< endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

}

