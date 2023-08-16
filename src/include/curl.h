#ifndef CURL_H
#define CURL_H

#include <iostream>

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

private:
    CURL* curl;
    double progress;
    std::string emailBeingSent;
};

CURL* setupCurlHandle(CurlHandleWrapper &curlWrapper, bool useSSL, bool verifyPeer, bool verifyHost,
                      const std::string& smtpServer, int smtpPort, const std::string& smtpUsername,
                      const std::string& senderEmail, const std::string& mailPassDecrypted);

bool checkCurlError(CURLcode res, const char* errorMessage);

#endif // CURL_H
