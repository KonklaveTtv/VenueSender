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
    
        // Menu/Selection Errors
        case ErrorType::INVALID_CHOICE_ERROR:
            cerr << "ERROR: Invalid choice." "\n";
            break;
        case ErrorType::INVALID_INDEX_ERROR:
            cerr << boost::format("ERROR: Invalid index: %s\n") % extraInfo;
            break;
        case ErrorType::INVALID_INDEX_FORMAT_ERROR:
            cerr << boost::format("ERROR: Invalid index format. %s Skipping.\n") % extraInfo;
            break;
        case ErrorType::INVALID_CHAR_INPUT_ERROR:
            cerr << "ERROR: Please enter 'Y' or 'y' or 'N' or 'n'." << endl;
            break;
        case ErrorType::INVALID_POSITIVE_CHAR_INPUT_ERROR:
            cerr << "ERROR: Please enter 'Y' or 'y'." << endl;
            break;
        case ErrorType::INVALID_NEGATIVE_CHAR_INPUT_ERROR:
            cerr << "ERROR: Please enter 'N' or 'n'." << endl;
            break;
        case ErrorType::INVALID_REDO_MODIFY_INPUT_ERROR:
            cerr << "Invalid input. Please enter 'REDO' or 'MODIFY'." << endl;
            break;
        case ErrorType::INVALID_INPUT_ERROR:
            cerr << "ERROR: Invalid input. Skipping." << endl;
            break;
        case ErrorType::INVALID_MENU_CHOICE_ERROR:
            cerr << "ERROR: Invalid input. Skipping." << endl;
            break;
        case ErrorType::INVALID_INPUT_LENGTH_ERROR:
            cerr << "ERROR: Input too long. Please try again." << endl;
            break;
        case ErrorType::INVALID_MENU_INPUT_ERROR:
            cerr << "ERROR: Invalid input. Please enter a number (press return to retry...)";
            break;
        case ErrorType::EMPTY_VENUE_LIST_ERROR:
            cerr << "ERROR: No venues could be loaded for filtering." << endl;
            break;
        case ErrorType::NO_VENUES_SELECTED_ERROR:
            cerr << "ERROR: No venues selected." << endl;
            break;
        case ErrorType::VENUE_ALREADY_SELECTED_ERROR:
            cerr << "===========================" << endl
                 << "      ALREADY SELECTED     " << endl
                 << "===========================" << endl
                 << extraInfo << endl
                 << "---------------------------" << endl;
            break;
        case ErrorType::NO_VENUES_SELECTED_FOR_EMAIL_ERROR:
            cerr << "ERROR: No venues selected for emails. Please add venues before sending emails." << endl;
            break;
        case ErrorType::NO_VENUES_SELECTED_FOR_TEMPLATE_ERROR:
            cerr << "ERROR: No venues selected for templates. Please select venues first before attempting to send the template." << endl;
            break;


        // Email/Attachment/Template Errors
        case ErrorType::SUBJECT_LENGTH_ERROR:
            cerr << "ERROR: Subject cannot be longer than 50 characters in length." << endl;
            break;
        case ErrorType::SUBJECT_MESSAGE_ERROR:
            cerr << "ERROR: An error occurred while entering subject and message." << endl;
            break;
        case ErrorType::EMAIL_BLANK_ERROR:
            cerr << "ERROR: Message cannot be blank." << endl;
            break;
        case ErrorType::EMAIL_MESSAGE_LENGTH_ERROR:
            cerr << "ERROR: Message cannot be longer than 2000 characters in length." << endl;
            break;
        case ErrorType::EMAIL_SENDING_ERROR:
            cerr << "ERROR: Failed to send email to " << extraInfo << endl;
            break;
        case ErrorType::EMAIL_SEND_FAILURE_ERROR:
            cerr << "===========================" << endl;
            cerr << "    EMAIL SENDING FAILED   " << endl;
            cerr << "===========================" << endl;
            break;
        case ErrorType::EMAIL_WRITING_ATTEMPTS_ERROR:
            cerr << "ERROR: TOO MANY UNSUCCESSFUL ATTEMPTS. Returning to main menu." << endl;
            break;
        case ErrorType::EMAIL_ERROR:
            cerr << "ERROR: An error occurred while processing the email." << endl;
            break;
        case ErrorType::EMAIL_AND_SUBJECT_BLANK_ERROR:
            cerr << "ERROR: Subject and Message are required" << endl;
            break;
        case ErrorType::EMAIL_AND_SUBJECT_WRITE_ATTEMPTS_ERROR:
            cerr << "ERROR: Too many unsuccessful attempts." << endl;
            break;
        case ErrorType::RECIPIENT_EMAIL_FORMAT_ERROR:
            cerr << "ERROR: The recipient email formatting is not correct" << endl;
            break;
        case ErrorType::SENDER_EMAIL_FORMAT_ERROR:
            cerr << boost::format("ERROR: The sender email '%1%' is not the valid format.\nPlease set it correctly in your custom.json file.\n") % extraInfo;
            break;
        case ErrorType::ATTACHMENT_PATH_ERROR:
            cerr << "ERROR: File does not exist at the specified path." << endl;
            break;
        case ErrorType::ATTACHMENT_PATH_EMPTY_ERROR:
            cerr << "ERROR: The path you have selected is empty, please choose a valid attachement." << endl;
            break;
        case ErrorType::ATTACHMENT_SIZE_ERROR:
            cerr << "ERROR: File exceeds the 24MB limit." << endl;
            break;
        case ErrorType::TEMPLATE_CREATION_ERROR:
            cerr << "ERROR: No templates have been created. Please create a template first." << endl;
            break;
        case ErrorType::TEMPLATE_EMPTY_ERROR:
            cerr << "ERROR: The template fields are empty." << endl;
            break;
        case ErrorType::TEMPLATE_PENDING_ERROR:
            cerr << "ERROR: Template pending, go back to add more venues, send the template, or clear the template." << endl;
            break;
        case ErrorType::TEMPLATE_NAME_FIELD_EMPTY_FIELD_ERROR:
            cerr << "ERROR: The name field must not be empty. Please enter your name." << endl;
            break;
        case ErrorType::TEMPLATE_POSSIBLE_ENTRY_ERROR:
            cerr << "The URL seems to be invalid. Is this correct? (Y/N): ";
            break;
        case ErrorType::TEMPLATE_SENDING_FAILED_ERROR:
            cerr << "===========================" << endl;
            cerr << "  TEMPLATE SENDING FAILED  " << endl;
            cerr << "===========================" << endl;
            break;


        // Email/cURL Config Errors
        case ErrorType::EMAIL_PASSWORD_EMPTY_ERROR:
            cerr << "ERROR: The email password has not been set." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_MIN_LENGTH_ERROR:
            cerr << "ERROR: The email password is too short." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_MAX_LENGTH_ERROR:
            cerr << "ERROR: The email password is too long." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_MISMATCH_ERROR:
            cerr << "\nERROR: Email passwords do not match. Please try again." << endl;
            break;
        case ErrorType::SMTP_USERNAME_LENGTH_ERROR:
            cerr << "ERROR: Invalid SMTP Username length" << extraInfo << endl;
            break;
        case ErrorType::SMTP_USERNAME_NON_MATCH_ERROR:
            cerr << "ERROR: Your SMTP Username is not an email, is this correct? (y/n): ";
            break;
        case ErrorType::SMTP_SERVER_LENGTH_ERROR:
            cerr << "ERROR: Invalid SMTP Server length." << extraInfo << endl;
            break;
        case ErrorType::SMTP_PORT_FORMAT_ERROR:
            cerr << "ERROR: SMTP Port is not formatted correctly" << endl;
            break;
        case ErrorType::SMTP_CONNECTION_ERROR:
            cerr << "ERROR: Connection to SMTP server failed." << endl;
            break;
        case ErrorType::SMTP_AUTH_ERROR:
            cerr << "ERROR: Authentication with SMTP server failed." << endl;
            break;
        case ErrorType::SSL_CONFIGURATION_ERROR:
            cerr << "ERROR: Verify Peer must be enabled when using SSL." << endl;
            break;
        case ErrorType::SSL_CERT_PATH_ERROR:
            cerr << "ERROR: SSL certificate path is not valid." << endl;
            break;
        case ErrorType::INVALID_URL_ERROR:
            cerr << "ERROR: Invalid URL." << endl;
            break;


        // System errors
        case ErrorType::X11_DISPLAY_ERROR:
            cerr << "ERROR: Failed to open X11 display." << endl;
            break;
        case ErrorType::X11_CAPS_LOCK_ERROR:
            cerr << "\nWARNING: Caps Lock is ON." << endl;
            break;
        case ErrorType::X11_SYSTEM_ERROR:
            cerr << "ERROR: Failed to get X11 state." << endl;
            break;
        case ErrorType::X11_UNSUPPORTED_PLATFORM_ERROR:
            cerr << "ERROR: Caps Lock check not supported on this platform.\n" << endl;
            break;
        case ErrorType::OPENSSL_INITIALIZATION_ERROR:
            cerr << "ERROR: Failed to initialize OpenSSL context.\n" << endl;
            break;
        case ErrorType::REGISTRATION_KEY_MISSING_ERROR:
            cerr << "ERROR: The registration key is missing: " << extraInfo << endl;
            break;
        case ErrorType::REGISTRATION_KEY_INVALID_ERROR:
            cerr << "ERROR: The registration key is invalid: " << extraInfo << endl;
            break;
        case ErrorType::REGISTRATION_FINAL_KEY_DERIVATION_ERROR:
            cerr << "ERROR: Failed to derive the final key to unlock the database: " << extraInfo << endl;
            break;
        case ErrorType::REGISTRATION_FINAL_IV_DERIVATION_ERROR:
            cerr << "ERROR: Failed to derive the final IV to unlock the database: " << extraInfo << endl;
            break;
        case ErrorType::CONFIG_OPEN_ERROR:
            cerr << "ERROR: Failed to open " << extraInfo << "." << endl;
            break;
        case ErrorType::CONFIG_LOAD_ERROR:
            cerr << "ERROR: Failed to load configuration settings from config.json." << endl;
            break;
        case ErrorType::CONFIG_OPEN_TO_WRITE_ERROR:
            cerr << "ERROR: Failed to open " << extraInfo << " for writing." << endl;
            break;
        case ErrorType::FILE_NOT_OPEN_ERROR:
            cerr << "ERROR: File not open" << endl;
            break;
        case ErrorType::UNKNOWN_ERROR:
            cerr << "ERROR: Unknown Error" << endl;
            break;

        // Database Errors
        case ErrorType::INVALID_CAPACITY_IN_CSV_ERROR:
            cerr << boost::format("ERROR: Invalid capacity in CSV file: %1%\n") % extraInfo << endl;
            break;
        case ErrorType::INVALID_DATA_IN_CSV_ERROR:
            cerr << boost::format("ERROR: Invalid data in CSV file: %1%\n") % extraInfo << endl; 
            break;
        case ErrorType::SQLITE_DATABASE_DECRYPTION_ERROR:
            cerr << "ERROR: Failed to decrypt SQLite database.\n" << endl;
            break;
        case ErrorType::SQLITE_MEMORY_ALLOCATION_ERROR:
            cerr << "ERROR: Failed to open in-memory SQLite database: " << extraInfo << "\n" << endl;
            break;
        case ErrorType::SQLITE_DECRYPTED_DATABASE_LOAD_ERROR:
            cerr << "ERROR: Failed to load decrypted data into SQLite database: " << extraInfo << "\n" << endl;
            break;
        case ErrorType::DATABASE_OPEN_ERROR:
            cerr << "ERROR: Database failed to open: " << extraInfo << endl;
            break;
        case ErrorType::DATABASE_QUERY_ERROR: 
            cerr << "ERROR: Database failed to read venue data from database: " << extraInfo << endl;
            break;
        case ErrorType::SQLITE_STATEMENT_ERROR:
            cerr << "ERROR: Failed to prepare statement: " << extraInfo << endl;
            break;

        // Boost Iostreams Errors
        case ErrorType::BOOST_IOSTREAMS_ERROR:
            cerr << boost::format("ERROR: Boost Iostreams Error: %s\n") % extraInfo;
            break;

        // Default Error
