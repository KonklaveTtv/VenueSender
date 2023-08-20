#ifndef MAIL_H
#define MAIL_H

#include "curl.h"
#include "fileutils.h"
#include "menu.h"
#include "errorhandler.h"
#include "structs.h"

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
static inline const int MAX_MESSAGE_LENGTH = 2000;
static inline const int MAX_SUBJECT_LENGTH = 50;

std::string getCurrentDateRfc2822();
std::string sanitizeSubject(std::string& subject);

// Function to display the email settings set in config.json to the user
void viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost, bool verbose,  
                       const std::string& senderEmail, int smtpPort, const std::string& smtpServer);

// Check if an email address is in a valid format
bool isValidEmail(const std::string& email);

// Function to construct an email by providing subject and message
void constructEmail(std::string &subject, std::string &message, std::string &attachmentPath, std::string &attachmentName, std::string &attachmentSize, std::istream &in = std::cin);

// Function to send an individual email to a recipient with custom subject and message
bool sendIndividualEmail(CURL* curl,
                         const SelectedVenue& selectedVenue,
                         const std::string& senderEmail,
                         std::string& subject,
                         std::string& message,
                         const std::string& smtpServer,
                         int smtpPort,
                         std::string& attachmentName,
                         std::string& attachmentSize,
                         const std::string& attachmentPath);

// Function to view the progress of email sending done by sendIndividualEmail()
void viewEmailSendingProgress(const std::string& senderEmail);
};

#endif // MAIL_H
