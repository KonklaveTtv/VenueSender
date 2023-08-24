#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <curl/curl.h>

#include <iostream>
#include <mutex>

// Forward declarations due to circular dependency between fileutils.h and errorhandler.h
class CsvReader; 
class ConfigManager;
class ConsoleUtils; 

// ErrorHandler class to manage different types of errors
class ErrorHandler {
private:
    static std::mutex outputMutex; // Mutex for thread-safe output
public:
    // Enumeration to represent different types of errors that can be encountered
    enum class ErrorType {
    INVALID_INPUT_ERROR,
    INVALID_MENU_INPUT_ERROR,
    INPUT_LENGTH_ERROR,
    INVALID_CHOICE_ERROR,
    INVALID_INDEX_ERROR,
    INVALID_INDEX_FORMAT_ERROR,
    NO_VENUES_SELECTED_ERROR,
    VENUE_ALREADY_SELECTED_ERROR,
    EMAIL_AND_SUBJECT_BLANK_ERROR,
    EMAIL_AND_SUBJECT_WRITE_ATTEMPTS_ERROR,
    NO_VENUES_SELECTED_FOR_EMAIL_ERROR,
    EMAIL_ERROR,
    SUBJECT_LENGTH_ERROR,
    EMAIL_MESSAGE_LENGTH_ERROR,
    EMAIL_BLANK_ERROR,
    SUBJECT_MESSAGE_ERROR,
    EMAIL_WRITING_ATTEMPTS_ERROR,
    ATTACHMENT_PATH_ERROR,
    ATTACHMENT_SIZE_ERROR,
    SENDER_EMAIL_FORMAT_ERROR,
    FILESYSTEM_ERROR,
    CONFIG_OPEN_ERROR,
    CONFIG_LOAD_ERROR,
    CONFIG_OPEN_TO_WRITE_ERROR,
    INVALID_CAPACITY_IN_CSV,
    INVALID_DATA_IN_CSV,
    LIBSODIUM_INIT_ERROR,
    ENCRYPTION_ERROR,
    DECRYPTION_ERROR,
    EMAIL_PASSWORD_ENCRYPTION_ERROR,
    EMAIL_PASSWORD_DECRYPTION_ERROR,
    EMAIL_PASSWORD_ENCRYPTION_FORMAT_ERROR,
    LIBCURL_ERROR,
    SMTP_CONNECTION_ERROR,
    SMTP_AUTH_ERROR
    };

    // Function to handle errors and display appropriate messages based on the type of error
    void handleErrorAndReturn(ErrorType error);
    
    // Overloaded version with extra information
    void handleErrorAndReturn(ErrorType error, const std::string& extraInfo);

    // Handles cURL errors and returns a boolean to indicate success/failure
    bool handleCurlError(CURLcode res);
};

#endif // ERRORHANDLER_H