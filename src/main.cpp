#include "filtercriteria.h"
#include "venueutils.h"
#include "venue.h"
#include "venuesender.h"

#include <iostream>
#include <algorithm>
#include <limits>
#include <map>
#include <vector>
#include <sstream>

#include <curl/curl.h>

// Function to validate the user's choice
bool isValidMenuChoice(int choice) {
    return choice >= static_cast<int>(MenuOption::FilterByGenre) && choice <= static_cast<int>(MenuOption::Exit);
}

int getCapacityFromUser() {
    int capacity;
    std::cout << "Enter the venue capacity: ";
    std::cin >> capacity;
    return capacity;
}

// Convert Venue to SelectedVenue
SelectedVenue convertToSelectedVenue(const Venue& venue) {
    SelectedVenue selectedVenue;
    selectedVenue.name = venue.name;
    selectedVenue.email = venue.email;
    selectedVenue.city = venue.city;
    selectedVenue.genre = venue.genre;
    selectedVenue.state = venue.state;
    selectedVenue.capacity = venue.capacity;
    return selectedVenue;
}

int getGenreIndexFromName(const std::string& genreName, const std::set<std::string>& uniqueGenres) {
    int index = 1;
    for (const std::string& genre : uniqueGenres) {
        if (genre == genreName) {
            return index;
        }
        ++index;
    }
    return -1; // Return an invalid index if genreName is not found
}

int getStateIndexFromName(const std::string& stateName, const std::set<std::string>& uniqueStates) {
    int index = 1;
    for (const std::string& state : uniqueStates) {
        if (state == stateName) {
            return index;
        }
        ++index;
    }
    return -1; // Return an invalid index if stateName is not found
}

int getCityIndexFromName(const std::string& cityName, const std::set<std::string>& uniqueCities) {
    int index = 1;
    for (const std::string& city : uniqueCities) {
        if (city == cityName) {
            return index;
        }
        ++index;
    }
    return -1; // Return an invalid index if cityName is not found
}

