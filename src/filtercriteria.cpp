#include "filtercriteria.h"
#include "venue.h"

// Utility function to get unique genres from a vector of venues
std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueGenres;
    for (const auto& venue : venues) {
        if (venue.genre != "all") {  // Exclude the "all" genre
            uniqueGenres.insert(venue.genre);
        }
    }
    return uniqueGenres;
}

// Utility function to get unique states from a vector of venues
std::set<std::string> getUniqueStates(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueStates;
    for (const auto& venue : venues) {
        if (uniqueStates.find(venue.state) == uniqueStates.end()) {
            uniqueStates.insert(venue.state);
        }
    }
    return uniqueStates;
}

// Utility function to get unique cities from a vector of venues
std::set<std::string> getUniqueCities(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueCities;
    for (const auto& venue : venues) {
        if (uniqueCities.find(venue.city) == uniqueCities.end()) {
            uniqueCities.insert(venue.city);
        }
    }
    return uniqueCities;
}

// Utility function to get unique capacities from a vector of venues
std::set<int> getUniqueCapacities(const std::vector<Venue>& venues) {
    std::set<int> uniqueCapacities;
    for (const Venue& venue : venues) {
        uniqueCapacities.insert(venue.capacity);
    }
    return uniqueCapacities;
}

// Utility function to get unique values of a member using a member pointer
std::vector<std::string> getUniqueValues(const std::vector<Venue>& venues, std::string Venue::* memberPtr) {
    std::vector<std::string> uniqueValues;
    for (const Venue& venue : venues) {
        std::string value = venue.*memberPtr;
        if (std::find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end()) {
            uniqueValues.push_back(value);
        }
    }
    return uniqueValues;
}

// Utility function to get unique values of a member using a member pointer
std::vector<int> getUniqueValues(const std::vector<Venue>& venues, int Venue::* memberPtr) {
    std::vector<int> uniqueValues;
    for (const Venue& venue : venues) {
        int value = venue.*memberPtr;
        if (std::find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end()) {
            uniqueValues.push_back(value);
        }
    }
    return uniqueValues;
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

// Function to process user input and select venues
void processVenueSelection(const std::vector<SelectedVenue>& temporaryFilteredVenues,
                           std::vector<SelectedVenue>& selectedVenuesForEmail) {
        if(temporaryFilteredVenues.empty()){
        return;
    }

    std::cout << "Select venues to add (comma-separated indices): ";
    std::string input;
    const int maxInputLength = 256;
    std::getline(std::cin, input);
    if (input.length() > maxInputLength) {
        std::cout << "Input too long. Please try again." << std::endl;
        return; // Or handle the error appropriately
    }

    std::istringstream iss(input);
    std::string indexStr;
    while (std::getline(iss, indexStr, ',')) {
        size_t selectedIndex;
        std::istringstream iss(indexStr);
        if (!(iss >> selectedIndex)) {
            std::cout << "Invalid index format. Skipping." << std::endl;
            continue; // Skip displaying genres if the index is invalid
        }
        selectedIndex--; // Decrement index to match 0-based indexing
        if (selectedIndex < temporaryFilteredVenues.size()) {
            selectedVenuesForEmail.push_back(temporaryFilteredVenues[selectedIndex]);
        } else {
            std::cout << "Invalid index: " << selectedIndex + 1 << ". Skipping." << std::endl;
            continue; // Skip displaying genres if the index is invalid
        }
    }
    // Add a newline to separate the filtered venues from the main menu
    std::cout << std::endl;
}

// Function to display filtered venues to the user
void displayFilteredVenues(const std::vector<SelectedVenue>& selectedVenuesForDisplay) {
    if (selectedVenuesForDisplay.empty()) {
        std::cout << "No venues found." << std::endl;
        return;
    }

    std::cout << "Filtered Venues: " << std::endl;
    for (size_t i = 0; i < selectedVenuesForDisplay.size(); ++i) {
        const auto& venue = selectedVenuesForDisplay[i];
        std::cout << std::setw(2) << i + 1 << ". Name: " << venue.name << std::endl;
        std::cout << "   Email: " << venue.email << std::endl;
        std::cout << "   City: " << venue.city << std::endl;
        std::cout << "   Capacity: " << venue.capacity << std::endl;
        std::cout << std::endl;
    }
}

// Common function for filtering by an option (Genre, State, City)
std::vector<SelectedVenue> filterByOptionCommon(const std::vector<Venue>& venues,
                                                const std::set<std::string>& uniqueOptions,
                                                const std::string& filterType,
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

    std::cout << std::endl; // Add a line of space

    // Validate and process the user's input
    std::vector<size_t> selectedIndices;
    std::istringstream iss(input);
    std::string indexStr;
    while (std::getline(iss, indexStr, CSV_DELIMITER)) {
        try {
            size_t selectedIndex = std::stoi(indexStr) - 1;
            selectedIndices.push_back(selectedIndex);
        } catch (const std::exception& e) {
            std::cout << "Invalid index. Skipping." << std::endl;
        }
    }

    // Now you have the validated selectedIndices vector to work with
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
            std::cout << "Invalid index. Skipping." << std::endl;
        }
    }

    return temporaryFilteredVenues;
}

// Function to filter by Genre, State, or City
std::vector<SelectedVenue> filterByOption(const std::vector<Venue>& venues,
                                          const std::string& filterType,
                                          const std::set<std::string>& uniqueOptions,
                                          std::vector<SelectedVenue>& temporaryFilteredVenues) {
    return filterByOptionCommon(venues, uniqueOptions, filterType, temporaryFilteredVenues);
}

// Function to filter by Capacity
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

    std::cout << std::endl; // Add a line of space

    // Validate and process the user's input
    std::vector<size_t> selectedIndices;
    std::istringstream iss(input);
    std::string indexStr;
    while (std::getline(iss, indexStr, CSV_DELIMITER)) {
        try {
            size_t selectedIndex = std::stoi(indexStr) - 1;
            selectedIndices.push_back(selectedIndex);
        } catch (const std::exception& e) {
            std::cout << "Invalid index format. Skipping." << std::endl;
        }
    }

    // Now you have the validated selectedIndices vector to work with
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
            std::cout << "Invalid index: " << selectedIndex << std::endl;
        }
    }

    return temporaryFilteredVenues;
}