#include "mail.h"

#include <cstdio>

using namespace std;

CurlHandleWrapper curlHandleWrapper;
ErrorHandler errorHandler;

vector<char> EmailManager::readFile(const string& filePath, string& attachmentName, string& attachmentSize) {
    FILE* file = fopen(filePath.c_str(), "rb"); // Open the file in binary mode
    if (!file) {
        throw runtime_error("Failed to open file: " + filePath);
    }

    fseek(file, 0, SEEK_END); // Move the file pointer to the end
    long fileSize = ftell(file); // Get the file size
    fseek(file, 0, SEEK_SET); // Move the file pointer back to the beginning

    vector<char> buffer(fileSize);
    size_t bytesRead = fread(buffer.data(), 1, fileSize, file); // Read the file content
    if (bytesRead != static_cast<size_t>(fileSize)) {
        fclose(file);
        throw runtime_error("Failed to read file: " + filePath);
    }

    fclose(file);

    // Get the file name from the file path
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash == string::npos) {
        attachmentName = filePath;
    } else {
        attachmentName = filePath.substr(lastSlash + 1);
    }

    // Get the file size as a string
    attachmentSize = to_string(fileSize);

    return buffer;
}

string EmailManager::base64Encode(const vector<char>& data) {
    if (data.empty()){
        return "";
    }

    size_t outputMaxLen = sodium_base64_ENCODED_LEN(data.size(), sodium_base64_VARIANT_ORIGINAL);

    char output[outputMaxLen];

    if (sodium_bin2base64(output, outputMaxLen, reinterpret_cast<const unsigned char*>(&data[0]), data.size(), sodium_base64_VARIANT_ORIGINAL) == NULL) {
        return "";
    }

    return string(output);
}

// Callback function for cURL's MIME encoding with base64
size_t base64EncodeCallback(void *contents, size_t size, size_t nmemb, void *userdata) {
    // Calculate the total size of the data to be encoded
    size_t totalSize = size * nmemb;
    
    // Cast the userdata pointer to EmailManager
    EmailManager *emailManager = static_cast<EmailManager *>(userdata);

    // Encode the data using the base64Encode method
    std::string encodedData = emailManager->base64Encode(std::vector<char>(static_cast<char *>(contents), static_cast<char *>(contents) + totalSize));

    // Copy the encoded data back to the 'contents' buffer
    memcpy(contents, encodedData.c_str(), encodedData.length());

    // Return the length of the encoded data
    return encodedData.length();
}

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
    errorHandler.showInfoAndReturn();
#endif
}

// Function to check if an email address is in a valid format
bool EmailManager::isValidEmail(const string& email) {
    // A simple regex pattern to check the format of the email
    static const regex emailPattern(R"((?=.{1,256})(?=.{1,64}@.{1,255})[^\s@]+@[^\s@]+\.[^\s@]+)");
    return regex_match(email, emailPattern);
}

/**
 * Constructs an email by prompting the user for the subject and message.
 * If the provided subject length exceeds the maximum allowed length, 
 * the function will inform the user, clear the subject, and return.
 * 
 * @param subject - Reference to the email subject string
 * @param message - Reference to the email message string
 * @param in - Input stream, defaulted to standard input (cin)
 */
