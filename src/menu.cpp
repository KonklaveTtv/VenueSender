#include "menu.h"

using namespace std;

// Function to validate user input
bool isValidMenuChoice(int choice) {
    // Validate if the choice is within valid menu options
    return choice >= static_cast<int>(MenuOption::FilterByGenre) &&
           choice <= static_cast<int>(MenuOption::Exit);
}

// Function to display the menu to the user
int displayMenuOptions() {
#ifdef UNIT_TESTING
    int choice;
    do {
        cout << "===== Main Menu =====" << endl;
        cout << static_cast<int>(MenuOption::FilterByGenre) << ". Filter by Genre" << endl;
        cout << static_cast<int>(MenuOption::FilterByState) << ". Filter by State" << endl;
        cout << static_cast<int>(MenuOption::FilterByCity) << ". Filter by City" << endl;
        cout << static_cast<int>(MenuOption::FilterByCapacity) << ". Filter by Capacity" << endl;
        cout << static_cast<int>(MenuOption::ClearSelectedVenues) << ". Clear Selected Venues" << endl;
        cout << static_cast<int>(MenuOption::ViewSelectedVenues) << ". View Selected Venues" << endl;
        cout << static_cast<int>(MenuOption::ShowEmailSettings) << ". Show Email Settings" << endl;
        cout << static_cast<int>(MenuOption::ViewEditEmail) << ". View & Edit Email" << endl;
        cout << static_cast<int>(MenuOption::FinishAndSendEmail) << ". Finish & Send Emails" << endl;
        cout << static_cast<int>(MenuOption::Exit) << ". Exit VenueSender" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input, please enter a number." << endl;
        } else if (!isValidMenuChoice(choice)) {
            cout << "Invalid choice. Please enter a number between " 
                 << static_cast<int>(MenuOption::FilterByGenre) 
                 << " and " 
                 << static_cast<int>(MenuOption::Exit) << "." << endl;
        } else {
            break;
        }
    } while (true);

    return choice;
#else
    int choice;
    do {
        cout << "===== Main Menu =====" << endl;
        cout << static_cast<int>(MenuOption::FilterByGenre) << ". Filter by Genre" << endl;
        cout << static_cast<int>(MenuOption::FilterByState) << ". Filter by State" << endl;
        cout << static_cast<int>(MenuOption::FilterByCity) << ". Filter by City" << endl;
        cout << static_cast<int>(MenuOption::FilterByCapacity) << ". Filter by Capacity" << endl;
        cout << static_cast<int>(MenuOption::ClearSelectedVenues) << ". Clear Selected Venues" << endl;
        cout << static_cast<int>(MenuOption::ViewSelectedVenues) << ". View Selected Venues" << endl;
        cout << static_cast<int>(MenuOption::ShowEmailSettings) << ". Show Email Settings" << endl;
        cout << static_cast<int>(MenuOption::ViewEditEmail) << ". View & Edit Email" << endl;
        cout << static_cast<int>(MenuOption::FinishAndSendEmail) << ". Finish & Send Emails" << endl;
        cout << static_cast<int>(MenuOption::Exit) << ". Exit VenueSender" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input, please enter a number. Press return to retry..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press
        } else if (!isValidMenuChoice(choice)) {
            cout << "Invalid choice. Please enter a number between " 
                 << static_cast<int>(MenuOption::FilterByGenre) 
                 << " and " 
                 << static_cast<int>(MenuOption::Exit) << "." << endl;
            cout << "Press return to continue..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press
        } else {
            break;
        }
    } while (true);

    return choice;
#endif
}

void viewEmailSettings(bool useSSL, bool verifyPeer, bool verifyHost,
                       const string& senderEmail, const string& mailPassDecrypted,
                       int smtpPort, const string& smtpServer) {

#ifdef UNIT_TESTING
    cout << "===== Email Settings =====" << endl;
    cout << "SMTP Server: " << smtpServer << endl;
    cout << "SMTP Port: " << smtpPort << endl;
    cout << "Sender Email: " << senderEmail << endl;
    cout << "Mail Password: " << mailPassDecrypted << endl; // only user for testing, will be removed in production
    cout << "SSL: " << (useSSL ? "true" : "false") << endl;
    cout << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl;
    cout << "verifyHost: " << (verifyHost ? "true" : "false") << endl;
    cout << "===========================" << endl;
#else
    cout << "===== Email Settings =====" << endl;
    cout << "SMTP Server: " << smtpServer << endl;
    cout << "SMTP Port: " << smtpPort << endl;
    cout << "Sender Email: " << senderEmail << endl;
    cout << "Mail Password: " << mailPassDecrypted << endl; // only user for testing, will be removed in production
    cout << "SSL: " << (useSSL ? "true" : "false") << endl;
    cout << "verifyPeer: " << (verifyPeer ? "true" : "false") << endl;
    cout << "verifyHost: " << (verifyHost ? "true" : "false") << endl;
    cout << "===========================" << endl;
    cout << "Press return to return to Main Menu" << endl;
    cin.ignore();  // If there's a chance you might have used cin before this point
    ConsoleUtils::clearInputBuffer();
    cin.get();     // This will wait for a key press
#endif
}

// Function to display selected venues to the user
void displaySelectedVenues(const vector<SelectedVenue>& selectedVenues) {
#ifdef UNIT_TESTING
    cout << "===== Selected Venues =====" << endl;
    if (selectedVenues.empty()) {
        cout << "No venues selected yet." << endl;
    } else {
        for (const auto& venue : selectedVenues) {
            cout << "Name: " << venue.name << endl;
            cout << "Email: " << venue.email << endl;
            cout << "City: " << venue.city << endl;
            cout << "--------------------------" << endl;
        }
    }
    cout << "===========================" << endl;
#else
    cout << "===== Selected Venues =====" << endl;
    if (selectedVenues.empty()) {
        cout << "No venues selected yet." << endl;
    } else {
        for (const auto& venue : selectedVenues) {
            cout << "Name: " << venue.name << endl;
            cout << "Email: " << venue.email << endl;
            cout << "City: " << venue.city << endl;
            cout << "--------------------------" << endl;
        }
    }
    cout << "===========================" << endl;
    cout << "Press return to return to Main Menu" << endl;
    cin.ignore();  // If there's a chance you might have used cin before this point
    ConsoleUtils::clearInputBuffer();
    cin.get();     // This will wait for a key press
#endif
}