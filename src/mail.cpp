#include "include/mail.h"

// Use the standard namespace and alias for filesystem
using namespace std;
namespace fs = filesystem;

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
    cout << "=========================="<< endl;
    cout << "===== Email Settings =====" << endl;
    cout << "=========================="<< endl;
    cout << "SMTP Server: " << smtpServer << endl;
    cout << "SMTP Port: " << smtpPort << endl;
    cout << "Sender Email: " << senderEmail << endl;
    cout << "SSL: " << (useSSL ? "true" : "false") << endl;
    cout << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl;
    cout << "verifyHost: " << (verifyHost ? "true" : "false") << endl;
    cout << "verbose: " << (verbose ? "true" : "false") << endl;    
    cout << "==========================" << endl;

    // If not in unit testing mode, prompt the user to acknowledge the displayed information
}

// Function to validate if the provided string is a valid email address format
bool EmailManager::isValidEmail(const string& email) {
    static const regex emailPattern(R"((?=.{1,256})(?=.{1,64}@.{1,255})[^\s@]+@[^\s@]+\.[^\s@]+)");
    return regex_match(email, emailPattern);
}

// Function to guide the user in constructing an email with a subject, message, and optional attachment
void EmailManager::constructEmail(string &subject, string &message, string &attachmentName, string &attachmentSize, string &attachmentPath, istream &in) {
    cout << "===========================" << endl;
    cout << "===== Construct Email =====" << endl;
    cout << "===========================" << endl;

    // Prompt the user to enter the subject and perform checks
    do {
        cout << "Enter subject for the email (press Enter on a new line to finish): ";
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
        cout << "Enter the message for the email (press Enter on a new line to finish):\n";
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
        cout << "Enter the path of the file to attach: ";
        getline(cin, attachmentPath);
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
            if (fs::exists(attachmentPath)) {
                size_t fileSize = fs::file_size(attachmentPath);
                attachmentSize = to_string(fileSize) + " bytes";
                cout << "File Size: " << fileSize << " bytes" << endl;

                // Check the attachment size doesn't exceed 24MB
                if (fileSize > MAX_ATTACHMENT_SIZE) {
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                    attachmentPath.clear();
                    attachmentName.clear();
                    attachmentSize.clear();
                    cout << "Do you want to add a different attachment? (Y/N): ";
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
            return;  // Exit the function if a filesystem error occurs
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
                                       const string& attachmentPath,
                                       const vector<SelectedVenue>& selectedVenuesForEmail) {

    CURLcode res = CURLE_FAILED_INIT;  // Initialize to a default value

    // Reset the count
    successfulSends = 0;

    //Update totalEmails dynamically
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
    
    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;

    struct curl_slist* recipients = nullptr;
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

        cout << "Authenticating with SMTP server..." << endl;
        cout.flush();
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        //Update totalEmails dynamically
        totalEmails = selectedVenuesForEmail.size();

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

        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);

        cout << "Email sending progress completed." << endl;

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
            cout << "Enter the custom email address: ";
            cin >> recipientEmail;
            ConsoleUtils::clearInputBuffer();

            if (isValidEmail(recipientEmail)) {
                break;
            } else {
                cout << "Invalid email address format. Please enter a valid email address." << endl;
            }
        } while (true);

        // Construct the email directly in this function
        cout << "Enter the subject of the email (press Enter on a new line to finish):\n";
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

        cout << "Enter the message body of the email (press Enter on a new line to finish):\n";
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
            cout << "Enter the path of the file to attach: ";
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
                if (fs::exists(attachmentPath)) {
                    size_t fileSize = fs::file_size(attachmentPath);
                    attachmentSize = to_string(fileSize) + " bytes";
                    cout << "File Size: " << fileSize << " bytes" << endl;

                    // Check the attachment size doesn't exceed 24MB
                    if (fileSize > MAX_ATTACHMENT_SIZE) {
                        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_SIZE_ERROR);
                        attachmentPath.clear();
                        attachmentName.clear();
                        attachmentSize.clear();
                        cout << "Do you want to add a different attachment? (Y/N): ";
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
                return;  // Exit the function if a filesystem error occurs
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
            attachmentName.clear();
            attachmentSize.clear();
            attachmentPath.clear();
            continue; // Loop back to the start of the function
        } else {
            break; // Exit the loop if the user is satisfied
        }
    }

    cout << "Do you wish to send this email? (Y/N): ";
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

        cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;

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

            cout << "Authenticating with SMTP server..." << endl;
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
                cout << "Progress: " << progressPercentage << "%" << endl;
                cout.flush();
            }

            // Free the MIME structure
            if (mime) {
                curl_mime_free(mime);
            }

            curl_slist_free_all(recipients);
            curl_slist_free_all(headers);

            cout << "Email sending progress completed." << endl;

            // Clear the subject, message, and attachment strings
            subject.clear();
            message.clear();
            attachmentName.clear();
            attachmentSize.clear();
            attachmentPath.clear();
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

        // Clear the subject, message, and attachment strings
        subject.clear();
        message.clear();
        attachmentName.clear();
        attachmentSize.clear();
        attachmentPath.clear();
        return;
    }
}
