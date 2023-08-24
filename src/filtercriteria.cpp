#include "include/filtercriteria.h"

// Use the standard namespace
using namespace std;

// Function to process venue selection based on user input
void VenueFilter::processVenueSelection(const vector<SelectedVenue>& temporaryFilteredVenues,
                           vector<SelectedVenue>& selectedVenuesForEmail,
                           istream& input,
                           ostream& output) {
    if (temporaryFilteredVenues.empty()) {
        return;
    }

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    output << "Select venues to add (comma-separated indices): ";
    ConsoleUtils::resetColor();
    string userInput;
    getline(input, userInput);

    // Validate input length
    if (userInput.length() > MAX_INPUT_LENGTH) {
        ErrorHandler errorHandler;
        errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INPUT_LENGTH_ERROR);
        cout << "Press return to continue..." << endl;
        ConsoleUtils consoleUtils;
        consoleUtils.clearInputBuffer();
        cin.get(); // This will wait for a key press
        return;
    }

    istringstream iss(userInput);
    string indexStr;
    while (getline(iss, indexStr, ',')) {
        try {
            size_t selectedIndex = stoul(indexStr);
            if (selectedIndex == 0) {
                ErrorHandler errorHandler;
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                cout << "Press return to continue..." << endl;
                ConsoleUtils consoleUtils;
                consoleUtils.clearInputBuffer();
                cin.get(); // This will wait for a key press
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
                    ErrorHandler errorHandler;
                    errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::VENUE_ALREADY_SELECTED_ERROR);
                    cout << "Press return to continue..." << endl;
                    ConsoleUtils consoleUtils;
                    consoleUtils.clearInputBuffer();
                    cin.get(); // This will wait for a key press
                }
            } else {
                ErrorHandler errorHandler;
                errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                cout << "Press return to continue..." << endl;
                ConsoleUtils consoleUtils;
                consoleUtils.clearInputBuffer();
                cin.get(); // This will wait for a key press  
                continue;
            }
        } catch (const invalid_argument& e) {
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
            cout << "Press return to continue..." << endl;
            ConsoleUtils consoleUtils;
            consoleUtils.clearInputBuffer();
            cin.get(); // This will wait for a key press 
            continue;
        }
    }

    // Add a newline to separate the filtered venues from the main menu
    output << endl;
}

// Function to display filtered venues to the user
void VenueFilter::displayFilteredVenues(const vector<SelectedVenue>& selectedVenuesForDisplay) {
    if (selectedVenuesForDisplay.empty()) {
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
        cout << "No venues found." << endl;
        ConsoleUtils::resetColor();
        return;
    }

    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "Filtered Venues: " << endl;
    ConsoleUtils::resetColor();
    for (size_t i = 0; i < selectedVenuesForDisplay.size(); ++i) {
        const auto& venue = selectedVenuesForDisplay[i];
        cout << setw(2) << i + 1 << ". Name: " << venue.name << endl;
        cout << "   Email: " << venue.email << endl;
        cout << "   City: " << venue.city << endl;
        cout << "   Capacity: " << venue.capacity << endl;
        cout << endl;
    }
}

// Common function for filtering venues by Genre, State, or City
vector<SelectedVenue> VenueFilter::filterByOptionCommon(const vector<Venue>& venues,
                                                const set<string>& uniqueOptions,
                                                const string& filterType,
                                                vector<SelectedVenue>& temporaryFilteredVenues) {
    vector<string> filterOptions(uniqueOptions.begin(), uniqueOptions.end());

    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "===== Filter By " << filterType << " =====" << endl;
    
    cout << "Available Options: " << endl;
    ConsoleUtils::resetColor();
    for (size_t i = 0; i < filterOptions.size(); ++i) {
        cout << i + 1 << ". " << filterOptions[i] << endl;
    }


    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter comma-separated indices of options to select: ";
    ConsoleUtils::resetColor();
    string input;
    ConsoleUtils consoleUtils;
    consoleUtils.clearInputBuffer();
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
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cout << "Press return to continue..." << endl;
            ConsoleUtils consoleUtils;
            consoleUtils.clearInputBuffer();
            cin.get(); // This will wait for a key press            
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
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cout << "Press return to continue..." << endl;
            ConsoleUtils consoleUtils;
            consoleUtils.clearInputBuffer();
            cin.get(); // This will wait for a key press   
        }
    }

    return temporaryFilteredVenues;
}

// Function to filter venues by Genre, State, or City
vector<SelectedVenue> VenueFilter::filterByOption(const vector<Venue>& venues,
                                          const string& filterType,
                                          const set<string>& uniqueOptions,
                                          vector<SelectedVenue>& temporaryFilteredVenues) {
    return filterByOptionCommon(venues, uniqueOptions, filterType, temporaryFilteredVenues);
}

// Function to filter venues by Capacity
vector<SelectedVenue> VenueFilter::filterByCapacity(const vector<Venue>& venues,
                                            const set<int>& uniqueCapacities,
                                            vector<SelectedVenue>& temporaryFilteredVenues) {
    vector<int> filterOptions(uniqueCapacities.begin(), uniqueCapacities.end());

    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    cout << "===== Filter By Capacity =====" << endl;


    cout << "Available Options: " << endl;
    for (size_t i = 0; i < filterOptions.size(); ++i) {
        cout << i + 1 << ". " << filterOptions[i] << endl;
    }
    ConsoleUtils::resetColor();

    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter comma-separated indices of options to select: ";
    ConsoleUtils::resetColor();
    string input;
    ConsoleUtils consoleUtils;
    consoleUtils.clearInputBuffer();
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
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cout << "Press return to continue..." << endl;
            ConsoleUtils consoleUtils;
            consoleUtils.clearInputBuffer();
            cin.get(); // This will wait for a key press 
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
            ErrorHandler errorHandler;
            errorHandler.handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR, std::to_string(selectedIndex));
            cout << "Press return to continue..." << endl;
            ConsoleUtils consoleUtils;
            consoleUtils.clearInputBuffer();
            cin.get(); // This will wait for a key press 
        }
    }

    return temporaryFilteredVenues;
}
