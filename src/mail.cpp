#include "mail.h"

string emailBeingSent;

/* CurlHandleWrapper*/
/*-------------------*/
int CurlHandleWrapper::progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/) {
    // You can calculate the progress percentage based on the parameters provided
    // and provide updates to the user here
    // Example: Print the progress every 10% completion
    if (dltotal > 0) {
        double progress = (dlnow / dltotal) * 100;
        if (progress >= 5) {
            cout << "Email sending progress: " << progress << "% (" << emailBeingSent << ")" << endl;
        }
    }
    return 0;
}

CurlHandleWrapper::CurlHandleWrapper() {
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress callback
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);

       // Set SSL/TLS options
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL); // Use SSL/TLS
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); // Verify the peer's certificate
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L); // Verify the host's certificate
    }
}

CurlHandleWrapper::~CurlHandleWrapper() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

CURL* CurlHandleWrapper::get() const {
    return curl;
}

void CurlHandleWrapper::init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void CurlHandleWrapper::cleanup() {
    curl_global_cleanup();
}
/*-------------------*/

// Function to check if an email address is in a valid format
bool isValidEmail(const string& email) {
    // A simple regex pattern to check the format of the email
    static const regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
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
void constructEmail(string &subject, string &message, istream &in = cin) {
    cout << "===== Construct Email =====" << endl;

    // Clear any remaining characters in the input buffer (including newline)
    clearInputBuffer();

    // Prompt user to enter email subject and message
    cout << "Enter subject for the email: ";
    getline(in, subject);

    const int maxSubjectLength = MAX_SUBJECT_LENGTH;
    const int maxMessageLength = MAX_MESSAGE_LENGTH;

    if (subject.length() > maxSubjectLength) {
        cout << "Subject too long. Please try again." << endl;
        subject.clear(); // Clear the subject if it's too long.
        return;
    }

    cout << "Enter the message for the email (press Enter on a new line to finish):\n";
    string line;
    while (getline(in, line) && !line.empty()) {
        if (message.length() + line.length() > maxMessageLength) {
            cout << "Message too long. Truncating to maximum length." << endl;
            int charsToAdd = maxMessageLength - message.length();
            message += trim(line).substr(0, charsToAdd); // Add as many characters as possible
            break;
        }
        message += trim(line) + "\n";
    }
    cout << "============================" << endl;
}

// Function to send an individual email to a recipient with custom subject and message using libcurl
bool sendIndividualEmail(CURL* curl,
                        const SelectedVenue& selectedVenue,
                        const string& senderEmail,
                        const string& subject,
                        const string& message,
                        const string& smtpServer,
                        int smtpPort,
                        const string& smtpUsername,
                        const string& smtpPassDecrypted,
                        double& progress) {
    // Set the value of emailBeingSent
    emailBeingSent = selectedVenue.email;

    // Set up and send an email using libcurl
    if (!curl) {
        cerr << "Failed to initialize libcurl." << endl;
        return false;
    }

    cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << endl;

    // Set the recipient email address
    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, selectedVenue.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    // Set the email sender
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

    // Set the email subject
    string subjectHeader = "Subject: " + subject;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, subjectHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the email body (message)
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(message.length()));

    // Set the URL for the SMTP server
    string smtpUrl = "smtp://" + smtpServer + ":" + to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // Set SMTP username and password
    string smtpUserPass = smtpUsername + ":" + smtpPassDecrypted;
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUserPass.c_str());

    cout << "Authenticating with SMTP server..." << endl;

    // Set SSL options for secure connection
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); // Enable peer certificate verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L); // Check that the certificate's common name matches the host name

    // Set the progress callback function
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    // Perform the email sending
    CURLcode res = curl_easy_perform(curl);

    // Clean up and return the result
    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        cerr << "Failed to send email: " << curl_easy_strerror(res) << endl;
        if (res == CURLE_COULDNT_CONNECT) {
            cerr << "Connection to SMTP server failed." << endl;
        } else if (res == CURLE_LOGIN_DENIED) {
            cerr << "Authentication with SMTP server failed." << endl;
        }
        return false;
    }

    return true;
}

void viewEmailSendingProgress(CURL* curl, const vector<SelectedVenue>& selectedVenuesForEmail,
                              const string& senderEmail,
                              const string& subject,
                              const string& message,
                              const string& smtpServer,
                              int smtpPort,
                              const string& smtpUsername,
                              const string& smtpPassDecrypted,
                              double& progress) {
    // Set the custom progress callback function from CurlHandleWrapper
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);

    for (size_t i = 0; i < selectedVenuesForEmail.size(); ++i) {
        const SelectedVenue& venue = selectedVenuesForEmail[i];
        emailBeingSent = venue.email; // Set the value of emailBeingSent
        cout << "Sending email " << (i + 1) << " of " << selectedVenuesForEmail.size() << " to: " << venue.email << endl;

        // Send the individual email with progress tracking
        sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, smtpUsername, smtpPassDecrypted, progress);

        emailBeingSent.clear(); // Reset the value of emailBeingSent
    }
    cout << "Email sending progress completed." << endl;
}
