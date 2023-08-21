#include "include/mail.h"

using namespace std;
namespace fs = std::filesystem;

CurlHandleWrapper curlHandleWrapper;
ErrorHandler errorHandler;

string EmailManager::getCurrentDateRfc2822() {
    char buffer[100];
    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now = gmtime(&now);
    strftime(buffer, sizeof buffer, "%a, %d %b %Y %H:%M:%S %Z", tm_now);
    return buffer;
}

string EmailManager::sanitizeSubject(string& subject) {
    string sanitized = subject;
    replace(sanitized.begin(), sanitized.end(), '\n', ' '); // replace newlines with spaces
    replace(sanitized.begin(), sanitized.end(), '\r', ' '); // replace carriage returns with spaces
    return sanitized;
}

void EmailManager::viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost, bool verbose,
                                     const string& senderEmail, int smtpPort, const string& smtpServer) {
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

#ifndef UNIT_TESTING
    errorHandler.showInfoAndRetry();
#endif
}

bool EmailManager::isValidEmail(const string& email) {
    static const regex emailPattern(R"((?=.{1,256})(?=.{1,64}@.{1,255})[^\s@]+@[^\s@]+\.[^\s@]+)");
    return regex_match(email, emailPattern);
}

void EmailManager::constructEmail(string &subject, string &message, string &attachmentName, string &attachmentSize, string &attachmentPath, istream &in) {
    cout << "===========================" << endl;
    cout << "===== Construct Email =====" << endl;
    cout << "===========================" << endl;

    do {
        cout << "Enter subject for the email (press Enter on a new line to finish): ";
        string line;
        while (getline(in, line)) {
            if (line.empty()) break;
            subject += sanitizeSubject(line) + " ";
        }
        subject = ConsoleUtils::trim(subject);

    } while (subject.empty());

    const std::string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const std::string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;

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

    while (true) {
        cout << "Enter the path of the file to attach: ";
        getline(cin, attachmentPath);
        attachmentPath.erase(std::remove(attachmentPath.begin(), attachmentPath.end(), '\''), attachmentPath.end());
        attachmentPath = ConsoleUtils::trim(attachmentPath);

        size_t lastSlash = attachmentPath.find_last_of("/\\\\");
        if (lastSlash == string::npos) {
            attachmentName = attachmentPath;
        } else {
            attachmentName = attachmentPath.substr(lastSlash + 1);
        }

        try {
            if (fs::exists(attachmentPath)) {
                size_t fileSize = fs::file_size(attachmentPath);
                attachmentSize = to_string(fileSize) + " bytes";
                cout << "File Size: " << fileSize << " bytes" << endl;

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
                        continue;  // Go back to asking for a new file
                    } else {
                        return;  // Exit the loop and function
                    }
                }

                cout << "Attachment: " << attachmentName << " (" << attachmentSize << ")" << endl;
                break;  // Exit the loop if the file is valid
            } else {
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::ATTACHMENT_PATH_ERROR);
                return;  // Exit the function if the path is invalid
            }
        } catch (const std::filesystem::filesystem_error& e) {
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, e.what());
            return;  // Exit the function if a filesystem error occurs
        }
    }
}

bool EmailManager::sendIndividualEmail(CURL* curl,
                                       const SelectedVenue& selectedVenue,
                                       const string& senderEmail,
                                       string& subject,
                                       string& message,
                                       const string& smtpServer,
                                       int smtpPort,
                                       string& attachmentName,
                                       string& attachmentSize,
                                       const string& attachmentPath) {
    curlHandleWrapper.setEmailBeingSent(selectedVenue.email);

    if (!curl) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
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

        size_t fileSize = std::filesystem::file_size(attachmentPath);
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
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    CURLcode res = curl_easy_perform(curl);

    // Free the MIME structure
    if (mime) {
        curl_mime_free(mime);
    }

    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);

    if (!checkCurlError(res, "")) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_ERROR); 
        if (res == CURLE_COULDNT_CONNECT) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_CONNECTION_ERROR);
        } else if (res == CURLE_LOGIN_DENIED) {
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::SMTP_AUTH_ERROR);
        }
        return false;
    }

    return true;
}


void EmailManager::viewEmailSendingProgress(const string& senderEmail) {
    if (!isValidEmail(senderEmail)) {
        cerr << "Error: The sender email '" << senderEmail << "' is not a valid format." << endl;
        cerr << "Please set it correctly in your custom.json file." << endl;
        return;
    }

    cout << "Email sending progress completed." << endl;
    cout << "Press return to continue..." << endl;
    ConsoleUtils::clearInputBuffer();
    cin.get();
}
