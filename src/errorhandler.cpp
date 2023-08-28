#include "include/errorhandler.h"

#include "include/fileutils.h" // Forward declaration due to circular dependency between fileutils.h and errorhandler.h

// Use the standard namespace
using namespace std;

// Define the static member variable
std::mutex ErrorHandler::outputMutex;

// Function to handle CURL errors
bool ErrorHandler::handleCurlError(CURLcode res) {
    // Check if CURL operation was successful
    if (res != CURLE_OK) {
        cerr << "CURL Error: " << curl_easy_strerror(res) << endl; // Display CURL error message
        handleErrorAndReturn(ErrorType::LIBCURL_ERROR); // Handle the error
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
    std::lock_guard<std::mutex> lock(outputMutex); // Lock the mutex

    // Set text color to red for error messages
    ConsoleUtils::setColor(ConsoleUtils::Color::RED);

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
            cerr << "Invalid index: " << extraInfo << endl;
            break;
        case ErrorType::INVALID_INDEX_FORMAT_ERROR:
            cerr << "Invalid index format." << extraInfo << "Skipping." << endl;
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
            cerr << "Error: The sender email '" << extraInfo << "' is not the valid format." << endl;
            cerr << "Please set it correctly in your custom.json file." << endl;
            break;
        case ErrorType::FILESYSTEM_ERROR:
            cerr << "Filesystem error: ";
            if (!extraInfo.empty()) {
                cerr << extraInfo;
            }
            cerr << endl;
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
        case ErrorType::INVALID_CAPACITY_IN_CSV:
            cerr << "Invalid capacity in CSV file: " << extraInfo << endl;
            break;
        case ErrorType::INVALID_DATA_IN_CSV:
            cerr << "Invalid data in CSV file: " << extraInfo << endl;
            break;
        case ErrorType::LIBSODIUM_INIT_ERROR:
            cerr << "Failed to initialize libsodium.";
            if (!extraInfo.empty()) {
                cerr << " " << extraInfo;
            }
            cerr << endl;
            break;        
        case ErrorType::ENCRYPTION_ERROR:
            cerr << "Failed to encrypt email password for saving in config.json." << endl;
            break;
        case ErrorType::DECRYPTION_ERROR:
            cerr << "Failed to decrypt passwords. Ensure they are correctly encrypted in config.json." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_ENCRYPTION_ERROR:
            cerr << "Failed to encrypt password." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_DECRYPTION_ERROR:
            cerr << "Email password decryption failed." << endl;
            break;
        case ErrorType::EMAIL_PASSWORD_ENCRYPTION_FORMAT_ERROR:
            cerr << "Invalid encrypted password format." << endl;
            break;
        case ErrorType::LIBCURL_ERROR:
#ifndef UNIT_TESTING
            cerr << "Failed to initialize libcurl." << endl;
            if (!extraInfo.empty()) {
                cerr << " " << extraInfo;
            }
            cerr << endl;
#endif
            break;
        case ErrorType::SMTP_CONNECTION_ERROR:
            cerr << "Connection to SMTP server failed." << endl;
            break;
        case ErrorType::SMTP_AUTH_ERROR:
            cerr << "Authentication with SMTP server failed." << endl;
            break;
        default:
            cerr << "An unknown error occurred." << endl;
            break;
    }
    // After displaying the error, reset the console text color
    ConsoleUtils::resetColor();
}