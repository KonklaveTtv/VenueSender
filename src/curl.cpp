#include "curl.h"

using namespace std;

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

// Setter for emailBeingSent
void CurlHandleWrapper::setEmailBeingSent(const std::string& email) {
    emailBeingSent = email;
}

// Getter for emailBeingSent
std::string CurlHandleWrapper::getEmailBeingSent() const {
    return emailBeingSent;
}

CURL* setupCurlHandle(CurlHandleWrapper &curlWrapper, bool useSSL, bool verifyPeer, bool verifyHost,
                      const string& smtpServer, int smtpPort, const string& senderEmail, 
                      const string& mailPassDecrypted) {

    CURL* curl = curlWrapper.get();
    
    // Handle libcurl errors and display enhanced error messages
    if (!curl) {
        cerr << "Failed to initialize libcurl easy handle." << endl;
        return nullptr;
    }

    // Set SSL to True or False
    CURLcode res;
    res = curl_easy_setopt(curl, CURLOPT_USE_SSL, useSSL);
    if (res != CURLE_OK) {
        cerr << "Failed to set useSSL option." << endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }

    // Set verifyPeer to True or False
    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verifyPeer);
    if (res != CURLE_OK) {
        cerr << "Failed to set verifyPeer option." << endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }

    // Set verifyHost to True or False
    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verifyHost);
    if (res != CURLE_OK) {
        cerr << "Failed to set verifyHost option." << endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }

    // Connect to the SMTP server
    string smtpUrl = "smtp://" + smtpServer + ":" + to_string(smtpPort);
    res = curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());
    if (res != CURLE_OK) {
        cerr << "Failed to set libcurl URL option." << endl;
        return nullptr;
    }

    // Set the sender email address
    res = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());
    if (res != CURLE_OK) {
        cerr << "Failed to set libcurl sender email option." << endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }

    // Set the email password for authentication
    res = curl_easy_setopt(curl, CURLOPT_PASSWORD, mailPassDecrypted.c_str());
    if (res != CURLE_OK) {
        cerr << "Failed to set libcurl email password option." << endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }

    return curl;
}