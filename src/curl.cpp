#include "curl.h"

using namespace std;

string emailBeingSent;

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

void curlGlobalInit() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void curlGlobalCleanup() {
    curl_global_cleanup();
}

void configureCurlForEmail(CurlHandleWrapper& curlWrapper, const std::string& smtpServer, int smtpPort, const std::string& smtpUsername, const std::string& smtpPassDecrypted) {
    CURL* curl = curlWrapper.get();
    // Set the URL for the SMTP server
    std::string smtpUrl = "smtp://" + smtpServer + ":" + std::to_string(smtpPort);
    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());

    // Set SMTP username and password
    std::string smtpUserPass = smtpUsername + ":" + smtpPassDecrypted;
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUserPass.c_str());

    // Set SSL options for secure connection
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); // Enable peer certificate verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L); // Check that the certificate's common name matches the host name
}