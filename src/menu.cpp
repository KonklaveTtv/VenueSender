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
        std::cout << "===== Main Menu =====" << std::endl;
        std::cout << "1. Filter by Genre" << std::endl;
        std::cout << "2. Filter by State" << std::endl;
        std::cout << "3. Filter by City" << std::endl;
        std::cout << "4. Filter by Capacity" << std::endl;
        std::cout << "5. Clear Selected Venues" << std::endl;
        std::cout << "6. View Selected Venues" << std::endl;
        std::cout << "7. Show Email Settings" << std::endl;
        std::cout << "8. Finish & Send Emails" << std::endl;
        std::cout << "9. Exit VenueSender" << std::endl;
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            clearInputBuffer();
        } else if (!isValidMenuChoice(choice)) {
            std::cout << "Invalid choice. Please enter a number between 1 and 9." << std::endl;
            clearInputBuffer();
        } else {
            break;
        }
    } while (true);

    return choice;
}

void viewEmailSettings(const std::string& smtpServer, int smtpPort, const std::string& senderEmail,
                       int senderSmtpPort, const std::string& smtpPassDecrypted, const std::string& mailPassDecrypted) {
    std::cout << "===== Email Settings =====" << std::endl;
    std::cout << "SMTP Server: " << smtpServer << std::endl;
    std::cout << "SMTP Port: " << smtpPort << std::endl;
    std::cout << "Sender Email: " << senderEmail << std::endl;
    std::cout << "Sender SMTP Port: " << senderSmtpPort << std::endl;
    std::cout << "SMTP Password: " << smtpPassDecrypted << std::endl;
    std::cout << "Mail Password: " << mailPassDecrypted << std::endl;  
    std::cout << "===========================" << std::endl;
}

// Function to display selected venues to the user
void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues) {
    std::cout << "===== Selected Venues =====" << std::endl;
    if (selectedVenues.empty()) {
        std::cout << "No venues selected yet." << std::endl;
    } else {
        for (const auto& venue : selectedVenues) {
            std::cout << "Name: " << venue.name << std::endl;
            std::cout << "Email: " << venue.email << std::endl;
            std::cout << "City: " << venue.city << std::endl;
            std::cout << "--------------------------" << std::endl;
        }
    }
    std::cout << "===========================" << std::endl;
}