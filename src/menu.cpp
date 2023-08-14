#include "menu.h"



bool isValidMenuChoice(int choice) {
    // Validate if the choice is within valid menu options
    return choice >= static_cast<int>(MenuOption::FilterByGenre) &&
           choice <= static_cast<int>(MenuOption::Exit);
}

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
            std::cin.clear();
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