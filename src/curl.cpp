#include "include/curl.h"

// Use the standard namespace
using namespace std;

/* CurlHandleWrapper Class Implementation */
/*---------------------------------------*/

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
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        cin.clear();
        return;
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

// Function to set up a cURL handle with various settings
CURL* setupCurlHandle(CurlHandleWrapper &curlWrapper, bool useSSL, const string& sslCertPath, bool verifyPeer, bool verifyHost, bool verbose, 
                      const string& senderEmail, const string& smtpUsername, string& mailPass, int smtpPort, const string& smtpServer) {

    // Initialize the curlWrapper
    CURL* curl = curlWrapper.get();
    if (!curl) {
        // Handle error
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBCURL_ERROR);
        cin.clear();
        return nullptr;
    }

    // Set SSL certificate path
    if (!sslCertPath.empty()) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, sslCertPath.c_str());
    } else {
        // Do nothing and use cURL pre-compiled SSL certificates
    }

    // SMTP server configuration
    string smtpUrl = "smtp://" + smtpServer + ":" + to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // SMTP authentication
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUsername.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, mailPass.c_str());

    // Set the sender
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

    // Set SSL options using the CurlHandleWrapper method
    curlWrapper.setSSLOptions(useSSL, verifyPeer, verifyHost);

    // Overall timeout set to 120 seconds, connection timeout set to 60 seconds
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 60L);  // 60 seconds

    // Enable verbose mode for debugging in config.json (if needed)
    curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);

    // Clear and Empty the Email Password from memory
    fill(mailPass.begin(), mailPass.end(), '\0');
    mailPass.clear();

    return curl;
}
