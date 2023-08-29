#ifndef MAIL_H
#define MAIL_H

#include "curl.h"
#include "fileutils.h"
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


    static inline void clearBookingTemplate(std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate, std::string& attachmentName, std::string& attachmentSize, std::string& attachmentPath, bool& templateExists) {
        emailToTemplate.clear();
        attachmentName.clear();
        attachmentSize.clear();
        attachmentPath.clear();
        templateExists = false;
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
        std::cout << "Booking template cleared." << std::endl; 
        ConsoleUtils::resetColor();
    }

    static inline void clearSelectedVenues(std::vector<SelectedVenue>& selectedVenuesForEmail) {
        selectedVenuesForEmail.clear();
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
        std::cout << "Selected venues cleared." << std::endl; 
        ConsoleUtils::resetColor();
    }

    static inline void clearSelectedVenuesForTemplates(std::vector<SelectedVenue>& selectedVenuesForTemplates) {
        selectedVenuesForTemplates.clear();
    }

    static inline void clearCustomAddressAttachmentData(std::string& customAddressAttachmentName, std::string& customAddressAttachmentSize, std::string& customAddressAttachmentPath) {
        customAddressAttachmentName.clear();
        customAddressAttachmentSize.clear();
        customAddressAttachmentPath.clear();
    }

    static inline void clearAttachmentData(std::string& attachmentName, std::string& attachmentSize, std::string& attachmentPath) {
        attachmentName.clear();
        attachmentSize.clear();
        attachmentPath.clear();
    }

    static inline void clearSubjectMessageData(std::string& subject, std::string& message) {
        subject.clear();
        message.clear();
    }


    // Function to display the email settings from the configuration file
    static void viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost, bool verbose,
                           const std::string& senderEmail, int smtpPort, const std::string& smtpServer);

    // Function to validate an email address format
    static bool isValidEmail(const std::string& email);

    // Function to construct an email, including the subject, message, and attachment details
    void constructEmail(std::string& subject, std::string& message, std::string& attachmentPath, std::string& attachmentName, std::string& attachmentSize, std::istream &in = std::cin);

    // Function to allow the user to modify the email
    void viewEditEmails(CURL* curl, 
                        const std::string& smtpServer,
                        int smtpPort,
                        std::vector<SelectedVenue>& selectedVenuesForEmail,
                        const std::string& senderEmail, 
                        std::string& subject, 
                        std::string& message, 
                        std::string& attachmentName, 
                        std::string& attachmentSize, 
                        std::string& attachmentPath, 
                        bool& templateExists,
                        std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate);

    void viewEditTemplates(CURL* curl,
                           const std::string& smtpServer,
                           int smtpPort,
                           std::vector<SelectedVenue>& selectedVenuesForEmail,
                           const std::string& senderEmail,
                           std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate,
                           std::string& attachmentName,
                           std::string& attachmentSize,
                           std::string& attachmentPath,
                           bool& templateExists);

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
                             std::string& attachmentPath,
                             const std::vector<SelectedVenue>& selectedVenuesForEmail);

    static bool sendBookingTemplateEmails(CURL* curl,
                                   const std::string& senderEmail,
                                   std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate,
                                   const std::string& smtpServer,
                                   int smtpPort,
                                   std::string& attachmentName,
                                   std::string& attachmentSize,
                                   std::string& attachmentPath);

    // Function to send a booking template email
    void createBookingTemplate(CURL* curl,
                             const std::string& senderEmail,
                             std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate,
                             const std::string& smtpServer,
                             int smtpPort,
                             std::string& attachmentName,
                             std::string& attachmentSize,
                             std::string& attachmentPath,
                             const std::vector<SelectedVenue>& selectedVenuesForEmail,
                             bool templateExists);

    // Function to send to a custom email address
    void emailCustomAddress(CURL* curl,
                             const std::string& senderEmail,
                             std::string& customAddressSubject,
                             std::string& customAddressMessage,
                             const std::string& smtpServer,
                             int smtpPort,
                             std::string& customAddressAttachmentName,
                             std::string& customAddressAttachmentSize,
                             std::string& customAddressAttachmentPath);

    // Function to confirm the email before sending
    void confirmSendEmail(CURL* curl,
                          std::vector<SelectedVenue>& selectedVenuesForEmail,
                          const std::string& senderEmail,
                          std::string& subject,
                          std::string& message,
                          const std::string& smtpServer,
                          int smtpPort,
                          std::string& attachmentName,
                          std::string& attachmentSize,
                          std::string& attachmentPath);

    static void confirmSendBookingTemplates(CURL* curl,
                                     std::vector<SelectedVenue>& selectedVenuesForTemplates,
                                     const std::string& senderEmail,
                                     std::map<std::string, std::pair<std::string, std::string>>& emailToTemplate,
                                     const std::string& smtpServer,
                                     int smtpPort,
                                     std::string& attachmentName,
                                     std::string& attachmentSize,
                                     std::string& attachmentPath);
};

#endif // MAIL_H