#ifdef UNIT_TESTING
        default:
            cerr << "" << endl;
            break;
#else
        default:
            cerr << "ERROR: An unknown error occurred." << endl;
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

void ErrorHandler::handleErrorAndThrow(ErrorType error, const string& extraInfo) {
    boost::lock_guard<boost::mutex> lock(outputMutex); // Lock the mutex
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
    string errorMessage;
    switch(error) {

        // Menu errors
        case ErrorType::MENU_LOAD_ERROR:
            errorMessage = "ERROR: Menu failed to load, please restart the program.";
            break;


        // Email Errors
        case ErrorType::LIBCURL_ERROR:
#ifndef UNIT_TESTING
            errorMessage = "ERROR: Failed to initialize libcurl.";
            if (!extraInfo.empty()) {
                errorMessage = boost::str(boost::format("%s %s") % errorMessage % extraInfo);
            }
#endif
            break;
        case ErrorType::MIME_INIT_ERROR:
            cerr << "ERROR: cURL MIME initialization failed." << endl;
            break;
        case ErrorType::MIME_PART_ERROR:
            cerr << "ERROR: cURL MIME part failed." << endl;
            break;


        // System Errors
        case ErrorType::FILESYSTEM_ERROR:
            errorMessage = boost::str(boost::format("ERROR: Filesystem error: %s") % extraInfo);
            break;
        case ErrorType::TERMINAL_GET_ATTRIBUTES_ERROR:
            errorMessage = "ERROR: Failed to get terminal attributes.";
            break;
        case ErrorType::TERMINAL_SET_ATTRIBUTES_ERROR:
            errorMessage = "ERROR: Failed to set terminal attributes.";
            break;
        case ErrorType::TERMINAL_RESTORE_ATTRIBUTES_ERROR:
            errorMessage = "ERROR: Failed to restore terminal attributes.";
            break;


        // Database Errors
        case ErrorType::SQLITE_DATABASE_OPEN_ERROR:
            cerr << boost::str(boost::format("ERROR: Database failed to open: %s\n") % extraInfo);
            break;


        // Default Error
#ifdef UNIT_TESTING
        default:
            cerr << "" << endl;
            break;
#else
        default:
            cerr << "ERROR: An unknown error occurred." << endl;
            break;
#endif
    }
#ifndef UNIT_TESTING
    // After displaying the error, reset the console text color
    ConsoleUtils::resetColor();
#endif

    // Throw the exception
    cerr << errorMessage << endl;
    throw ErrorHandlerException(errorMessage);
}