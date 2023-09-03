#include "include/errorhandler.h"

#include "include/fileutils.h" // Forward declaration due to circular dependency between fileutils.h and errorhandler.h

// Use the standard namespace
using namespace std;

// Define the static member variable
boost::mutex ErrorHandler::outputMutex;

// Function to handle CURL errors
bool ErrorHandler::handleCurlError(CURLcode res) {
    if (res != CURLE_OK) {
        boost::lock_guard<boost::mutex> lock(outputMutex);
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        cerr << boost::format("CURL Error: %s\n") % curl_easy_strerror(res);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        handleErrorAndReturn(ErrorType::LIBCURL_ERROR);
        return false;
    }
    return true;
}

// Overloaded function to handle errors without extra information
void ErrorHandler::handleErrorAndReturn(ErrorType error) {
    handleErrorAndReturn(error, "");
}

// Function to handle various types of errors and display appropriate messages
void ErrorHandler::handleErrorAndReturn(ErrorType error, const string& extraInfo = "") {
    boost::lock_guard<boost::mutex> lock(outputMutex);
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
    // Switch statement to handle different types of errors
    switch (error) {
        case ErrorType::INVALID_INPUT_ERROR:
            cerr << "Invalid input. Skipping." << endl;
            break;
        case ErrorType::INVALID_MENU_INPUT_ERROR:
            cerr << "Invalid input, please enter a number. Press return to retry..." << endl;
            break;
        case ErrorType::INPUT_LENGTH_ERROR:
            cerr << "Input too long. Please try again." << endl;
            break;
        case ErrorType::INVALID_CHOICE_ERROR:
            cerr << "Invalid choice. Press return..." << endl;
            break;
        case ErrorType::INVALID_INDEX_ERROR:
            std::cerr << boost::format("Invalid index: %s\n") % extraInfo;
            break;
        case ErrorType::INVALID_INDEX_FORMAT_ERROR:
            std::cerr << boost::format("Invalid index format. %s Skipping.\n") % extraInfo;
            break;
        case ErrorType::EMPTY_VENUE_LIST_ERROR:
            cerr << "No venues could be loaded for filtering" << endl;
            break;
        case ErrorType::NO_VENUES_SELECTED_ERROR:
            cerr << "No venues selected yet." << endl;
            break;
        case ErrorType::VENUE_ALREADY_SELECTED_ERROR:
            cerr << "Venue already selected. Skipping." << endl;
            break;
        case ErrorType::EMAIL_AND_SUBJECT_BLANK_ERROR:
            cerr << "Subject and Message are required. Please set them." << endl;
            break;
        case ErrorType::EMAIL_AND_SUBJECT_WRITE_ATTEMPTS_ERROR:
            cerr << "Too many unsuccessful attempts." << endl;
            break;
        case ErrorType::NO_VENUES_SELECTED_FOR_EMAIL_ERROR:
            cerr << "No venues selected. Please add venues before sending emails." << endl;
            break;
        case ErrorType::EMAIL_ERROR:
            cerr << "An error occurred while processing the email." << endl;
            break;
        case ErrorType::SUBJECT_LENGTH_ERROR:
            cerr << "Subject cannot be longer than 50 characters in length." << endl;
            break;
        case ErrorType::EMAIL_MESSAGE_LENGTH_ERROR:
            cerr << "Message cannot be longer than 2000 characters in length." << endl;
            break;
        case ErrorType::EMAIL_BLANK_ERROR:
            cerr << "Message cannot be blank." << endl;
            break;
        case ErrorType::SUBJECT_MESSAGE_ERROR:
            cerr << "An error occurred while entering subject and message." << endl;
            break;
        case ErrorType::EMAIL_WRITING_ATTEMPTS_ERROR:
            cerr << "Too many unsuccessful attempts. Returning to main menu." << endl;
            break;
        case ErrorType::TEMPLATE_PENDING_ERROR:
            cerr << "Template pending, go back to add more venues, send the template, or clear the template." << endl;
            break;
        case ErrorType::ATTACHMENT_PATH_ERROR:
            cerr << "File does not exist at the specified path." << endl;
            break;
        case ErrorType::ATTACHMENT_PATH_EMPTY_ERROR:
            cerr << "The path you have selected is empty, please choose a valid attachement." << endl;
            break;
        case ErrorType::ATTACHMENT_SIZE_ERROR:
            cerr << "File exceeds the 24MB limit." << endl;
            break;
        case ErrorType::RECIPIENT_EMAIL_FORMAT_ERROR:
            cerr << "The recipient email formatting is not correct" << endl;
            break;
        case ErrorType::SENDER_EMAIL_FORMAT_ERROR:
            std::cerr << boost::format("Error: The sender email '%1%' is not the valid format.\nPlease set it correctly in your custom.json file.\n") % extraInfo;
            break;
        case ErrorType::EMAIL_PASSWORD_MIN_LENGTH_ERROR:
            cerr << "Error: The email password is too short." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_MAX_LENGTH_ERROR:
            cerr << "Error: The email password is too long." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_MISMATCH_ERROR:
            cerr << "\nEmail passwords do not match. Please try again." << endl;
            break;
        case ErrorType::CONFIG_OPEN_ERROR:
            cerr << "Failed to open " << extraInfo << "." << endl;
            break;
        case ErrorType::CONFIG_LOAD_ERROR:
            cerr << "Failed to load configuration settings from config.json." << endl;
            break;
        case ErrorType::CONFIG_OPEN_TO_WRITE_ERROR:
            cerr << "Failed to open " << extraInfo << " for writing." << endl;
            break;
        case ErrorType::INVALID_CAPACITY_IN_CSV_ERROR:
            std::cerr << boost::format("Invalid capacity in CSV file: %1%\n") % extraInfo;
            break;
        case ErrorType::INVALID_DATA_IN_CSV_ERROR:
            std::cerr << boost::format("Invalid data in CSV file: %1%\n") % extraInfo;
            break;
        case ErrorType::REGISTRATION_KEY_MISSING_ERROR:
            cerr << "The registration key is missing: " << extraInfo << endl;
            break;
        case ErrorType::REGISTRATION_KEY_INVALID_ERROR:
            cerr << "The registration key is invalid: " << extraInfo << endl;
            break;
        case ErrorType::REGISTRATION_FINAL_KEY_DERIVATION_ERROR:
            cerr << "Failed to derive the final key to unlock the database: " << extraInfo << endl;
            break;
        case ErrorType::REGISTRATION_FINAL_IV_DERIVATION_ERROR:
            cerr << "Failed to derive the final IV to unlock the database: " << extraInfo << endl;
            break;
        case ErrorType::DATABASE_OPEN_ERROR:
            cerr << "Database failed to open: " << extraInfo << endl;
            break;
        case ErrorType::DATABASE_QUERY_ERROR: 
            cerr << "Database failed to read venue data from database: " << extraInfo << endl;
            break;
        case ErrorType::SMTP_USERNAME_LENGTH_ERROR:
            cerr << "Invalid SMTP Username length" << extraInfo << endl;
            break;
        case ErrorType::SMTP_SERVER_LENGTH_ERROR:
            cerr << "Invalid SMTP Server length." << extraInfo << endl;
            break;
        case ErrorType::SMTP_PORT_FORMAT_ERROR:
            cerr << "SMTP Port is not formatted correctly" << endl;
            break;
        case ErrorType::SMTP_CONNECTION_ERROR:
            cerr << "Connection to SMTP server failed." << endl;
            break;
        case ErrorType::SMTP_AUTH_ERROR:
            cerr << "Authentication with SMTP server failed." << endl;
            break;
        case ErrorType::SSL_CERT_PATH_ERROR:
            cerr << "SSL certificate path is not valid." << endl;
            break;
#ifdef UNIT_TESTING
        default:
            cerr << "" << endl;
            break;
#else
        default:
            cerr << "An unknown error occurred." << endl;
            break;
#endif

    }
#ifndef UNIT_TESTING
    // After displaying the error, reset the console text color
    ConsoleUtils::resetColor();
#endif
}

