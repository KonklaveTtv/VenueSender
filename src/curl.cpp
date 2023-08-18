#include "curl.h"

using namespace std;

/* CurlHandleWrapper*/
/*-------------------*/
int CurlHandleWrapper::progressCallback(void* clientp, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/) {
    auto* instance = static_cast<CurlHandleWrapper*>(clientp);

    if (dltotal > 0) {
        instance->progress = (dlnow / dltotal) * 100;  // set the progress here
        if (instance->progress <= 100) {
            cout << "Email sending progress: " << instance->progress << "% (" << instance->getEmailBeingSent() << ")" << endl;
        }
    }
    return 0;
}

size_t CurlHandleWrapper::readCallback(void* ptr, size_t size, size_t nmemb, void* userp) {
    auto* payload = static_cast<std::string*>(userp);
    size_t totalsize = size * nmemb;

    if (!payload->empty()) {
        size_t toCopy = (totalsize < payload->size() ? totalsize : payload->size());
        memcpy(ptr, payload->c_str(), toCopy);
        payload->erase(0, toCopy);
        return toCopy;
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

CurlHandleWrapper::CurlHandleWrapper() : curl(nullptr) {
    curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize libcurl." << endl;
        return;
    }

    if (curl) {
        // Keeping these options as they're unique and not present in `setupCurlHandle`.
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CurlHandleWrapper::progressCallback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
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
    std::lock_guard<std::mutex> lock(mtx);
    emailBeingSent = email;
}

std::string CurlHandleWrapper::getEmailBeingSent() const {
    std::lock_guard<std::mutex> lock(mtx);
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
    
    // SMTP server configuration
    std::string smtpUrl = "smtp://" + smtpServer + ":" + std::to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // SMTP authentication
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUsername.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, mailPassDecrypted.c_str());

    // Set the sender
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());

    // Set SSL options using the CurlHandleWrapper method
    curlWrapper.setSSLOptions(useSSL, verifyPeer, verifyHost);

    // Enable verbose mode for debugging (if needed)
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    return curl;
}

bool checkCurlError(CURLcode res, const char* errorMessage) {
    if (res != CURLE_OK) {
        cerr << errorMessage << ": " << curl_easy_strerror(res) << endl;
        return false;
    }
    return true;
}
