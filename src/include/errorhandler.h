#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include "fileutils.h"

#include <iostream>

class ErrorHandler {
public:
    enum class ErrorType {
    EMAIL_ERROR,
    SUBJECT_MESSAGE_ERROR,
    CONFIG_LOAD_ERROR,
    DECRYPTION_ERROR,
    LIBCURL_ERROR
    };

    // Function to allow the user time to see the information
    void showInfoAndReturn();

    // Function to show info to user before returning
    void handleErrorAndReturn(ErrorType error);
};

#endif // ERRORHANDLER_H