#include "include/curl.h"

// Use the standard namespace
using namespace std;

#include "include/fileutils.h" // Forward declaration due to circular dependency between fileutils.h and errorhandler.h

/* CurlHandleWrapper Class Implementation */
/*---------------------------------------*/

// Progress callback to show progress in percentage for email sending
int CurlHandleWrapper::progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/) {
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
    if (dltotal > 0) {
        cout << "Callback called: dltotal=" << dltotal << ", dlnow=" << dlnow << endl;
        progress = (dlnow / dltotal) * 100;
        if (progress <= 100) {
            cout << "Email sending progress: " << progress << "% (" << emailBeingSent << ")" << endl;
            
        }
    }
    ConsoleUtils::resetColor();
    return 0;
}

// Callback function to read data for sending in the request
size_t CurlHandleWrapper::readCallback(void* ptr, size_t size, size_t nmemb, void* userp) {
    string* payload = static_cast<string*>(userp);
    size_t totalsize = size * nmemb;

    if (!payload->empty()) {
        // Calculate the size of data to copy to ptr
        size_t toCopy = (totalsize < payload->size() ? totalsize : payload->size());

        memcpy(ptr, payload->c_str(), toCopy); // Copy data to ptr
        payload->erase(0, toCopy); // Remove the portion that has been read from the payload

        return toCopy;
    }

    return 0; // Return 0 to signify no data left to read
}

// Function to set SSL options for cURL
void CurlHandleWrapper::setSSLOptions(bool useSSL, bool verifyPeer, bool verifyHost) {
    if (useSSL) {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    } else {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY); // try or revert to no SSL
    }

    // Set within the config.json and mock_config.json
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verifyPeer ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verifyHost ? 2L : 0L);
}

// Constructor for CurlHandleWrapper class
CurlHandleWrapper::CurlHandleWrapper() : curl(nullptr) {
    // Initialize cURL handle and set basic options
    curl = curl_easy_init();
    if (!curl) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return;
    }

    if (curl) {
        // Keeping these options as they're unique and not present in `setupCurlHandle`.
        //curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, &CurlHandleWrapper::progressCallback);

        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    }
}

// Destructor for CurlHandleWrapper class
CurlHandleWrapper::~CurlHandleWrapper() {
    // Clean up cURL handle
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

// Function to get the underlying cURL handle
CURL* CurlHandleWrapper::get() const {
    return curl;
}

// Global cURL initialization
void CurlHandleWrapper::init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

// Global cURL cleanup
void CurlHandleWrapper::cleanup() {
    curl_global_cleanup();
}

// Setter for emailBeingSent
void CurlHandleWrapper::setEmailBeingSent(const string& email) {
    emailBeingSent = email;
}

// Getter for emailBeingSent
string CurlHandleWrapper::getEmailBeingSent() const {
    return emailBeingSent;
}

// Function to clear the email being sent
void CurlHandleWrapper::clearEmailBeingSent() {
    emailBeingSent.clear();
}

// Function to set up a cURL handle with various settings
CURL* setupCurlHandle(CurlHandleWrapper &curlWrapper, bool useSSL, bool verifyPeer, bool verifyHost, bool verbose, 
                      const string& senderEmail, const string& smtpUsername, string& mailPassDecrypted, int smtpPort, const string& smtpServer) {

    // Initialize the curlWrapper
    CURL* curl = curlWrapper.get();
    if (!curl) {
        // Handle error
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        return nullptr;
    }
    
    // SMTP server configuration
    string smtpUrl = "smtp://" + smtpServer + ":" + to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // SMTP authentication
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUsername.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, mailPassDecrypted.c_str());

    // Let's clear the memory for the mailPassDecrypted here after CURLOPT_PASSWORD is set
    fill(mailPassDecrypted.begin(), mailPassDecrypted.end(), '\0');
    mailPassDecrypted.clear();

    // Set the sender
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

    // Set SSL options using the CurlHandleWrapper method
    curlWrapper.setSSLOptions(useSSL, verifyPeer, verifyHost);

    // Increase timeout for larger attachments
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    // Enable verbose mode for debugging in config.json (if needed)
    curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);

    return curl;
}
