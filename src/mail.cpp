#include "mail.h"

std::string emailBeingSent;

/* CurlHandleWrapper*/
/*-------------------*/
int CurlHandleWrapper::progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/) {
    // You can calculate the progress percentage based on the parameters provided
    // and provide updates to the user here
    // Example: Print the progress every 10% completion
    if (dltotal > 0) {
        double progress = (dlnow / dltotal) * 100;
        if (progress >= 10) {
            std::cout << "Email sending progress: " << progress << "% (" << emailBeingSent << ")" << std::endl;
        }
    }
    return 0;
}

CurlHandleWrapper::CurlHandleWrapper() {
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress callback
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
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
bool isValidEmail(const std::string& email) {
    // A simple regex pattern to check the format of the email
    static const std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailPattern);
}

// Function to construct an email by providing subject and message
void constructEmail(std::string &subject, std::string &message, std::istream &in = std::cin) {
    std::cout << "===== Construct Email =====" << std::endl;

    // Clear any remaining characters in the input buffer (including newline)
    clearInputBuffer();

    // Prompt user to enter email subject and message
    std::cout << "Enter subject for the email: ";
    std::getline(in, subject);

    const int maxSubjectLength = MAX_SUBJECT_LENGTH;
    const int maxMessageLength = MAX_MESSAGE_LENGTH;

    if (subject.length() > maxSubjectLength) {
        std::cout << "Subject too long. Please try again." << std::endl;
        subject.clear(); // Clear the subject if it's too long.
        return;
    }

    std::cout << "Enter the message for the email (press Enter on a new line to finish):\n";
    std::string line;
    while (std::getline(in, line) && !line.empty()) {
        if (message.length() + line.length() > maxMessageLength) {
            std::cout << "Message too long. Truncating to maximum length." << std::endl;
            int charsToAdd = maxMessageLength - message.length();
            message += trim(line).substr(0, charsToAdd); // Add as many characters as possible
            break;
        }
        message += trim(line) + "\n";
    }
    std::cout << "============================" << std::endl;
}

// Function to send an individual email to a recipient with custom subject and message using libcurl
bool sendIndividualEmail(CURL* curl,
                        const SelectedVenue& selectedVenue,
                        const std::string& senderEmail,
                        const std::string& subject,
                        const std::string& message,
                        const std::string& smtpServer,
                        int smtpPort,
                        const std::string& smtpUsername,
                        const std::string& smtpPassDecrypted,
                        double& progress) {
    // Set the value of emailBeingSent
    emailBeingSent = selectedVenue.email;

    // Set up and send an email using libcurl
    if (!curl) {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return false;
    }

    std::cout << "Connecting to SMTP server: " << smtpServer << ":" << smtpPort << std::endl;

    // Set the recipient email address
    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, selectedVenue.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    // Set the email sender
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

    // Set the email subject
    std::string subjectHeader = "Subject: " + subject;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, subjectHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the email body (message)
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(message.length()));

    // Set the URL for the SMTP server
    std::string smtpUrl = "smtp://" + smtpServer + ":" + std::to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // Set SMTP username and password
    std::string smtpUserPass = smtpUsername + ":" + smtpPassDecrypted;
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUserPass.c_str());

    std::cout << "Authenticating with SMTP server..." << std::endl;

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
        std::cerr << "Failed to send email: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    return true;
}

void viewEmailSendingProgress(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail,
                              const std::string& senderEmail,
                              const std::string& subject,
                              const std::string& message,
                              const std::string& smtpServer,
                              int smtpPort,
                              const std::string& smtpUsername,
                              const std::string& smtpPassDecrypted) {
    // Set the custom progress callback function from CurlHandleWrapper
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);

    double progress = 0.0; // Initialize progress

    for (size_t i = 0; i < selectedVenuesForEmail.size(); ++i) {
        const SelectedVenue& venue = selectedVenuesForEmail[i];
        emailBeingSent = venue.email; // Set the value of emailBeingSent
        std::cout << "Sending email " << (i + 1) << " of " << selectedVenuesForEmail.size() << " to: " << venue.email << std::endl;

        // Send the individual email with progress tracking
        sendIndividualEmail(curl, venue, senderEmail, subject, message, smtpServer, smtpPort, smtpUsername, smtpPassDecrypted, progress);

        emailBeingSent.clear(); // Reset the value of emailBeingSent
    }
    std::cout << "Email sending progress completed." << std::endl;
}
