#include "filtercriteria.h"

using namespace std;

// Convert Venue to SelectedVenue
SelectedVenue VenueUtilities::convertToSelectedVenue(const Venue& venue) {
    // Create a SelectedVenue instance based on Venue data
    SelectedVenue selectedVenue;
    selectedVenue.name = venue.name;
    selectedVenue.email = venue.email;
    selectedVenue.genre = venue.genre;
    selectedVenue.state = venue.state;
    selectedVenue.city = venue.city;
    selectedVenue.capacity = venue.capacity;
    return selectedVenue;
}

// Utility function to get unique genres from a vector of venues
set<string> VenueUtilities::getUniqueGenres(const vector<Venue>& venues) {
    set<string> uniqueGenres;
    for (const auto& venue : venues) {
        if (uniqueGenres.find(venue.genre) == uniqueGenres.end()) {
            uniqueGenres.insert(venue.genre);
        }
    }
    return uniqueGenres;
}

// Utility function to get unique states from a vector of venues
set<string> VenueUtilities::getUniqueStates(const vector<Venue>& venues) {
    set<string> uniqueStates;
    for (const auto& venue : venues) {
        if (uniqueStates.find(venue.state) == uniqueStates.end()) {
            uniqueStates.insert(venue.state);
        }
    }
    return uniqueStates;
}

// Utility function to get unique cities from a vector of venues
set<string> VenueUtilities::getUniqueCities(const vector<Venue>& venues) {
    set<string> uniqueCities;
    for (const auto& venue : venues) {
        if (uniqueCities.find(venue.city) == uniqueCities.end()) {
            uniqueCities.insert(venue.city);
        }
    }
    return uniqueCities;
}

// Utility function to get unique capacities from a vector of venues
set<int> VenueUtilities::getUniqueCapacities(const vector<Venue>& venues) {
    set<int> uniqueCapacities;
    for (const Venue& venue : venues) {
        uniqueCapacities.insert(venue.capacity);
    }
    return uniqueCapacities;
}

// Utility function to get unique values of a member using a member pointer
vector<string> VenueUtilities::getUniqueValues(const vector<Venue>& venues, string Venue::* memberPtr) {
    vector<string> uniqueValues;
    for (const Venue& venue : venues) {
        string value = venue.*memberPtr;
        if (find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end()) {
            uniqueValues.push_back(value);
        }
    }
    return uniqueValues;
}

// Utility function to get unique values of a member using a member pointer
vector<int> VenueUtilities::getUniqueValues(const vector<Venue>& venues, int Venue::* memberPtr) {
    vector<int> uniqueValues;
    for (const Venue& venue : venues) {
        int value = venue.*memberPtr;
        if (find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end()) {
            uniqueValues.push_back(value);
        }
    }
    return uniqueValues;
}

void VenueFilter::processVenueSelection(const vector<SelectedVenue>& temporaryFilteredVenues,
                           vector<SelectedVenue>& selectedVenuesForEmail,
                           istream& input,
                           ostream& output) {
    if (temporaryFilteredVenues.empty()) {
        return;
    }

    output << "Select venues to add (comma-separated indices): ";
    string userInput;
    getline(input, userInput);

    // Validate input length
    if (userInput.length() > MAX_INPUT_LENGTH) {
        output << "Input too long. Please try again." << endl;
        cout << "Press return to continue..." << endl;
        cin.ignore();  // If there's a chance you might have used cin before this point
        ConsoleUtils::clearInputBuffer();
        cin.get();     // This will wait for a key press
        return; // Or handle the error appropriately
    }

    istringstream iss(userInput);
    string indexStr;
    while (getline(iss, indexStr, ',')) {
        try {
            size_t selectedIndex = stoul(indexStr);
            if (selectedIndex == 0) {
                output << "Invalid index format. Skipping." << endl;
                cout << "Press return to continue..." << endl;
                cin.ignore();  // If there's a chance you might have used cin before this point
                ConsoleUtils::clearInputBuffer();
                cin.get();     // This will wait for a key press
                continue;
            }
            selectedIndex--; // Decrement index to match 0-based indexing
            if (selectedIndex < temporaryFilteredVenues.size()) {
                const SelectedVenue& selectedVenue = temporaryFilteredVenues[selectedIndex];

                // Check if the selected venue is already in the selectedVenuesForEmail vector
                bool isDuplicate = false;
                for (const SelectedVenue& existingVenue : selectedVenuesForEmail) {
                    if (existingVenue.name == selectedVenue.name && existingVenue.email == selectedVenue.email) {
                        isDuplicate = true;
                        break;
                    }
                }

                if (!isDuplicate) {
                    selectedVenuesForEmail.push_back(selectedVenue);
                } else {
                    output << "Venue already selected. Skipping." << endl;
                    cout << "Press return to continue..." << endl;
                    cin.ignore();  // If there's a chance you might have used cin before this point
                    ConsoleUtils::clearInputBuffer();
                    cin.get();     // This will wait for a key press
                }
            } else {
                output << "Invalid index: " << selectedIndex + 1 << ". Skipping." << endl;
                cout << "Press return to continue..." << endl;
                cin.ignore();  // If there's a chance you might have used cin before this point
                ConsoleUtils::clearInputBuffer();
                cin.get();     // This will wait for a key press  
                continue;
            }
        } catch (const invalid_argument& e) {
            output << "Invalid input. Skipping." << endl;
            cout << "Press return to continue..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press 
            continue;
        }
    }

    // Add a newline to separate the filtered venues from the main menu
    output << endl;
}

