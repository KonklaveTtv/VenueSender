#include "filtercriteria.h"
#include "venueutils.h"
#include "venue.h"
#include "venuesender.h"

#include <limits>
#include <map>

#include <curl/curl.h>

// Function to validate the user's choice
bool isValidMenuChoice(int choice) {
    // Validate if the choice is within valid menu options
    return choice >= static_cast<int>(MenuOption::FilterByGenre) && choice <= static_cast<int>(MenuOption::Exit);
}

// Convert Venue to SelectedVenue
SelectedVenue convertToSelectedVenue(const Venue& venue) {
    // Create a SelectedVenue instance based on Venue data
    SelectedVenue selectedVenue;
    selectedVenue.name = venue.name;
    selectedVenue.email = venue.email;
    selectedVenue.city = venue.city;
    selectedVenue.genre = venue.genre;
    selectedVenue.state = venue.state;
    selectedVenue.capacity = venue.capacity;
    return selectedVenue;
}

// Function to display filtered venues to the user
void displayFilteredVenues(const std::vector<SelectedVenue>& selectedVenuesForDisplay) {
    if (selectedVenuesForDisplay.empty()) {
        std::cout << "No venues found." << std::endl;
        return;
    }

    std::cout << "Filtered Venues: " << std::endl;
    for (const auto& venue : selectedVenuesForDisplay) {
        std::cout << "Name: " << venue.name << std::endl;
        std::cout << "Email: " << venue.email << std::endl;
        std::cout << "City: " << venue.city << std::endl;
        std::cout << "Capacity: " << venue.capacity << std::endl;
        // Add any other members you want to display here
        std::cout << std::endl;
    }
}

// Function to display the menu options
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
        std::cout << "7. Finish & Send Emails" << std::endl;
        std::cout << "8. Exit VenueSender" << std::endl;
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice) || !isValidMenuChoice(choice)) {
            std::cout << "Invalid choice. Please enter a number between 1 and 8." << std::endl;
            std::cin.clear();
            clearInputBuffer();
        } else {
            break;
        }
    } while (true);

    return choice;
}

std::vector<SelectedVenue> filterByOption(const std::vector<Venue>& venues,
                                          const std::string& filterType,
                                          const std::set<std::string>& uniqueOptions,
                                          std::vector<SelectedVenue>& temporaryFilteredVenues) {
    std::vector<std::string> filterOptions(uniqueOptions.begin(), uniqueOptions.end());
    std::cout << "===== Filter By " << filterType << " =====" << std::endl;

    std::cout << "Available Options: " << std::endl;
    for (size_t i = 0; i < filterOptions.size(); ++i) {
        std::cout << i + 1 << ". " << filterOptions[i] << std::endl;
    }

    std::cout << "Enter comma-separated indices of options to select: ";
    std::string input;
    std::cin.ignore();
    std::getline(std::cin, input);

    std::vector<size_t> selectedIndices;
    std::istringstream iss(input);
    std::string indexStr;
    while (std::getline(iss, indexStr, ',')) {
        size_t selectedIndex = std::stoi(indexStr) - 1;
        if (selectedIndex < filterOptions.size()) {
            selectedIndices.push_back(selectedIndex);
        } else {
            std::cout << "Invalid index: " << selectedIndex + 1 << ". Skipping." << std::endl;
        }
    }

    for (size_t selectedIndex : selectedIndices) {
        if (selectedIndex < filterOptions.size()) {
            std::string filterValue = filterOptions[selectedIndex];
            for (const Venue& venue : venues) {
                if ((filterType == "Genre" && venue.genre == filterValue) ||
                    (filterType == "State" && venue.state == filterValue) ||
                    (filterType == "City" && venue.city == filterValue)) {
                    SelectedVenue selectedVenue = convertToSelectedVenue(venue);
                    temporaryFilteredVenues.push_back(selectedVenue);
                }
            }
        } else {
            std::cout << "Invalid index: " << selectedIndex + 1 << ". Skipping." << std::endl;
        }
    }

    return temporaryFilteredVenues;
}