void EmailManager::constructEmail(string &subject, string &message, istream &in = cin) {
    cout << "===========================" << endl;
    cout << "===== Construct Email =====" << endl;
    cout << "===========================" << endl;

    // Prompt user to enter email subject and message
    do {
        cout << "Enter subject for the email (press Enter on a new line to finish): ";
        string line;
        while (getline(in, line)) {
            if (line.empty()) break; // exit on a blank line
            subject += sanitizeSubject(line) + " ";
        }
        subject = ConsoleUtils::trim(subject); // To remove trailing space
    } while (subject.empty());

    const std::string::size_type maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const std::string::size_type maxMessageLength = EmailManager::MAX_MESSAGE_LENGTH;

    if (subject.length() > maxSubjectLength) {
        cout << "Subject cannot be longer than 50 characters in length." << endl;
#ifndef UNIT_TESTING
        cout << "Press return to go back..." << endl;
        cin.ignore();  // If there's a chance you might have used cin before this point
        ConsoleUtils::clearInputBuffer();
        cin.get();     // This will wait for a key press
#endif
        subject.clear(); // Clear the subject if it's too long.
        return;
    }

    bool inputProvided;  // Declare outside of the loop
    do {
        cout << "Enter the message for the email (press Enter on a new line to finish):\n";
        string line;
        inputProvided = false;  // Initialize to false at the start of each loop iteration

        while (getline(in, line)) {
            if (line.empty()) break;
            if (message.length() + line.length() > maxMessageLength) {
                cout << "Message cannot be longer than 2000 characters in length." << endl;
#ifndef UNIT_TESTING
                cout << "Press return to go back..." << endl;
                cin.ignore();  // If there's a chance you might have used cin before this point
                ConsoleUtils::clearInputBuffer();
                cin.get();     // This will wait for a key press     
#endif  
                int charsToAdd = maxMessageLength - message.length();
                message += ConsoleUtils::trim(line).substr(0, charsToAdd); // Add as many characters as possible
                break;
            }
            message += ConsoleUtils::trim(line) + "\n";
        }

        if (in.eof()) {
            inputProvided = true;  // Set to true to indicate that input has been provided
        } else if (message.empty()) {
            cout << "Message cannot be blank." << endl;
#ifndef UNIT_TESTING
            cout << "Press return to go back..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press     
#endif
        }

        if (!inputProvided) {  // If no input is provided, exit the loop
            break;
        }

    } while (!inputProvided);

    size_t pos = 0;
    while ((pos = message.find("\n.\n", pos)) != string::npos) {
        message.replace(pos + 1, 1, "..");
        pos += 3;
    }

    // add attachment
}

// Function to send an individual email to a recipient with custom subject and message using libcurl
bool EmailManager::sendIndividualEmail(CURL* curl,
                        const SelectedVenue& selectedVenue,
                        const string& senderEmail,
                        string& subject,
                        string& message,
                        const string& smtpServer,
                        int smtpPort) {


    // Set the value of emailBeingSent
    curlHandleWrapper.setEmailBeingSent(selectedVenue.email);

    // Set up and send an email using libcurl
    if (!curl) {
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return false;
    }

    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;
    this_thread::sleep_for(chrono::milliseconds(1000));

    // Set the recipient email address
    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, selectedVenue.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    
    const int maxSubjectHeaderLength = 68; // 78 - "Subject: ".length()
    if (subject.length() > maxSubjectHeaderLength) {
        subject = subject.substr(0, maxSubjectHeaderLength - 3) + "...";
    }

    // Construct headers for the email content
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

    // amend header for attachments
    // Set the email body (message) with the desired formatting
    string payload = dateHeader + "\r\n" + 
                 toHeader + "\r\n" + 
                 fromHeader + "\r\n" + 
                 subjectHeader + "\r\n\r\n" + 
                 message; 
    size_t pos = 0;
    while((pos = payload.find('\n', pos)) != string::npos) {
        payload.replace(pos, 1, "\r\n");
        pos += 2;  // Skip the newly added characters to avoid infinite loop
    }
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlHandleWrapper::readCallback); // Set custom read function
    curl_easy_setopt(curl, CURLOPT_READDATA, &payload); // Pass the message string to the read function
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(payload.length()));

    cout << "Authenticating with SMTP server..." << endl;

    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // Perform the email sending
    CURLcode res = curl_easy_perform(curl);

    // Clean up and return the result
    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);

    if (!checkCurlError(res, "Failed to send email")) {
        if (res == CURLE_COULDNT_CONNECT) {
            cerr << "Connection to SMTP server failed." << endl;
        } else if (res == CURLE_LOGIN_DENIED) {
            cerr << "Authentication with SMTP server failed." << endl;
                cout << "Email sending progress completed." << endl;
                cout << "Press return to continue..." << endl;
                ConsoleUtils::clearInputBuffer();
                cin.get();     // This will wait for a key press     
        }
        return false;
    }

    return true;
}

void EmailManager::viewEmailSendingProgress(const string& senderEmail) {
    if(!isValidEmail(senderEmail)) {
        cerr << "Error: The sender email '" << senderEmail << "'is not a valid format." << endl;
        cerr << "Please set it correctly in your custom.json file." << endl;
        return;
}

    cout << "Email sending progress completed." << endl;
    cout << "Press return to continue..." << endl;
    ConsoleUtils::clearInputBuffer();
    cin.get();     // This will wait for a key press     
}
