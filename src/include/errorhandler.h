#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <curl/curl.h>

#include <iostream>
#include <mutex>

// Boost Libraries
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/format.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

// Forward declarations due to circular dependency between fileutils.h and errorhandler.h
class ConfigManager;
class ConsoleUtils; 

class ErrorHandlerException : public std::runtime_error {
public:
    explicit ErrorHandlerException(const std::string& message) : std::runtime_error(message) {}
};

// ErrorHandler class to manage different types of errors
class ErrorHandler {
private:
    static boost::mutex outputMutex; // Mutex for thread-safe output
public:
    // Enumeration to represent different types of errors that can be encountered
    enum class ErrorType {

    // Menu/Selection Errors
    MENU_LOAD_ERROR,
    INVALID_CHOICE_ERROR,
    INVALID_INDEX_ERROR,
    INVALID_INDEX_FORMAT_ERROR,
    INVALID_CHAR_INPUT_ERROR,
    INVALID_POSITIVE_CHAR_INPUT_ERROR,
    INVALID_NEGATIVE_CHAR_INPUT_ERROR,
    INVALID_REDO_MODIFY_INPUT_ERROR,
    INVALID_INPUT_ERROR,
    INVALID_MENU_CHOICE_ERROR,
    INVALID_INPUT_LENGTH_ERROR,
    INVALID_MENU_INPUT_ERROR,
    EMPTY_VENUE_LIST_ERROR,
    NO_VENUES_SELECTED_ERROR,
    VENUE_ALREADY_SELECTED_ERROR,
    NO_VENUES_SELECTED_FOR_EMAIL_ERROR,
    NO_VENUES_SELECTED_FOR_TEMPLATE_ERROR,

    // Email/Attachment/Template Errors
    SUBJECT_LENGTH_ERROR,
    SUBJECT_MESSAGE_ERROR,
    EMAIL_BLANK_ERROR,
    EMAIL_MESSAGE_LENGTH_ERROR,    
    EMAIL_SENDING_ERROR,
    EMAIL_SEND_FAILURE_ERROR,
    EMAIL_WRITING_ATTEMPTS_ERROR,
    EMAIL_ERROR,  
    EMAIL_AND_SUBJECT_BLANK_ERROR,
    EMAIL_AND_SUBJECT_WRITE_ATTEMPTS_ERROR,
    RECIPIENT_EMAIL_FORMAT_ERROR,
    SENDER_EMAIL_FORMAT_ERROR,
    ATTACHMENT_PATH_ERROR,
    ATTACHMENT_PATH_EMPTY_ERROR,
    ATTACHMENT_SIZE_ERROR,
    TEMPLATE_CREATION_ERROR,
    TEMPLATE_EMPTY_ERROR,
    TEMPLATE_PENDING_ERROR,
    TEMPLATE_NAME_FIELD_EMPTY_FIELD_ERROR,
    TEMPLATE_POSSIBLE_ENTRY_ERROR,
    TEMPLATE_SENDING_FAILED_ERROR,

    // Email/cURL Config Errors
    EMAIL_PASSWORD_EMPTY_ERROR,
    EMAIL_PASSWORD_MIN_LENGTH_ERROR,
    EMAIL_PASSWORD_MAX_LENGTH_ERROR,
    EMAIL_PASSWORD_MISMATCH_ERROR,
    SMTP_USERNAME_LENGTH_ERROR,
    SMTP_USERNAME_NON_MATCH_ERROR,
    SMTP_SERVER_LENGTH_ERROR,
    SMTP_PORT_FORMAT_ERROR,
    SMTP_CONNECTION_ERROR,
    SMTP_AUTH_ERROR,
    SSL_CONFIGURATION_ERROR,
    SSL_CERT_PATH_ERROR,
    INVALID_URL_ERROR,
    LIBCURL_ERROR,
    MIME_INIT_ERROR,
    MIME_PART_ERROR,

    // System Errors
    OPENSSL_INITIALIZATION_ERROR,
    FILESYSTEM_ERROR,
    TERMINAL_GET_ATTRIBUTES_ERROR,
    TERMINAL_SET_ATTRIBUTES_ERROR,
    TERMINAL_RESTORE_ATTRIBUTES_ERROR,
    REGISTRATION_KEY_MISSING_ERROR,
    REGISTRATION_KEY_INVALID_ERROR,
    REGISTRATION_FINAL_KEY_DERIVATION_ERROR,
    REGISTRATION_FINAL_IV_DERIVATION_ERROR,
    CONFIG_OPEN_ERROR,
    CONFIG_LOAD_ERROR,
    CONFIG_OPEN_TO_WRITE_ERROR,
    FILE_NOT_OPEN_ERROR,
    UNKNOWN_ERROR,


    // Database Errors
    INVALID_CAPACITY_IN_CSV_ERROR,
    INVALID_DATA_IN_CSV_ERROR,
    SQLITE_DATABASE_DECRYPTION_ERROR,
    SQLITE_MEMORY_ALLOCATION_ERROR,
    SQLITE_DECRYPTED_DATABASE_LOAD_ERROR,
    SQLITE_STATEMENT_ERROR,
    DATABASE_OPEN_ERROR,
    DATABASE_QUERY_ERROR,
    SQLITE_DATABASE_OPEN_ERROR,

    // Boost Errors
    BOOST_IOSTREAMS_ERROR
    };

    // Function to handle errors and display appropriate messages based on the type of error
    static void handleErrorAndReturn(ErrorType error);
    
    // Overloaded version with extra information
    static void handleErrorAndReturn(ErrorType error, const std::string& extraInfo);

    // Handles cURL errors and returns a boolean to indicate success/failure
    static bool handleCurlError(CURLcode res);

    // Function to handle errors and display appropriate messages based on the type of error
    static void handleErrorAndThrow(ErrorType error);

    // Overloaded version with extra information
    static void handleErrorAndThrow(ErrorType error, const std::string& extraInfo);

};

#endif // ERRORHANDLER_H