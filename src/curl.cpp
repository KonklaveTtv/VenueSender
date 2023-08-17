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

void CurlHandleWrapper::setSSLOptions(bool useSSL, bool verifyPeer, bool verifyHost) {
    if (useSSL) {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    } else {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_NONE);  // no SSL
    }

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verifyPeer ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verifyHost ? 2L : 0L);
}

CurlHandleWrapper::CurlHandleWrapper() {
    curl = curl_easy_init();
    if (curl) {
        // Keeping these options as they're unique and not present in `setupCurlHandle`.
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        // Set up location of SSL certs for Linux
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");
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
                      const string& senderEmail, const string& smtpUsername,
                      const string& mailPassDecrypted, int smtpPort, const string& smtpServer) {

    CURL* curl = curlWrapper.get();
    if (!curl) {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return nullptr;
    }
    
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // Set the SSL options using the CurlHandleWrapper method
    curlWrapper.setSSLOptions(useSSL, verifyPeer, verifyHost);

    // SMTP server configuration
    std::string smtpUrl = "smtp://" + smtpServer + ":" + std::to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // SMTP authentication
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUsername.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, mailPassDecrypted.c_str());

    // Set the sender
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

    return curl;
}

bool checkCurlError(CURLcode res, const char* errorMessage) {
    if (res != CURLE_OK) {
        cerr << errorMessage << ": " << curl_easy_strerror(res) << endl;
        return false;
    }
    return true;
}
