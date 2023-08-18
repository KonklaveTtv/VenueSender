#include "mail.h"

using namespace std;

CurlHandleWrapper curlWrapper;

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

void EmailManager::viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost,
                       const string& senderEmail, int smtpPort, const string& smtpServer) {

#ifdef UNIT_TESTING
    cout << "===== Email Settings =====" << endl;
    cout << "SMTP Server: " << smtpServer << endl;
    cout << "SMTP Port: " << smtpPort << endl;
    cout << "Sender Email: " << senderEmail << endl;
    cout << "SSL: " << (useSSL ? "true" : "false") << endl;
    cout << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl;
    cout << "verifyHost: " << (verifyHost ? "true" : "false") << endl;
    cout << "===========================" << endl;
#else
    cout << "===== Email Settings =====" << endl;
    cout << "SMTP Server: " << smtpServer << endl;
    cout << "SMTP Port: " << smtpPort << endl;
    cout << "Sender Email: " << senderEmail << endl;
    cout << "SSL: " << (useSSL ? "true" : "false") << endl;
    cout << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl;
    cout << "verifyHost: " << (verifyHost ? "true" : "false") << endl;
    cout << "===========================" << endl;
    cout << "Press return to return to Main Menu" << endl;
    cin.ignore();  // If there's a chance you might have used cin before this point
    ConsoleUtils::clearInputBuffer();
    cin.get();     // This will wait for a key press
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
    cout << "===== Construct Email =====" << endl;
        // Prompt user to enter email subject and message
    do {
        cout << "Enter subject for the email: ";
        getline(in, subject);
        subject = sanitizeSubject(subject);  // Sanitize the subject
    } while (subject.empty());

    const int maxSubjectLength = EmailManager::MAX_SUBJECT_LENGTH;
    const int maxMessageLength = EmailManager::MAX_EMAIL_LENGTH;

    if (subject.length() > maxSubjectLength) {
        cout << "Subject too long. Please try again." << endl;
        subject.clear(); // Clear the subject if it's too long.
        return;
    }
    do {
        cout << "Enter the message for the email (press Enter on a new line to finish):\n";
        string line;
        while (getline(in, line) && !line.empty()) {
            if (message.length() + line.length() > maxMessageLength) {
                cout << "Message too long. Truncating to maximum length." << endl;
                int charsToAdd = maxMessageLength - message.length();
                message += ConsoleUtils::trim(line).substr(0, charsToAdd); // Add as many characters as possible
                break;
            }
            message += ConsoleUtils::trim(line) + "\n";
        }
        cout << "============================" << endl;
        if (message.empty()) {
            cout << "Message cannot be blank. Please enter a message." << endl;
        }
    } while (message.empty());

    size_t pos = 0;
    while ((pos = message.find("\n.\n", pos)) != string::npos) {
    message.replace(pos + 1, 1, "..");
    pos += 3;
}
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
    curlWrapper.setEmailBeingSent(selectedVenue.email);

    // Set up and send an email using libcurl
    if (!curl) {
        cerr << "Failed to initialize libcurl." << endl;
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
        }
        return false;
    }

    return true;
}

void EmailManager::viewEmailSendingProgress(CURL* curl, const vector<SelectedVenue>& selectedVenuesForEmail,
                              const string& senderEmail,
                              string& subject,
                              string& message,
                              const string& smtpServer,
                              int smtpPort) {
    if(!isValidEmail(senderEmail)) {
        cerr << "Error: The sender email '" << senderEmail << "'is not a valid format." << endl;
        cerr << "Please set it correctly in your custom.json file." << endl;
        return;
}
    for (size_t i = 0; i < selectedVenuesForEmail.size(); ++i) {
        const SelectedVenue& venue = selectedVenuesForEmail[i];
        curlWrapper.setEmailBeingSent(venue.email); // Set the value of emailBeingSent
        cout << "Sending email " << (i + 1) << " of " << selectedVenuesForEmail.size() << " to: " << venue.email << endl;

        // Send the individual email with progress tracking
        sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort);

        curlWrapper.clearEmailBeingSent();
    }

    cout << "Email sending progress completed." << endl;
}
