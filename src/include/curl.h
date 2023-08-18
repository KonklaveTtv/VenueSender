#ifndef CURL_H
#define CURL_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <mutex>

#include <curl/curl.h>

class CurlHandleWrapper {
public:
    // Constructor
    CurlHandleWrapper();

    // Destructor
    ~CurlHandleWrapper();

    // Get the CURL handle
    CURL* get() const;

    // Initialize the cURL library. Should be called at the start of the program.
    static void init();

    // Clean up the cURL library. Should be called at the end of the program.
    static void cleanup();

    // Callback to update the progress of the current operation
    int progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/);

    // Set the email that is currently being sent
    void setEmailBeingSent(const std::string& email);

    // Get the email that is currently being sent
    std::string getEmailBeingSent() const;

    // Clear the email that was set
    void clearEmailBeingSent();

    // Set SSL options for the CURL handle
    void setSSLOptions(bool useSSL = true, bool verifyPeer = true, bool verifyHost = true);

    // Callback for reading the email payload
    static size_t readCallback(void* ptr, size_t size, size_t nmemb, void* userp);

private:
    // The cURL handle
    CURL* curl;

    // Current progress of the cURL operation
    double progress{};

    // Email address of the recipient for the email currently being sent
    std::string emailBeingSent;

    // Mutex for thread safety
    mutable std::mutex mtx;
};

// Set up the CURL handle with appropriate options
CURL* setupCurlHandle(CurlHandleWrapper &curlWrapper, bool useSSL, bool verifyPeer, bool verifyHost, bool verbose,
                                        const std::string& senderEmail, const std::string& smtpUsername, std::string& mailPassDecrypted, int smtpPort, const std::string& smtpServer);

// Check the result of a CURL operation and print an error message if necessary
bool checkCurlError(CURLcode res, const char* errorMessage);

#endif // CURL_H
