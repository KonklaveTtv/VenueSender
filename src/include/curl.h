#ifndef CURL_H
#define CURL_H

#include <iostream>

#include <curl/curl.h>

extern std::string emailBeingSent;

class CurlHandleWrapper {
public:
    CurlHandleWrapper();
    ~CurlHandleWrapper();
    CURL* get() const;
    static void init();
    static void cleanup();

    // Change visibility of progressCallback to public
    int progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/);

private:
    CURL* curl;
};

void curlGlobalInit();
void curlGlobalCleanup();


#endif // CURL_H