// Function to filter venues based on the selected criteria
std::vector<SelectedVenue> filterVenues(const std::vector<Venue>& venues, const FilterCriteria& criteria) {
    std::vector<SelectedVenue> filteredVenues;

    for (const auto& venue : venues) {
        // Check if the genre matches the selected genre (if selected)
        if (criteria.filterByGenre && !criteria.genre.empty() && venue.genre != criteria.genre) {
            continue;
        }

        // Check if the state matches the selected state (if selected)
        if (criteria.filterByState && !criteria.state.empty() && venue.state != criteria.state) {
            continue;
        }

        // Check if the city matches the selected city (if selected)
        if (criteria.filterByCity && !criteria.city.empty() && venue.city != criteria.city) {
            continue;
        }

        // Check if the capacity matches the selected capacity (if selected)
        if (criteria.filterByCapacity && criteria.capacity != 0 && venue.capacity != criteria.capacity) {
            continue;
        }

        // If all criteria match, add the venue to the filteredVenues vector
        filteredVenues.push_back(convertToSelectedVenue(venue));
    }

    return filteredVenues;
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

void filterByGenre(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues) {
    std::cout << "===== Filter By Genre =====" << std::endl;

    // Extract unique genres from the venue data
    std::set<std::string> uniqueGenres;
    for (const Venue& venue : venues) {
        uniqueGenres.insert(venue.genre);
    }

    // Display unique genres to the user
    int genreIndex = 1;
    for (const std::string& genre : uniqueGenres) {
        std::cout << genreIndex << ". " << genre << std::endl;
        ++genreIndex;
    }

    // Get user input for genre choice
    int chosenGenreIndex;
    std::cout << "Enter the genre number to filter by: ";
    std::cin >> chosenGenreIndex;

    // Filter venues by chosen genre and add them to selectedVenuesForEmail
    const std::string& chosenGenre = *std::next(uniqueGenres.begin(), chosenGenreIndex - 1);
    for (const Venue& venue : venues) {
        if (venue.genre == chosenGenre) {
            SelectedVenue selectedVenue;
            selectedVenue.name = venue.name;
            selectedVenue.city = venue.city;
            selectedVenue.state = venue.state;
            selectedVenue.genre = venue.genre;
            selectedVenuesForEmail.push_back(selectedVenue);
        }
    }
}

void filterByState(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues) {
    std::cout << "===== Filter By State =====" << std::endl;

    // Extract unique states from the venue data
    std::set<std::string> uniqueStates;
    for (const Venue& venue : venues) {
        uniqueStates.insert(venue.state);
    }

    // Display unique states to the user
    int stateIndex = 1;
    for (const std::string& state : uniqueStates) {
        std::cout << stateIndex << ". " << state << std::endl;
        ++stateIndex;
    }

    // Get user's selected state indices
    std::cout << "Enter the indices of states to filter by (comma-separated, e.g., 1,2,3): ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string stateIndexStr;
    std::vector<int> selectedIndices;
    while (std::getline(iss, stateIndexStr, ',')) {
        int stateIndex = std::stoi(stateIndexStr);
        if (stateIndex >= 1 && stateIndex <= static_cast<int>(uniqueStates.size())) {
            selectedIndices.push_back(stateIndex);
        }
    }

    // Filter venues based on selected states
    std::vector<SelectedVenue> filteredVenuesByState;
    for (const Venue& venue : venues) {
        int stateIndex = getStateIndexFromName(venue.state, uniqueStates);
        if (std::find(selectedIndices.begin(), selectedIndices.end(), stateIndex) != selectedIndices.end()) {
            filteredVenuesByState.push_back(convertToSelectedVenue(venue));
        }
    }

    // Display filtered venues and let the user select venues to add to selectedVenuesForEmail
    std::vector<SelectedVenue> selectedVenues = selectVenuesFromFilteredResults(filteredVenuesByState);

    // Get user's selected indices to add to selectedVenuesForEmail
    std::cout << "Enter the indices of venues to add (comma-separated, e.g., 1,2,3): ";
    std::getline(std::cin, input);
    iss.clear();
    iss.str(input);
    std::string venueIndexStr;
    while (std::getline(iss, venueIndexStr, ',')) {
        int venueIndex = std::stoi(venueIndexStr);
        if (venueIndex >= 1 && venueIndex <= static_cast<int>(selectedVenues.size())) {
            selectedVenuesForEmail.push_back(selectedVenues[static_cast<size_t>(venueIndex) - 1]);
        }
    }
}

void filterByCity(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues) {
    std::cout << "===== Filter By City =====" << std::endl;

    // Extract unique cities from the venue data
    std::set<std::string> uniqueCities;
    for (const Venue& venue : venues) {
        uniqueCities.insert(venue.city);
    }

    // Display unique cities to the user
    int cityIndex = 1;
    for (const std::string& city : uniqueCities) {
        std::cout << cityIndex << ". " << city << std::endl;
        ++cityIndex;
    }

    // Get user's selected city indices
    std::cout << "Enter the indices of cities to filter by (comma-separated, e.g., 1,2,3): ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string cityIndexStr;
    std::vector<int> selectedIndices;
    while (std::getline(iss, cityIndexStr, ',')) {
        int cityIndex = std::stoi(cityIndexStr);
        if (cityIndex >= 1 && cityIndex <= static_cast<int>(uniqueCities.size())) {
            selectedIndices.push_back(cityIndex);
        }
    }

    // Filter venues based on selected cities
    std::vector<SelectedVenue> filteredVenuesByCity;
    for (const Venue& venue : venues) {
        int cityIndex = getStateIndexFromName(venue.city, uniqueCities);
        if (std::find(selectedIndices.begin(), selectedIndices.end(), cityIndex) != selectedIndices.end()) {
            filteredVenuesByCity.push_back(convertToSelectedVenue(venue));
        }
    }

    // Display filtered venues and let the user select venues to add to selectedVenuesForEmail
    std::vector<SelectedVenue> selectedVenues = selectVenuesFromFilteredResults(filteredVenuesByCity);

    // Get user's selected indices to add to selectedVenuesForEmail
    std::cout << "Enter the indices of venues to add (comma-separated, e.g., 1,2,3): ";
    std::getline(std::cin, input);
    iss.clear();
    iss.str(input);
    std::string venueIndexStr;
    while (std::getline(iss, venueIndexStr, ',')) {
        int venueIndex = std::stoi(venueIndexStr);
        if (venueIndex >= 1 && venueIndex <= static_cast<int>(selectedVenues.size())) {
            selectedVenuesForEmail.push_back(selectedVenues[static_cast<size_t>(venueIndex) - 1]);
        }
    }
}

void filterByCapacity(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues, const std::vector<int>& uniqueCapacities) {
    std::cout << "===== Filter By Capacity =====" << std::endl;

    // Display unique capacities to the user
    for (size_t i = 0; i < uniqueCapacities.size(); ++i) {
        std::cout << i + 1 << ". " << uniqueCapacities[i] << std::endl;
    }

    // Get user's selected capacity indices
    std::cout << "Enter the indices of capacities to filter by (comma-separated, e.g., 1,2,3): ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string capacityIndexStr;
    std::vector<int> selectedIndices;
    while (std::getline(iss, capacityIndexStr, ',')) {
        int capacityIndex = std::stoi(capacityIndexStr);
        if (capacityIndex >= 1 && capacityIndex <= static_cast<int>(uniqueCapacities.size())) {
            selectedIndices.push_back(capacityIndex - 1); // Adjust the index to match the vector indices
        }
    }

    // Filter venues based on selected capacities
    std::vector<SelectedVenue> filteredVenuesByCapacity;
    for (const Venue& venue : venues) {
        if (std::find(selectedIndices.begin(), selectedIndices.end(), venue.capacity) != selectedIndices.end()) {
            filteredVenuesByCapacity.push_back(convertToSelectedVenue(venue));
        }
    }

    // Display filtered venues and let the user select venues to add to selectedVenuesForEmail
    std::vector<SelectedVenue> selectedVenues = selectVenuesFromFilteredResults(filteredVenuesByCapacity);

    // Get user's selected indices to add to selectedVenuesForEmail
    std::cout << "Enter the indices of venues to add (comma-separated, e.g., 1,2,3): ";
    std::getline(std::cin, input);
    iss.clear();
    iss.str(input);
    std::string venueIndexStr;
    while (std::getline(iss, venueIndexStr, ',')) {
        int venueIndex = std::stoi(venueIndexStr);
        if (venueIndex >= 1 && venueIndex <= static_cast<int>(selectedVenues.size())) {
            selectedVenuesForEmail.push_back(selectedVenues[static_cast<size_t>(venueIndex) - 1]);
        }
    }
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

// Utility function to get unique genres from a vector of venues
std::vector<std::string> getUniqueGenres(const std::vector<Venue>& venues) {
    std::vector<std::string> uniqueGenres;
    for (const auto& venue : venues) {
        if (std::find(uniqueGenres.begin(), uniqueGenres.end(), venue.genre) == uniqueGenres.end()) {
            uniqueGenres.push_back(venue.genre);
        }
    }
    return uniqueGenres;
}

// Utility function to get unique states from a vector of venues
std::vector<std::string> getUniqueStates(const std::vector<Venue>& venues) {
    std::vector<std::string> uniqueStates;
    for (const auto& venue : venues) {
        if (std::find(uniqueStates.begin(), uniqueStates.end(), venue.state) == uniqueStates.end()) {
            uniqueStates.push_back(venue.state);
        }
    }
    return uniqueStates;
}

// Utility function to get unique cities from a vector of venues
std::vector<std::string> getUniqueCities(const std::vector<Venue>& venues) {
    std::vector<std::string> uniqueCities;
    for (const auto& venue : venues) {
        if (std::find(uniqueCities.begin(), uniqueCities.end(), venue.city) == uniqueCities.end()) {
            uniqueCities.push_back(venue.city);
        }
    }
    return uniqueCities;
}

// Utility function to get unique capacities from a vector of venues
std::vector<int> getUniqueCapacities(const std::vector<Venue>& venues) {
    std::vector<int> uniqueCapacities;
    for (const auto& venue : venues) {
        if (std::find(uniqueCapacities.begin(), uniqueCapacities.end(), venue.capacity) == uniqueCapacities.end()) {
            uniqueCapacities.push_back(venue.capacity);
        }
    }
    return uniqueCapacities;
}

int main() {
    // Initialize
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
    std::vector<std::string> uniqueGenres = getUniqueValues(venues, &Venue::genre);
    std::vector<std::string> uniqueStates = getUniqueValues(venues, &Venue::state);
    std::vector<std::string> uniqueCities = getUniqueValues(venues, &Venue::city);
    std::vector<int> uniqueCapacities = getUniqueValues(venues, &Venue::capacity);

    // Print the unique values for verification
    std::cout << "Unique Genres: ";
    for (const auto& genre : uniqueGenres) {
        std::cout << genre << " ";
    }
    std::cout << std::endl;

    std::cout << "Unique States: ";
    for (const auto& state : uniqueStates) {
        std::cout << state << " ";
    }
    std::cout << std::endl;

    std::cout << "Unique Cities: ";
    for (const auto& city : uniqueCities) {
        std::cout << city << " ";
    }
    std::cout << std::endl;

    std::cout << "Unique Capacities: ";
    for (const auto& capacity : uniqueCapacities) {
        std::cout << capacity << " ";
    }
    std::cout << std::endl;

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

while (true) {
    int choice = displayMenuOptions(); // Get the user's choice

    if (choice == static_cast<int>(MenuOption::FilterByGenre) ||
        choice == static_cast<int>(MenuOption::FilterByState) ||
        choice == static_cast<int>(MenuOption::FilterByCity) ||
        choice == static_cast<int>(MenuOption::FilterByCapacity)) {
       
        // Filter by Genre, State, City, or Capacity
        std::vector<std::string> filterOptions;

        if (choice == static_cast<int>(MenuOption::FilterByGenre)) {
            filterOptions = uniqueGenres;
        } else if (choice == static_cast<int>(MenuOption::FilterByState)) {
            filterOptions = uniqueStates;
        } else if (choice == static_cast<int>(MenuOption::FilterByCity)) {
            filterOptions = uniqueCities;
        } else if (choice == static_cast<int>(MenuOption::FilterByCapacity)) {
            std::cout << "Available Capacities: ";
            for (const auto& capacity : uniqueCapacities) {
                std::cout << capacity << " ";
            }
            std::cout << std::endl;
            int capacity = getCapacityFromUser();
            criteria.filterByCapacity = true;
            criteria.capacity = capacity;
        }

        std::cout << "Available Options: ";
        for (size_t i = 0; i < filterOptions.size(); ++i) {
            std::cout << i + 1 << ". " << filterOptions[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "Enter the indices of options (comma-separated): ";
        std::string input;
        std::getline(std::cin, input);

        std::vector<int> selectedIndices;
        std::stringstream ss(input);
        int index;
        while (ss >> index) {
            selectedIndices.push_back(index);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }

            if (choice == static_cast<int>(MenuOption::FilterByGenre)) {
                filterOptions = uniqueGenres;

                std::cout << "Available Options: ";
                for (size_t i = 0; i < filterOptions.size(); ++i) {
                    std::cout << i + 1 << ". " << filterOptions[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Enter the indices of options (comma-separated): ";
                std::string input;
                std::getline(std::cin, input);

                std::vector<int> selectedIndices;
                std::istringstream iss(input);
                std::string indexStr;
                while (std::getline(iss, indexStr, ',')) {
                    int index = std::stoi(indexStr);
                    if (index >= 1 && index <= static_cast<int>(filterOptions.size())) {
                        selectedIndices.push_back(index - 1); // Adjust the index to match the vector indices
                    }
                }

                criteria.filterByGenre = true;
                for (int selectedIndex : selectedIndices) {
                    criteria.genre = filterOptions[selectedIndex];
                    selectedVenuesForEmail = filterVenues(venues, criteria);
                }

                displayFilteredVenues(selectedVenuesForEmail);
        } else if (choice == static_cast<int>(MenuOption::FilterByState)) {
                filterOptions = uniqueStates;

                std::cout << "Available Options: ";
                for (size_t i = 0; i < filterOptions.size(); ++i) {
                    std::cout << i + 1 << ". " << filterOptions[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Enter the indices of options (comma-separated): ";
                std::string input;
                std::getline(std::cin, input);

                std::vector<int> selectedIndices;
                std::istringstream iss(input);
                std::string indexStr;
                while (std::getline(iss, indexStr, ',')) {
                    int index = std::stoi(indexStr);
                    if (index >= 1 && index <= static_cast<int>(filterOptions.size())) {
                        selectedIndices.push_back(index - 1); // Adjust the index to match the vector indices
                    }
                }

                criteria.filterByState = true;
                for (int selectedIndex : selectedIndices) {
                    criteria.state = filterOptions[selectedIndex];
                    selectedVenuesForEmail = filterVenues(venues, criteria);
                }

                displayFilteredVenues(selectedVenuesForEmail);
        } else if (choice == static_cast<int>(MenuOption::FilterByCity)) {
                filterOptions = uniqueCities;

                std::cout << "Available Options: ";
                for (size_t i = 0; i < filterOptions.size(); ++i) {
                    std::cout << i + 1 << ". " << filterOptions[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "Enter the indices of options (comma-separated): ";
                std::string input;
                std::getline(std::cin, input);

                std::vector<int> selectedIndices;
                std::istringstream iss(input);
                std::string indexStr;
                while (std::getline(iss, indexStr, ',')) {
                    int index = std::stoi(indexStr);
                    if (index >= 1 && index <= static_cast<int>(filterOptions.size())) {
                        selectedIndices.push_back(index - 1); // Adjust the index to match the vector indices
                    }
                }

                criteria.filterByCity = true;
                for (int selectedIndex : selectedIndices) {
                    criteria.city = filterOptions[selectedIndex];
                    selectedVenuesForEmail = filterVenues(venues, criteria);
                }

                displayFilteredVenues(selectedVenuesForEmail);
        } else if (choice == static_cast<int>(MenuOption::FilterByCapacity)) {
            std::cout << "Available Capacities: ";
            for (const auto& capacity : uniqueCapacities) {
                std::cout << capacity << " ";
            }
            std::cout << std::endl;
            
            // Get user's selected capacity indices
            std::cout << "Enter the indices of capacities to filter by (comma-separated): ";
            std::string input;
            std::getline(std::cin, input);
            std::istringstream iss(input);
            std::string capacityIndexStr;
            std::vector<int> selectedCapacities;
            while (std::getline(iss, capacityIndexStr, ',')) {
                int capacityIndex = std::stoi(capacityIndexStr);
                if (capacityIndex >= 1 && capacityIndex <= static_cast<int>(uniqueCapacities.size())) {
                    selectedCapacities.push_back(uniqueCapacities[capacityIndex - 1]);
                }
            }

            // Filter venues based on selected capacities
            criteria.filterByCapacity = true;
            criteria.capacity = 0; // Reset capacity filter
            std::vector<SelectedVenue> filteredVenues;
            for (int selectedCapacity : selectedCapacities) {
                criteria.capacity = selectedCapacity;
                std::vector<SelectedVenue> capacityFilteredVenues = filterVenues(venues, criteria);
                filteredVenues.insert(filteredVenues.end(), capacityFilteredVenues.begin(), capacityFilteredVenues.end());
            }

            // Display filtered venues and let the user select venues to add to selectedVenuesForEmail
            std::vector<SelectedVenue> selectedVenues = selectVenuesFromFilteredResults(filteredVenues);

            // Add selected venues to selectedVenuesForEmail
            selectedVenuesForEmail.insert(selectedVenuesForEmail.end(), selectedVenues.begin(), selectedVenues.end());
        }

        displayFilteredVenues(selectedVenuesForEmail);
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
