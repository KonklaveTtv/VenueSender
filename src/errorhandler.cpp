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
        case ErrorType::EMAIL_ERROR:
            cerr << "An error occurred while processing the email." << endl;
            [[fallthrough]];
        case ErrorType::SUBJECT_MESSAGE_ERROR:
            cerr << "An error occurred while entering subject and message." << endl;
            [[fallthrough]];
        case ErrorType::CONFIG_LOAD_ERROR:
            cerr << "Failed to load configuration settings from config.json." << endl;
            [[fallthrough]];
        case ErrorType::DECRYPTION_ERROR:
            cerr << "Failed to decrypt passwords. Ensure they are correctly encrypted in config.json." << endl;
            [[fallthrough]];
        case ErrorType::LIBCURL_ERROR:
            cerr << "Failed to initialize libcurl." << endl;
            [[fallthrough]];
        default:
            cerr << "An unknown error occurred." << endl;
            break;
    }
    showInfoAndReturn();
}