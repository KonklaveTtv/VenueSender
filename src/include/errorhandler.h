#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include "fileutils.h"

#include <iostream>

class ErrorHandler {
public:
    enum class ErrorType {
    INVALID_INPUT_ERROR,
    INVALID_MENU_INPUT_ERROR,
    INPUT_LENGTH_ERROR,
    INVALID_CHOICE_ERROR,
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
    FILESYSTEM_ERROR,
    CONFIG_OPEN_ERROR,
    CONFIG_LOAD_ERROR,
    CONFIG_OPEN_TO_WRITE_ERROR,
    ENCRYPTION_ERROR,
    DECRYPTION_ERROR,
    EMAIL_PASSWORD_ENCRYPTION_ERROR,
    EMAIL_PASSWORD_DECRYPTION_ERROR,
    LIBCURL_ERROR,
    SMTP_CONNECTION_ERROR,
    SMTP_AUTH_ERROR
    };

    // Function to allow the user time to see the information before returning to main menu
    void showInfoAndReturn();

    // Function to allow the user time to see the information before continuing
    void showInfoAndRetry();

    // Function to show info to user before returning
    void handleErrorAndReturn(ErrorType error);
};

#endif // ERRORHANDLER_H