// Overloaded function to handle errors without extra information
void ErrorHandler::handleErrorAndThrow(ErrorType error) {
    handleErrorAndReturn(error, "");
}

void ErrorHandler::handleErrorAndThrow(ErrorType error, const std::string& extraInfo) {
    boost::lock_guard<boost::mutex> lock(outputMutex); // Lock the mutex
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
    std::string errorMessage;
    switch(error) {
        case ErrorType::MENU_LOAD_ERROR:
            errorMessage = "Menu failed to load, please restart the program.";
            break;
        case ErrorType::INVALID_INDEX_FORMAT_ERROR:
            errorMessage = boost::str(boost::format("Invalid index format. %s Skipping.") % extraInfo);
            break;
        case ErrorType::SUBJECT_MESSAGE_ERROR:
            errorMessage = "An error occurred while entering subject and message.";
            break;
        case ErrorType::SQLITE_DATABASE_OPEN_ERROR:
            std::cerr << boost::str(boost::format("Database failed to open: %s\n") % extraInfo);
            break;
        case ErrorType::FILESYSTEM_ERROR:
            errorMessage = boost::str(boost::format("Filesystem error: %s") % extraInfo);
            break;
        case ErrorType::LIBCURL_ERROR:
#ifndef UNIT_TESTING
            errorMessage = "Failed to initialize libcurl.";
            if (!extraInfo.empty()) {
                errorMessage = boost::str(boost::format("%s %s") % errorMessage % extraInfo);
            }
#endif
            break;
        case ErrorType::TERMINAL_GET_ATTRIBUTES_ERROR:
            errorMessage = "Failed to get terminal attributes.";
            break;
        case ErrorType::TERMINAL_SET_ATTRIBUTES_ERROR:
            errorMessage = "Failed to set terminal attributes.";
            break;
        case ErrorType::TERMINAL_RESTORE_ATTRIBUTES_ERROR:
            errorMessage = "Failed to restore terminal attributes.";
            break;
        default:
            errorMessage = "An unknown error occurred.";
            break;
    }
#ifndef UNIT_TESTING
    // After displaying the error, reset the console text color
    ConsoleUtils::resetColor();
#endif

    // Throw the exception
    std::cerr << errorMessage << std::endl;
    throw ErrorHandlerException(errorMessage);
}