std::vector<SelectedVenue> filterByCapacity(const std::vector<Venue>& venues,
                                            const std::set<int>& uniqueCapacities,
                                            std::vector<SelectedVenue>& temporaryFilteredVenues) {
    std::vector<int> filterOptions(uniqueCapacities.begin(), uniqueCapacities.end());
    std::cout << "===== Filter By Capacity =====" << std::endl;

    std::cout << "Available Options: " << std::endl;
    for (size_t i = 0; i < filterOptions.size(); ++i) {
        std::cout << i + 1 << ". " << filterOptions[i] << std::endl;
    }

    std::cout << "Enter comma-separated indices of options to select: ";
    std::string input;
    std::cin.ignore();
    std::getline(std::cin, input);

    std::vector<size_t> selectedIndices;
    std::istringstream iss(input);
    std::string indexStr;
    while (std::getline(iss, indexStr, ',')) {
        size_t selectedIndex = std::stoi(indexStr) - 1;
        if (selectedIndex < filterOptions.size()) {
            selectedIndices.push_back(selectedIndex);
        } else {
            std::cout << "Invalid index: " << selectedIndex + 1 << ". Skipping." << std::endl;
        }
    }

    for (size_t selectedIndex : selectedIndices) {
        if (selectedIndex < filterOptions.size()) {
            int filterValue = filterOptions[selectedIndex];
            for (const Venue& venue : venues) {
                if (venue.capacity == filterValue) {
                    SelectedVenue selectedVenue = convertToSelectedVenue(venue);
                    temporaryFilteredVenues.push_back(selectedVenue);
                }
            }
        } else {
            std::cout << "Invalid index: " << selectedIndex + 1 << ". Skipping." << std::endl;
        }
    }

    return temporaryFilteredVenues;
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

int main() {
    // Initialize data and configuration settings
    std::vector<Venue> venues;
    std::string venuesCsvPath;
    std::string smtpServer;
    int smtpPort;
    std::string smtpPassword;
    std::string smtpUsername;
    std::string emailPassword;
    std::string senderEmail;
    int senderSmtpPort;
    std::string subject;
    std::string message;

    // Read data from CSV file and load configuration settings
    if (!loadConfigSettings(smtpServer, smtpPort, smtpUsername, smtpPassword, venuesCsvPath, emailPassword, senderEmail, senderSmtpPort)) {
        std::cerr << "Failed to load configuration settings from config.json." << std::endl;
        return 1;
    }
    readCSV(venues, venuesCsvPath);

    // Extract unique genres, states, cities, and capacities from the venues data
    std::set<std::string> uniqueGenres = getUniqueGenres(venues);
    std::set<std::string> uniqueStates = getUniqueStates(venues);
    std::set<std::string> uniqueCities = getUniqueCities(venues);
    std::set<int> uniqueCapacities = getUniqueCapacities(venues);

    // Get user's email credentials and SMTP settings
    getUserEmailSettings(emailPassword, smtpServer, smtpPort, senderEmail, senderSmtpPort); // Remove smtpUsername and smtpPassword from this line

    // Create a libcurl easy handle for sending emails
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return 1;
    }

    // Connect to the SMTP server
    CURLcode res;
    std::string smtpUrl = "smtp://" + smtpServer + ":" + std::to_string(smtpPort);
    res = curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());
    if (res != CURLE_OK) {
        std::cerr << "Failed to set libcurl URL option." << std::endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Set the sender email address
    res = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());
    if (res != CURLE_OK) {
        std::cerr << "Failed to set libcurl sender email option." << std::endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Set the email password for authentication
    res = curl_easy_setopt(curl, CURLOPT_PASSWORD, emailPassword.c_str());
    if (res != CURLE_OK) {
        std::cerr << "Failed to set libcurl email password option." << std::endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    FilterCriteria criteria;
    std::vector<SelectedVenue> selectedVenuesForEmail;
    std::vector<SelectedVenue> filteredVenues;

    // Main loop for interacting with the user
    while (true) {
        // Display menu options and get user's choice
        int choice = displayMenuOptions();

            // Handle menu choices
            if (choice >= static_cast<int>(MenuOption::FilterByGenre) &&
                choice <= static_cast<int>(MenuOption::FilterByCapacity)) {
                // Declare a temporary vector to store filtered venues
                std::vector<SelectedVenue> temporaryFilteredVenues;

                // Handle filtering options
                if (choice == static_cast<int>(MenuOption::FilterByGenre)) {
                    // Filter by Genre
                    temporaryFilteredVenues = filterByOption(venues, "Genre", uniqueGenres, temporaryFilteredVenues);
                } else if (choice == static_cast<int>(MenuOption::FilterByState)) {
                    // Filter by State
                    temporaryFilteredVenues = filterByOption(venues, "State", uniqueStates, temporaryFilteredVenues);
                } else if (choice == static_cast<int>(MenuOption::FilterByCity)) {
                    // Filter by City
                    temporaryFilteredVenues = filterByOption(venues, "City", uniqueCities, temporaryFilteredVenues);
                } else if (choice == static_cast<int>(MenuOption::FilterByCapacity)) {
                    // Filter by Capacity
                    temporaryFilteredVenues = filterByCapacity(venues, uniqueCapacities, temporaryFilteredVenues);
                }

            // Display filtered venues
            displayFilteredVenues(temporaryFilteredVenues);

            // Allow user to select venues to add to selectedVenuesForEmail
            std::cout << "Select venues to add (comma-separated indices): ";
            std::string input;
            std::getline(std::cin, input);

            std::istringstream iss(input);
            std::string indexStr;
            while (std::getline(iss, indexStr, ',')) {
                size_t selectedIndex = std::stoi(indexStr) - 1;
                if (selectedIndex < filteredVenues.size()) {
                    selectedVenuesForEmail.push_back(filteredVenues[selectedIndex]);
                } else {
                    std::cout << "Invalid index: " << selectedIndex + 1 << ". Skipping." << std::endl;
                }
            }
        } else if (choice == static_cast<int>(MenuOption::ViewSelectedVenues)) {
            // View Selected Venues
            displaySelectedVenues(selectedVenuesForEmail);
        } else if (choice == static_cast<int>(MenuOption::ClearSelectedVenues)) {
            // Clear Selected Venues
            selectedVenuesForEmail.clear();
            std::cout << "Selected venues cleared." << std::endl;
        } else if (choice == static_cast<int>(MenuOption::FinishAndSendEmails)) {
            // Finish and Send Emails

            // Check if selectedVenuesForEmail is empty
            if (selectedVenuesForEmail.empty()) {
                std::cout << "No venues selected. Please add venues before sending emails." << std::endl;
                continue; // Return to the main menu
            }

            // Check if subject and message are empty
            if (subject.empty() || message.empty()) {
                std::cout << "Subject and Message are required. Please set them before sending emails." << std::endl;
                getEmailSubjectAndMessage(subject, message); // Prompt the user to enter the subject and message
                continue; // Return to the main menu
            }
            
            // Prompt for confirmation
            std::cout << "Confirm sending the email (Y/N): ";
            char confirmSend;
            std::cin >> confirmSend;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (confirmSend == 'Y' || confirmSend == 'y') {
                // Proceed to send emails if confirmed
                filteredVenues.insert(filteredVenues.end(), selectedVenuesForEmail.begin(), selectedVenuesForEmail.end());
                sendEmails(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort);
                filteredVenues.clear(); // Clear the filtered venues for the next round of emails

                // Reset subject and message after sending emails
                subject.clear();
                message.clear();
            } else if (confirmSend == 'N' || confirmSend == 'n') {
                std::cout << "Email not sent. Returning to the main menu." << std::endl;
                // The user chose not to send the email, return to the main menu without clearing the selectedVenuesForEmail vector
            } else {
                std::cout << "Invalid choice. Please try again." << std::endl;
                // The user entered an invalid choice, return to the main menu without clearing the selectedVenuesForEmail vector
            }
        } else if (choice == static_cast<int>(MenuOption::Exit)) {
        // Exit VenueSender
        std::cout << "Exiting the program." << std::endl;
        break;
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
    }
}

    // Clean up and exit
    curl_global_cleanup();
    return 0;
}
