#ifndef MAIL_H
#define MAIL_H

#include "curl.h"
#include "fileutils.h"
#include "menu.h"
#include "errorhandler.h"
#include "structs.h"

#include <ctime>
#include <filesystem>
#include <regex>
#include <thread>

// Class responsible for managing email-related operations
class EmailManager {
public:
// Maximum length for email message body
static inline const int MAX_MESSAGE_LENGTH = 2000;

// Maximum length for email subject
static inline const int MAX_SUBJECT_LENGTH = 50;

// Maximum attachment size (24 MB)
const size_t MAX_ATTACHMENT_SIZE = 24 * 1024 * 1024;  // 24 MB in bytes

// Function to get the current date in RFC 2822 format
std::string getCurrentDateRfc2822();

// Function to sanitize email subject (e.g., remove special characters)
std::string sanitizeSubject(std::string& subject);

// Function to display the email settings from the configuration file
void viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost, bool verbose,  
                       const std::string& senderEmail, int smtpPort, const std::string& smtpServer);

// Function to validate an email address format
bool isValidEmail(const std::string& email);

// Function to construct an email, including the subject, message, and attachment details
void constructEmail(std::string &subject, std::string &message, std::string &attachmentPath, std::string &attachmentName, std::string &attachmentSize, std::istream &in = std::cin);

// Function to send an individual email to a selected venue
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

// Function to display the progress of email sending
void viewEmailSendingProgress(const std::string& senderEmail);
};

#endif // MAIL_H
