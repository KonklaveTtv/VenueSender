#ifndef MAIL_H
#define MAIL_H

#include "curl.h"
#include "fileutils.h"
#include "errorhandler.h"
#include "messagehandler.h"
#include "menutitlehandler.h"
#include "structs.h"

#include <ctime>
#include <cctype>
#include <filesystem>
#include <limits>
#include <regex>
#include <thread>
#include <unordered_set>

// Boost Libraries
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/regex.hpp>

// Class responsible for managing email-related operations
class EmailManager {
private:
    // Map to store email addresses we have sent to this session
    static std::unordered_set<std::string> sentEmailAddressesForEmails;
    static std::unordered_set<std::string> sentEmailAddressesForTemplates;

public:
    // SSL/TLS SMTP Port
    static inline const int SSL_TLS_SMTP_PORT = 465;

    // Default SMTP Port
    static inline const int DEFAULT_SMTP_PORT = 587;

    // Maximum length for email message body
    static inline const int MAX_MESSAGE_LENGTH = 2000;

    // Maximum length for email subject
    static inline const int MAX_SUBJECT_LENGTH = 50;

    // Maximum length for smtp server name
    static inline const int MIN_SMTP_SERVER_LENGTH = 1;

    // Maximum length for smtp server name
    static inline const int MAX_SMTP_SERVER_LENGTH = 256;

    // Reset the email send counter
    static inline const int RESET_SEND_COUNT_TO_ZERO = 0;

    // Total number of emails being sent via emailCustomAddress()
    static inline const int CUSTOM_EMAIL_TO_SEND_COUNT = 1; // Only ever sends 1 email

    // Maximum attachment size (24 MB)
    const size_t MAX_ATTACHMENT_SIZE = 24 * 1024 * 1024;  // 24 MB in bytes

    // Function to get the current date in RFC 2822 format
    static inline std::string getCurrentDateRfc2822() {
        char buffer[100];
        time_t now;
        struct tm *tm_now;
        time(&now);
        tm_now = gmtime(&now);
        strftime(buffer, sizeof buffer, "%a, %d %b %Y %H:%M:%S %Z", tm_now);
        return buffer;
    }

    // Function to sanitize the subject line of an email by replacing newline and carriage return characters with spaces
    static inline std::string sanitizeSubject(std::string& subject) {
        std::string sanitized = subject;
        replace(sanitized.begin(), sanitized.end(), '\n', ' '); // replace newlines with spaces
        replace(sanitized.begin(), sanitized.end(), '\r', ' '); // replace carriage returns with spaces
        return sanitized;
    }

    static inline void clearAllEmailData(std::string& subject, std::string& message, std::string& attachmentName, std::string& attachmentSize, std::string& attachmentPath) {
        subject.clear();
        message.clear();
        attachmentName.clear();
        attachmentSize.clear();
        attachmentPath.clear();
    }

    static inline void clearAllCustomAddressEmailData(std::string& customAddressSubject, std::string& customAddressMessage, std::string& customAddressAttachmentName, std::string& customAddressAttachmentSize, std::string& customAddressAttachmentPath) {
        customAddressSubject.clear();
        customAddressMessage.clear();
        customAddressAttachmentName.clear();
        customAddressAttachmentSize.clear();
        customAddressAttachmentPath.clear();
    }

