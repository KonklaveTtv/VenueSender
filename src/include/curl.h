#ifndef CURL_H
#define CURL_H

#include "errorhandler.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <mutex>

// Include headers for email processing
#include <curl/curl.h>

// Class to manage the cURL handle and associated operations
class CurlHandleWrapper {
public:
    // Deleted copy constructor and assignment operator
    CurlHandleWrapper(const CurlHandleWrapper&) = delete;
    CurlHandleWrapper& operator=(const CurlHandleWrapper&) = delete;

    // Static method to return an instance of the class
    static CurlHandleWrapper& getInstance() {
        static CurlHandleWrapper instance; // Guaranteed to be destroyed, instantiated on first use.
        return instance;
    }

    // Get the cURL handle for use in other operations
    CURL* get() const;

    // Initialize the cURL library (should be called at program start)
    static void init();

    // Clean up the cURL library (should be called at program end)
    static void cleanup();

    inline double getProgress() const {
        return progress;
    }

    // Callback function to update the progress of ongoing operations
    int progressCallback(void* /*clientp*/, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/);

    // Set the email address that is currently being sent
    void setEmailBeingSent(const std::string& email);

    // Get the email address that is currently being sent
    std::string getEmailBeingSent() const;

    // Clear the email address that was set
    void clearEmailBeingSent();

    // Set SSL options for the cURL handle
    void setSSLOptions(bool useSSL = true, bool verifyPeer = true, bool verifyHost = true);

    // Callback function for reading the email payload
    static size_t readCallback(void* ptr, size_t size, size_t nmemb, void* userp);

private:
    CurlHandleWrapper();  // Made private to enforce Singleton pattern
    ~CurlHandleWrapper(); // Destructor

    // The actual cURL handle
    CURL* curl;

    // Progress of the current cURL operation
    double progress{};

    // Email address of the recipient for the email being sent
    std::string emailBeingSent;
};

// Function to set up a cURL handle with the given options
CURL* setupCurlHandle(CurlHandleWrapper &curlWrapper, bool useSSL, bool verifyPeer, bool verifyHost, bool verbose,
                                        const std::string& senderEmail, const std::string& smtpUsername, std::string& mailPassDecrypted, int smtpPort, const std::string& smtpServer);

#endif // CURL_H