// Function to display filtered venues to the user
void VenueFilter::displayFilteredVenues(const vector<SelectedVenue>& selectedVenuesForDisplay) {
    if (selectedVenuesForDisplay.empty()) {
        cout << "No venues found." << endl;
        return;
    }
    ConsoleUtils::clearConsole();
    cout << "Filtered Venues: " << endl;
    for (size_t i = 0; i < selectedVenuesForDisplay.size(); ++i) {
        const auto& venue = selectedVenuesForDisplay[i];
        cout << setw(2) << i + 1 << ". Name: " << venue.name << endl;
        cout << "   Email: " << venue.email << endl;
        cout << "   City: " << venue.city << endl;
        cout << "   Capacity: " << venue.capacity << endl;
        cout << endl;
    }
}

// Common function for filtering by an option (Genre, State, City)
vector<SelectedVenue> VenueFilter::filterByOptionCommon(const vector<Venue>& venues,
                                                const set<string>& uniqueOptions,
                                                const string& filterType,
                                                vector<SelectedVenue>& temporaryFilteredVenues) {
    vector<string> filterOptions(uniqueOptions.begin(), uniqueOptions.end());
    ConsoleUtils::clearConsole();
    cout << "===== Filter By " << filterType << " =====" << endl;
    
    cout << "Available Options: " << endl;
    for (size_t i = 0; i < filterOptions.size(); ++i) {
        cout << i + 1 << ". " << filterOptions[i] << endl;
    }

    cout << "Enter comma-separated indices of options to select: ";
    string input;
    ConsoleUtils::clearInputBuffer();
    getline(cin, input);

    cout << endl; // Add a line of space

    // Validate and process the user's input
    vector<size_t> selectedIndices;
    istringstream iss(input);
    string indexStr;
    while (getline(iss, indexStr, CSV_DELIMITER)) {
        try {
            size_t selectedIndex = stoi(indexStr) - 1;
            selectedIndices.push_back(selectedIndex);
        } catch (const exception& e) {
            cout << "Invalid index. Skipping." << endl;
            cout << "Press return to continue..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press            
        }
    }

    // Now you have the validated selectedIndices vector to work with
    for (size_t selectedIndex : selectedIndices) {
        if (selectedIndex < filterOptions.size()) {
            string filterValue = filterOptions[selectedIndex];
            for (const Venue& venue : venues) {
                if ((filterType == "Genre" && venue.genre == filterValue) ||
                    (filterType == "State" && venue.state == filterValue) ||
                    (filterType == "City" && venue.city == filterValue)) {
                    SelectedVenue selectedVenue = VenueUtilities::convertToSelectedVenue(venue);
                    temporaryFilteredVenues.push_back(selectedVenue);
                }
            }
        } else {
            cout << "Invalid index. Skipping." << endl;
            cout << "Press return to continue..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press   
        }
    }

    return temporaryFilteredVenues;
}

// Function to filter by Genre, State, or City
vector<SelectedVenue> VenueFilter::filterByOption(const vector<Venue>& venues,
                                          const string& filterType,
                                          const set<string>& uniqueOptions,
                                          vector<SelectedVenue>& temporaryFilteredVenues) {
    return filterByOptionCommon(venues, uniqueOptions, filterType, temporaryFilteredVenues);
}

// Function to filter by Capacity
vector<SelectedVenue> VenueFilter::filterByCapacity(const vector<Venue>& venues,
                                            const set<int>& uniqueCapacities,
                                            vector<SelectedVenue>& temporaryFilteredVenues) {
    vector<int> filterOptions(uniqueCapacities.begin(), uniqueCapacities.end());
    ConsoleUtils::clearConsole();
    cout << "===== Filter By Capacity =====" << endl;


    cout << "Available Options: " << endl;
    for (size_t i = 0; i < filterOptions.size(); ++i) {
        cout << i + 1 << ". " << filterOptions[i] << endl;
    }

    cout << "Enter comma-separated indices of options to select: ";
    string input;
    ConsoleUtils::clearInputBuffer();
    getline(cin, input);

    cout << endl; // Add a line of space

    // Validate and process the user's input
    vector<size_t> selectedIndices;
    istringstream iss(input);
    string indexStr;
    while (getline(iss, indexStr, CSV_DELIMITER)) {
        try {
            size_t selectedIndex = stoi(indexStr) - 1;
            selectedIndices.push_back(selectedIndex);
        } catch (const exception& e) {
            cout << "Invalid index format. Skipping." << endl;
            cout << "Press return to continue..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press 
        }
    }

    // Now you have the validated selectedIndices vector to work with
    for (size_t selectedIndex : selectedIndices) {
        if (selectedIndex < filterOptions.size()) {
            int filterValue = filterOptions[selectedIndex];
            for (const Venue& venue : venues) {
                if (venue.capacity == filterValue) {
                    SelectedVenue selectedVenue = VenueUtilities::convertToSelectedVenue(venue);
                    temporaryFilteredVenues.push_back(selectedVenue);
                }
            }
        } else {
            cout << "Invalid index: " << selectedIndex << endl;
            cout << "Press return to continue..." << endl;
            cin.ignore();  // If there's a chance you might have used cin before this point
            ConsoleUtils::clearInputBuffer();
            cin.get();     // This will wait for a key press 
        }
    }

    return temporaryFilteredVenues;
}
