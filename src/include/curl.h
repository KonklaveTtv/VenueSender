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
    std::string emailBeingSent;
};

#endif // CURL_H
