#include "errorhandler.h"

using namespace std;

// Function to show info to user before returning
void ErrorHandler::showInfoAndReturn() {
    cout << "Press return to go back to Main Menu..." << endl;
    ConsoleUtils::clearInputBuffer();
    cin.get();     // This will wait for a key press
}

void ErrorHandler::handleErrorAndReturn(ErrorType error) {
    switch (error) {
        case ErrorType::INVALID_INPUT_ERROR:
            cerr << "Invalid input. Skipping." << endl;
            [[fallthrough]];
        case ErrorType::INVALID_MENU_INPUT_ERROR:
            cerr << "Invalid input, please enter a number. Press return to retry..." << endl;
            [[fallthrough]];
        case ErrorType::INPUT_LENGTH_ERROR:
            cerr << "Input too long. Please try again." << endl;
            [[fallthrough]];
        case ErrorType::INVALID_CHOICE_ERROR:
            cerr << "Invalid choice. Press return..." << endl;
            [[fallthrough]];
        case ErrorType::INVALID_INDEX_FORMAT_ERROR:
            cerr << "Invalid index format. Skipping." << endl;
            [[fallthrough]];
        case ErrorType::NO_VENUES_SELECTED_ERROR:
            cerr << "No venues selected yet." << endl;
            [[fallthrough]];
        case ErrorType::VENUE_ALREADY_SELECTED_ERROR:
            cerr << "Venue already selected. Skipping." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_AND_SUBJECT_BLANK_ERROR:
            cerr << "Too many unsuccessful attempts." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_AND_SUBJECT_WRITE_ATTEMPTS_ERROR:
            cerr << "Subject and Message are required. Please set them." << endl;
            [[fallthrough]];
        case ErrorType::NO_VENUES_SELECTED_FOR_EMAIL_ERROR:
            cerr << "No venues selected. Please add venues before sending emails." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_ERROR:
            cerr << "An error occurred while processing the email." << endl;
            [[fallthrough]];
        case ErrorType::SUBJECT_LENGTH_ERROR:
            cerr << "Subject cannot be longer than 50 characters in length." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_MESSAGE_LENGTH_ERROR:
            cerr << "Message cannot be longer than 2000 characters in length." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_BLANK_ERROR:
            cerr << "Message cannot be blank." << endl;
            [[fallthrough]];
        case ErrorType::SUBJECT_MESSAGE_ERROR:
            cerr << "An error occurred while entering subject and message." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_WRITING_ATTEMPTS_ERROR:
            cerr << "Too many unsuccessful attempts. Returning to main menu." << endl;
            [[fallthrough]];
        case ErrorType::ATTACHMENT_PATH_ERROR:
            cerr << "File does not exist at the specified path." << endl;
            [[fallthrough]];
        case ErrorType::FILESYSTEM_ERROR:
            cerr << "Filesystem error: " << endl;
            [[fallthrough]];
        case ErrorType::CONFIG_OPEN_ERROR:
            cerr << "Failed to open config.json." << endl;
            [[fallthrough]];
        case ErrorType::CONFIG_LOAD_ERROR:
            cerr << "Failed to load configuration settings from config.json." << endl;
            [[fallthrough]];
        case ErrorType::CONFIG_OPEN_TO_WRITE_ERROR:
            cerr << "Failed to open config.json for writing." << endl;
            [[fallthrough]];
        case ErrorType::ENCRYPTION_ERROR:
            cerr << "Failed to encrypt email password for saving in config.json." << endl;
            [[fallthrough]];
        case ErrorType::DECRYPTION_ERROR:
            cerr << "Failed to decrypt passwords. Ensure they are correctly encrypted in config.json." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_PASSWORD_ENCRYPTION_ERROR:
            cerr << "Failed to encrypt password." << endl;
            [[fallthrough]];
        case ErrorType::EMAIL_PASSWORD_DECRYPTION_ERROR:
            cerr << "Email password decryption failed." << endl;
            [[fallthrough]];
        case ErrorType::LIBCURL_ERROR:
            cerr << "Failed to initialize libcurl." << endl;
            [[fallthrough]];
        case ErrorType::SMTP_CONNECTION_ERROR:
            cerr << "Connection to SMTP server failed." << endl;
            [[fallthrough]];
        case ErrorType::SMTP_AUTH_ERROR:
            cerr << "Authentication with SMTP server failed." << endl;
            [[fallthrough]];
        default:
            cerr << "An unknown error occurred." << endl;
            break;
    }
    showInfoAndReturn();
}