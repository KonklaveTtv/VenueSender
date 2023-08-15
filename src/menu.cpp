#include "menu.h"

// Function to validate user input
bool isValidMenuChoice(int choice) {
    // Validate if the choice is within valid menu options
    return choice >= static_cast<int>(MenuOption::FilterByGenre) &&
           choice <= static_cast<int>(MenuOption::Exit);
}

// Function to display the menu to the user
int displayMenuOptions() {
    int choice;
    do {
        cout << "===== Main Menu =====" << endl;
        cout << "1. Filter by Genre" << endl;
        cout << "2. Filter by State" << endl;
        cout << "3. Filter by City" << endl;
        cout << "4. Filter by Capacity" << endl;
        cout << "5. Clear Selected Venues" << endl;
        cout << "6. View Selected Venues" << endl;
        cout << "7. Show Email Settings" << endl;
        cout << "8. Finish & Send Emails" << endl;
        cout << "9. Exit VenueSender" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number." << endl;
            clearInputBuffer();
        } else if (!isValidMenuChoice(choice)) {
            cout << "Invalid choice. Please enter a number between 1 and 9." << endl;
            clearInputBuffer();
        } else {
            break;
        }
    } while (true);

    return choice;
}

void viewEmailSettings(const string& smtpServer, int smtpPort, const string& senderEmail,
                       int senderSmtpPort, const string& smtpPassDecrypted, const string& mailPassDecrypted,
                       bool useSSL) {
    cout << "===== Email Settings =====" << endl;
    cout << "SMTP Server: " << smtpServer << endl;
    cout << "SMTP Port: " << smtpPort << endl;
    cout << "Sender Email: " << senderEmail << endl;
    cout << "Sender SMTP Port: " << senderSmtpPort << endl;
    cout << "SMTP Password: " << smtpPassDecrypted << endl; // only used for testing, will be removed in production
    cout << "Mail Password: " << mailPassDecrypted << endl; // only user for testing, will be removed in production
    cout << "SSL Enabled: " << (useSSL ? "true" : "false") << endl; 
    cout << "===========================" << endl;
}

// Function to display selected venues to the user
void displaySelectedVenues(const vector<SelectedVenue>& selectedVenues) {
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
}