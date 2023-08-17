#ifndef MAIL_H
#define MAIL_H

#include "fileutils.h"

#include <chrono>
#include <regex>
#include <thread>

// Define enums for return codes
enum class ReturnCode {
    Success,
    ConfigLoadError,
    EmailDecryptionError,
    SmtpPassDecryptionError
};

// Constants
const int MAX_EMAIL_LENGTH = 500; // Maximum length for an email address, adjust as needed
const int MAX_SUBJECT_LENGTH = 50; // Maximum length for an email address, adjust as needed

// Custom read callback function to read from a string
size_t readCallback(void* ptr, size_t size, size_t nmemb, void* userp);

// Check if an email address is in a valid format
bool isValidEmail(const std::string& email);

// Function to construct an email by providing subject and message
void constructEmail(std::string &subject, std::string &message, std::istream &in);

// Function to send an individual email to a recipient with custom subject and message
bool sendIndividualEmail(CURL* curl,
                        const SelectedVenue& selectedVenue,
                        const std::string& senderEmail,
                        const std::string& subject,
                        const std::string& message,
                        const std::string& smtpServer,
                        int smtpPort);

// Function to view the progress of email sending done by sendIndividualEmail()
void viewEmailSendingProgress(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail,
                              const std::string& senderEmail,
                              const std::string& subject,
                              const std::string& message,
                              const std::string& smtpServer,
                              int smtpPort);

#endif // MAIL_H
