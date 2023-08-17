#ifndef CURL_H
#define CURL_H

#include <cstring>
#include <iostream>
#include <mutex>

#include <curl/curl.h>

class CurlHandleWrapper {
public:
    CurlHandleWrapper();
    ~CurlHandleWrapper();
    CURL* get() const;
    static void init();
    static void cleanup();

    // Change visibility of progressCallback to public
    int progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/);

    void setEmailBeingSent(const std::string& email);
    std::string getEmailBeingSent() const;

    // Add a method to set SSL options
    void setSSLOptions(bool useSSL = true, bool verifyPeer = true, bool verifyHost = true);
    static size_t readCallback(void* ptr, size_t size, size_t nmemb, void* userp);
private:
    CURL* curl;
    double progress;
    std::string emailBeingSent;
    mutable std::mutex mtx;
};

CURL* setupCurlHandle(CurlHandleWrapper &curlWrapper, bool useSSL, bool verifyPeer, bool verifyHost,
                      const std::string& senderEmail, const std::string& smtpUsername,
                      const std::string& mailPassDecrypted, int smtpPort, const std::string& smtpServer);

bool checkCurlError(CURLcode res, const char* errorMessage);

#endif // CURL_H
