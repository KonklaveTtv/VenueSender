#ifndef MAIL_H
#define MAIL_H

#include "fileutils.h"

#include <chrono>
#include <ctime>
#include <regex>
#include <thread>

class EmailManager {
public:
// Define enums for return codes
enum class ReturnCode {
    Success,
    ConfigLoadError,
    EmailDecryptionError,
    SmtpPassDecryptionError
};

// Constants
static const int MAX_EMAIL_LENGTH = 500; // Maximum length for an email address, adjust as needed
static const int MAX_SUBJECT_LENGTH = 50; // Maximum length for an email address, adjust as needed

std::string getCurrentDateRfc2822();

std::string sanitizeSubject(std::string& subject);

// Function to display the email settings set in config.json to the user
void viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost, 
                       const std::string& senderEmail, int smtpPort, const std::string& smtpServer);

// Check if an email address is in a valid format
bool isValidEmail(const std::string& email);

// Function to construct an email by providing subject and message
void constructEmail(std::string &subject, std::string &message, std::istream &in);

// Function to send an individual email to a recipient with custom subject and message
bool sendIndividualEmail(CURL* curl,
                        const SelectedVenue& selectedVenue,
                        const std::string& senderEmail,
                        std::string& subject,
                        std::string& message,
                        const std::string& smtpServer,
                        int smtpPort);

// Function to view the progress of email sending done by sendIndividualEmail()
void viewEmailSendingProgress(CURL* curl, const std::vector<SelectedVenue>& selectedVenuesForEmail,
                              const std::string& senderEmail,
                              std::string& subject,
                              std::string& message,
                              const std::string& smtpServer,
                              int smtpPort);
};
#endif // MAIL_H