    static inline void clearAllBookingTemplateData(std::map<std::string, std::pair<std::string, std::string>>& templateForEmail, std::string& templateAttachmentName, std::string& templateAttachmentSize, std::string& templateAttachmentPath, bool& templateExists) {
        templateForEmail.clear();
        templateAttachmentName.clear();
        templateAttachmentSize.clear();
        templateAttachmentPath.clear();
        templateExists = false;
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::BOOKING_TEMPLATES_CLEARED_MESSAGE);
    }

    static inline void clearSelectedVenuesForEmails(std::vector<SelectedVenueForEmails>& selectedVenuesForEmails) {
        selectedVenuesForEmails.clear();
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SELECTED_VENUES_CLEARED);
    }

    static inline void clearSelectedVenuesForTemplates(std::vector<SelectedVenueForTemplates>& selectedVenuesForTemplates) {
        selectedVenuesForTemplates.clear();
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SELECTED_VENUES_FOR_TEMPLATES_CLEARED);
    }

    static inline void clearCustomAddressAttachmentData(std::string& customAddressAttachmentName, std::string& customAddressAttachmentSize, std::string& customAddressAttachmentPath) {
        customAddressAttachmentName.clear();
        customAddressAttachmentSize.clear();
        customAddressAttachmentPath.clear();
    }

    static inline void clearEmailAttachmentData(std::string& attachmentName, std::string& attachmentSize, std::string& attachmentPath) {
        attachmentName.clear();
        attachmentSize.clear();
        attachmentPath.clear();
    }

    static inline void clearTemplateAttachmentData(std::string& templateAttachmentName, std::string& templateAttachmentSize, std::string& templateAttachmentPath) {
        templateAttachmentName.clear();
        templateAttachmentSize.clear();
        templateAttachmentPath.clear();
    }

    static inline void clearSubjectMessageData(std::string& subject, std::string& message) {
        subject.clear();
        message.clear();
    }


    // Function to display the email settings from the configuration file
    static void viewEmailSettings(bool useSSL, const std::string& sslCertPath, bool verifyPeer, bool verifyHost, bool verbose,
                           const std::string& senderEmail, const std::string& smtpUsername, int smtpPort, const std::string& smtpServer);

    // Function to validate an email address format
    static bool isValidEmail(const std::string& email);

    static bool isValidURL(const std::string& url);

    // Function to construct an email, including the subject, message, and attachment details
    void constructEmail(std::string& subject, std::string& message, std::string& attachmentPath, std::string& attachmentName, std::string& attachmentSize, std::istream &in = std::cin) const;

    // Function to allow the user to modify the email
    void viewEditEmails(const std::string& senderEmail, 
                        std::string& subject, 
                        std::string& message, 
                        std::string& attachmentName, 
                        std::string& attachmentSize, 
                        std::string& attachmentPath) const;

    void viewEditTemplates(CURL* curl,
                           std::vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                           const std::string& smtpServer,
                           int smtpPort,
                           bool useSSL,
                           bool verifyPeer,
                           const std::string& senderEmail,
                           std::map<std::string, std::pair<std::string, std::string>>& templateForEmail,
                           std::string& templateAttachmentName,
                           std::string& templateAttachmentSize,
                           std::string& templateAttachmentPath,
                           bool& templateExists) const;

    // Function to send an individual email to a selected venue
    static bool sendIndividualEmail(CURL* curl,
                                    const SelectedVenueForEmails& selectedVenueForEmails,
                                    const std::string& senderEmail,
                                    std::string& subject,
                                    std::string& message,
                                    const std::string& smtpServer,
                                    int smtpPort,
                                    bool useSSL,
                                    bool verifyPeer,
                                    std::string& attachmentName,
                                    std::string& attachmentSize,
                                    std::string& attachmentPath,
                                    std::vector<SelectedVenueForEmails>& selectedVenuesForEmails);

    static bool sendBookingTemplateEmails(CURL* curl,
                                          const std::string& senderEmail,
                                          std::map<std::string, std::pair<std::string, std::string>>& templateForEmail,
                                          const std::string& smtpServer,
                                          int smtpPort,
                                          bool useSSL,
                                          bool verifyPeer,
                                          std::string& templateAttachmentName,
                                          std::string& templateAttachmentSize,
                                          std::string& templateAttachmentPath,
                                          bool templateExists);

    // Function to append the string for the booking template
    static void appendIfNotEmpty(std::ostringstream& os, const std::string& label, const std::string& value) ;

    // Function to send a booking template email
    void createBookingTemplate(CURL* curl,
                               std::vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                               const std::string& senderEmail,
                               std::map<std::string, std::pair<std::string, std::string>>& templateForEmail,
                               const std::string& smtpServer,
                               int smtpPort,
                               bool useSSL,
                               bool verifyPeer,
                               std::string& templateAttachmentName,
                               std::string& templateAttachmentSize,
                               std::string& templateAttachmentPath,
                               bool templateExists) const;

    // Function to send to a custom email address
    void emailCustomAddress(CURL* curl,
                            const std::string& senderEmail,
                            std::string& customAddressSubject,
                            std::string& customAddressMessage,
                            const std::string& smtpServer,
                            int smtpPort,
                            bool useSSL,
                            bool verifyPeer,
                            std::string& customAddressAttachmentName,
                            std::string& customAddressAttachmentSize,
                            std::string& customAddressAttachmentPath) const;

    // Function to confirm the email before sending
    static void confirmSendEmail(CURL* curl,
                                 std::vector<SelectedVenueForEmails>& selectedVenuesForEmails,
                                 const std::string& senderEmail,
                                 std::string& subject,
                                 std::string& message,
                                 const std::string& smtpServer,
                                 int smtpPort,
                                 bool useSSL,
                                 bool verifyPeer,
                                 std::string& attachmentName,
                                 std::string& attachmentSize,
                                 std::string& attachmentPath);

    static void confirmSendBookingTemplates(CURL* curl,
                                            std::vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                                            const std::string& senderEmail,
                                            std::map<std::string, std::pair<std::string, std::string>>& templateForEmail,
                                            const std::string& smtpServer,
                                            int smtpPort,
                                            bool useSSL,
                                            bool verifyPeer,
                                            std::string& templateAttachmentName,
                                            std::string& templateAttachmentSize,
                                            std::string& templateAttachmentPath,
                                            bool templateExists);
};

#endif // MAIL_H
