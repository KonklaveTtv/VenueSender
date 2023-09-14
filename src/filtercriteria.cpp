#include "include/filtercriteria.h"

// Use the standard namespace
using namespace std;

VenueForEmails VenueUtilities::convertToVenueForEmails(const SelectedVenueForEmails& selectedVenueForEmails) {
    // Create a Venue instance based on SelectedVenueForEmails data
    VenueForEmails venueForEmails;
    venueForEmails.name = selectedVenueForEmails.name;
    venueForEmails.email = selectedVenueForEmails.email;
    venueForEmails.country = selectedVenueForEmails.country;
    venueForEmails.state = selectedVenueForEmails.state;
    venueForEmails.city = selectedVenueForEmails.city;   
    venueForEmails.capacity = selectedVenueForEmails.capacity;
    venueForEmails.genre = selectedVenueForEmails.genre;
    return venueForEmails;
}

// Utility function to convert a Venue object to a SelectedVenue object
SelectedVenueForEmails VenueUtilities::convertToSelectedVenueForEmails(const VenueForEmails& venueForEmails) {
    // Create a SelectedVenue instance based on Venue data
    SelectedVenueForEmails selectedVenueForEmails;
    selectedVenueForEmails.name = venueForEmails.name;
    selectedVenueForEmails.email = venueForEmails.email;
    selectedVenueForEmails.country = venueForEmails.country;
    selectedVenueForEmails.state = venueForEmails.state;
    selectedVenueForEmails.city = venueForEmails.city;   
    selectedVenueForEmails.capacity = venueForEmails.capacity;
    selectedVenueForEmails.genre = venueForEmails.genre;
    return selectedVenueForEmails;
}

VenueForTemplates VenueUtilities::convertToVenueForTemplates(const SelectedVenueForTemplates& selectedVenueForTemplates) {
    // Create a VenueForTemplates instance based on SelectedVenueForTemplates data
    VenueForTemplates venueForTemplates;
    venueForTemplates.name = selectedVenueForTemplates.name;
    venueForTemplates.email = selectedVenueForTemplates.email;
    venueForTemplates.country = selectedVenueForTemplates.country;
    venueForTemplates.state = selectedVenueForTemplates.state;
    venueForTemplates.city = selectedVenueForTemplates.city;   
    venueForTemplates.capacity = selectedVenueForTemplates.capacity;
    venueForTemplates.genre = selectedVenueForTemplates.genre;
    return venueForTemplates;
}

// Utility function to convert a Venue object to a SelectedVenue object
SelectedVenueForTemplates VenueUtilities::convertToSelectedVenueForTemplates(const VenueForTemplates& venueForTemplates) {
    // Create a SelectedVenueForTemplates instance based on VenueForTemplates data
    SelectedVenueForTemplates selectedVenueForTemplates;
    selectedVenueForTemplates.name = venueForTemplates.name;
    selectedVenueForTemplates.email = venueForTemplates.email;
    selectedVenueForTemplates.country = venueForTemplates.country;
    selectedVenueForTemplates.state = venueForTemplates.state;
    selectedVenueForTemplates.city = venueForTemplates.city;   
    selectedVenueForTemplates.capacity = venueForTemplates.capacity;
    selectedVenueForTemplates.genre = venueForTemplates.genre;
    return selectedVenueForTemplates;
}

// Utility function to get unique countries from a list of venues
set<string> VenueUtilities::getUniqueCountriesForEmails(const vector<VenueForEmails>& venuesForEmails) {
    set<string> uniqueCountriesForEmails;
    for (const auto& venueForEmails : venuesForEmails) {
        if (uniqueCountriesForEmails.find(venueForEmails.country) == uniqueCountriesForEmails.end()) {
            uniqueCountriesForEmails.insert(venueForEmails.country);
        }
    }
    return uniqueCountriesForEmails;
}

// Utility function to get unique states from a list of venues
set<string> VenueUtilities::getUniqueStatesForEmails(const vector<VenueForEmails>& venuesForEmails) {
    set<string> uniqueStatesForEmails;
    for (const auto& venueForEmails : venuesForEmails) {
        if (uniqueStatesForEmails.find(venueForEmails.state) == uniqueStatesForEmails.end()) {
            uniqueStatesForEmails.insert(venueForEmails.state);
        }
    }
    return uniqueStatesForEmails;
}

// Utility function to get unique cities from a list of venues
set<string> VenueUtilities::getUniqueCitiesForEmails(const vector<VenueForEmails>& venuesForEmails) {
    set<string> uniqueCitiesForEmails;
    for (const auto& venueForEmails : venuesForEmails) {
        if (uniqueCitiesForEmails.find(venueForEmails.city) == uniqueCitiesForEmails.end()) {
            uniqueCitiesForEmails.insert(venueForEmails.city);
        }
    }
    return uniqueCitiesForEmails;
}

// Utility function to get unique capacities from a list of venues
set<int> VenueUtilities::getUniqueCapacitiesForEmails(const vector<VenueForEmails>& venuesForEmails) {
    set<int> uniqueCapacitiesForEmails;
    for (const VenueForEmails& venueForEmails : venuesForEmails) {
        uniqueCapacitiesForEmails.insert(venueForEmails.capacity);
    }
    return uniqueCapacitiesForEmails;
}

// Utility function to get unique genres from a list of venues
set<string> VenueUtilities::getUniqueGenresForEmails(const vector<VenueForEmails>& venuesForEmails) {
    set<string> uniqueGenresForEmails;
    for (const auto& venueForEmails : venuesForEmails) {
        if (uniqueGenresForEmails.find(venueForEmails.genre) == uniqueGenresForEmails.end()) {
            uniqueGenresForEmails.insert(venueForEmails.genre);
        }
    }
    return uniqueGenresForEmails;
}

// Utility function to get unique options for a given filterType from a list of venues
variant<set<string>, set<int>> VenueUtilities::getUniqueOptionsForEmails(const vector<VenueForEmails>& venuesForEmails, const string& filterType) {
    if (filterType == "state") {
        return getUniqueStatesForEmails(venuesForEmails);
    } else if (filterType == "city") {
        return getUniqueCitiesForEmails(venuesForEmails);
    } else if (filterType == "capacity") {
        return getUniqueCapacitiesForEmails(venuesForEmails);
    } else if (filterType == "genre") {
        return getUniqueGenresForEmails(venuesForEmails);
    } else {
        // Handle invalid filterType
        return set<string>{}; // or some other default value
    }
}

// Utility function to get unique countries from a list of venues
set<string> VenueUtilities::getUniqueCountriesForTemplates(const vector<VenueForTemplates>& venuesForTemplates) {
    set<string> uniqueCountriesForTemplates;
    for (const auto& venueForTemplates : venuesForTemplates) {
        if (uniqueCountriesForTemplates.find(venueForTemplates.country) == uniqueCountriesForTemplates.end()) {
            uniqueCountriesForTemplates.insert(venueForTemplates.country);
        }
    }
    return uniqueCountriesForTemplates;
}

// Utility function to get unique states from a list of venues
set<string> VenueUtilities::getUniqueStatesForTemplates(const vector<VenueForTemplates>& venuesForTemplates) {
    set<string> uniqueStatesForTemplates;
    for (const auto& venueForTemplates : venuesForTemplates) {
        if (uniqueStatesForTemplates.find(venueForTemplates.state) == uniqueStatesForTemplates.end()) {
            uniqueStatesForTemplates.insert(venueForTemplates.state);
        }
    }
    return uniqueStatesForTemplates;
}

// Utility function to get unique cities from a list of venues
set<string> VenueUtilities::getUniqueCitiesForTemplates(const vector<VenueForTemplates>& venuesForTemplates) {
    set<string> uniqueCitiesForTemplates;
    for (const auto& venueForTemplates : venuesForTemplates) {
        if (uniqueCitiesForTemplates.find(venueForTemplates.city) == uniqueCitiesForTemplates.end()) {
            uniqueCitiesForTemplates.insert(venueForTemplates.city);
        }
    }
    return uniqueCitiesForTemplates;
}

// Utility function to get unique capacities from a list of venues
set<int> VenueUtilities::getUniqueCapacitiesForTemplates(const vector<VenueForTemplates>& venuesForTemplates) {
    set<int> uniqueCapacitiesForTemplates;
    for (const VenueForTemplates& venueForTemplates : venuesForTemplates) {
        uniqueCapacitiesForTemplates.insert(venueForTemplates.capacity);
    }
    return uniqueCapacitiesForTemplates;
}

// Utility function to get unique genres from a list of venues
set<string> VenueUtilities::getUniqueGenresForTemplates(const vector<VenueForTemplates>& venuesForTemplates) {
    set<string> uniqueGenresForTemplates;
    for (const auto& venueForTemplates : venuesForTemplates) {
        if (uniqueGenresForTemplates.find(venueForTemplates.genre) == uniqueGenresForTemplates.end()) {
            uniqueGenresForTemplates.insert(venueForTemplates.genre);
        }
    }
    return uniqueGenresForTemplates;
}

// Utility function to get unique options for a given filterType from a list of venues
variant<set<string>, set<int>> VenueUtilities::getUniqueOptionsForTemplates(const vector<VenueForTemplates>& venuesForTemplates, const string& filterType) {
    if (filterType == "state") {
        return getUniqueStatesForTemplates(venuesForTemplates);
    } else if (filterType == "city") {
        return getUniqueCitiesForTemplates(venuesForTemplates);
    } else if (filterType == "capacity") {
        return getUniqueCapacitiesForTemplates(venuesForTemplates);
    } else if (filterType == "genre") {
        return getUniqueGenresForTemplates(venuesForTemplates);
    } else {
        // Handle invalid filterType
        return set<string>{}; // or some other default value
    }
}

// Utility to clear the temporary filtered venue vectors
void VenueFilter::clearTemporaryFilteredVenuesVectorsForEmails() {
    temporaryFilteredVenuesForEmails.clear();
    temporaryFilteredVenuesBufferForEmails.clear();
}

// Utility to clear the temporary filtered venue vectors
void VenueFilter::clearTemporaryFilteredVenuesVectorsForTemplates() {
    temporaryFilteredVenuesForTemplates.clear();
    temporaryFilteredVenuesBufferForTemplates.clear();
}

string displayFilterType(const string& filterType) {
    if (filterType == "state") {
        return "States";
    } else if (filterType == "city") {
        return "Cities";
    } else if (filterType == "genre") {
        return "Genres";
    } else if (filterType == "capacity") {
        return "Capacities";
    } else {
        return filterType;  // Default case, no change.
    }
}

// Function to process venue selection based on user input
void VenueFilter::processVenueSelectionForEmails(const vector<VenueForEmails>& venuesForEmails,
                                        vector<SelectedVenueForEmails>& selectedVenuesForEmails,
                                        istream& input,
                                        ostream& output) {
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::VENUE_SELECTION_MENU_HEADER);

    // Clear the temporary filtered venue vectors
    VenueFilter::clearTemporaryFilteredVenuesVectorsForEmails();


    // Step 1: Start with all venues
    for (const auto& venueForEmails : venuesForEmails) {
        temporaryFilteredVenuesForEmails.push_back(venueUtilities.convertToSelectedVenueForEmails(venueForEmails));
    }

    // If temporaryFilteredVenues is empty we return to Main Menu
    if (temporaryFilteredVenuesForEmails.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMPTY_VENUE_LIST_ERROR);
        cin.clear();
        return;
    }

    // Step 2: Filter by country
    set<string> uniqueCountriesForEmails = venueUtilities.getUniqueCountriesForEmails(venuesForEmails);
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::AVAILABLE_COUNTRIES_MESSAGE);

    // Limit the number of countries to show (for example, only show the first 5)
    size_t maxCountriesToShow = MAX_COUNTRIES_TO_DISPLAY; // Set the desired maximum number of countries to show
    string selectedCountry;
    bool countrySelected = false;  // Flag to check if a country is selected

    do {
        size_t index = INDICES_START_AT_ONE;
        for (const auto& country : uniqueCountriesForEmails) {
            if (index > maxCountriesToShow) {
                break; // Stop displaying countries after reaching the limit
            }
            output << index++ << ". " << country << " ";
        }

#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SELECT_COUNTRY_MESSAGE);
        cin.clear();
        size_t selectedIndex;
        input >> selectedIndex;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        cin.ignore();

        if (input.fail()) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
            cin.clear();
            continue;
        }

        // Do not allow the user to enter an input less than MAX_INPUT_LENGTH
        if (to_string(selectedIndex).length() <= MIN_INPUT_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_LENGTH_ERROR);
            cin.clear();
            continue;
        }

        // Validate input is not greater than MAX_INPUT_LENGTH
        if (to_string(selectedIndex).length() > MAX_INPUT_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_LENGTH_ERROR);
            cin.clear();
            continue;
        }

        if (selectedIndex > uniqueCountriesForEmails.size() || selectedIndex < 1 || selectedIndex > maxCountriesToShow) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
            cin.clear();
        } else {
            auto it = uniqueCountriesForEmails.begin();
            advance(it, selectedIndex - 1);
            selectedCountry = *it;
            countrySelected = true;  // Set the flag to true
            break;  // Break out of the loop
        }
    } while (!countrySelected);

    temporaryFilteredVenuesBufferForEmails.clear();
    for (const auto& venueForEmails : temporaryFilteredVenuesForEmails) {
        if (venueForEmails.country == selectedCountry) {
            temporaryFilteredVenuesBufferForEmails.push_back(venueForEmails);
        }
    }
    temporaryFilteredVenuesForEmails = temporaryFilteredVenuesBufferForEmails;
    vector<string> filterTypes = {"state", "city", "capacity", "genre"};

    // Step 3: Loop over filter types (state, city, capacity, genre)
    for (const auto& filterType : filterTypes) {
        do {
            temporaryFilteredVenuesBufferForEmails.clear();

            vector<VenueForEmails> convertedVenuesForEmails;
            for (const auto& selectedVenueForEmails : temporaryFilteredVenuesForEmails) {
                convertedVenuesForEmails.push_back(venueUtilities.convertToVenueForEmails(selectedVenueForEmails));
            }

            auto uniqueOptionsVariantForEmails = venueUtilities.getUniqueOptionsForEmails(convertedVenuesForEmails, filterType);

            output << "Available " << displayFilterType(filterType) << ": " << endl;
            size_t localIndex = INDICES_START_AT_ONE;
            vector<int> localCapacityVector;

            visit([&](auto&& arg) {
                using T = decay_t<decltype(arg)>;
                if constexpr (is_same_v<T, set<int>>) {
                    localCapacityVector.assign(arg.begin(), arg.end());
                    for (const auto& option : localCapacityVector) {
                        output << localIndex++ << ". " << option << " ";
                    }
                } else {
                    for (const auto& option : arg) {
                        output << localIndex++ << ". " << option << " ";
                    }
                }
            }, uniqueOptionsVariantForEmails);

#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
            output << "\nSelect a " << filterType << " using (comma-separated) indices: ";
            string inputIndices;
            getline(input, inputIndices);
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
#endif

            // Convert the input to lowercase for case-insensitive comparison
            transform(inputIndices.begin(), inputIndices.end(), inputIndices.begin(), ::tolower);

            if (inputIndices.empty()) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                cin.clear();
                continue;
            }

            if (inputIndices == "all" || inputIndices == "ALL") {
                // User selected all options, no need to filter
                temporaryFilteredVenuesBufferForEmails = temporaryFilteredVenuesForEmails;
            } else {
                // Check if the input contains only digits and commas
                if (!all_of(inputIndices.begin(), inputIndices.end(), [](char c) { return isdigit(c) || c == ','; })) {
                    ConsoleUtils::clearInputBuffer();
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                    cin.clear();
                    continue;
                }

                // Parse selected indices
                vector<size_t> selectedIndices;
                istringstream iss(inputIndices);
                string indexStr;
                while (getline(iss, indexStr, CSV_DELIMITER)) {
                    try {
                        size_t selectedIndex = stoul(indexStr);
                        if (selectedIndex == 0 || selectedIndex > localIndex - SQLITE_TABLE_INDEX) {
                            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                            cin.clear();
                            continue;
                        }
                        selectedIndex--;
                        selectedIndices.push_back(selectedIndex);
                    } catch (const invalid_argument& e) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                        cin.clear();
                        continue;
                    }
                }

                // Filter venues based on selected indices
                for (size_t selectedIndex : selectedIndices) {
                    visit([&](auto&& arg) {
                        using T = decay_t<decltype(arg)>;
                        auto it = arg.begin();
                        advance(it, selectedIndex);

                        if constexpr (is_same_v<T, set<string>>) {
                            for (const auto& venueForEmails : temporaryFilteredVenuesForEmails) {
                                if (venueForEmails.state == *it || venueForEmails.city == *it || venueForEmails.genre == *it) {
                                    temporaryFilteredVenuesBufferForEmails.push_back(venueForEmails);
                                }
                            }
                        } else if constexpr (is_same_v<T, set<int>>) {
                            for (const auto& venueForEmails : temporaryFilteredVenuesForEmails) {
                                if (venueForEmails.capacity == *it) {
                                    temporaryFilteredVenuesBufferForEmails.push_back(venueForEmails);
                                }
                            }
                        }
                    }, uniqueOptionsVariantForEmails);
                }
            }

            if (!temporaryFilteredVenuesBufferForEmails.empty()) {
                temporaryFilteredVenuesForEmails = temporaryFilteredVenuesBufferForEmails;
                break;  // Move to next iteration only if input is valid
            }
            else {
                continue;  // Restart the selection process for this step
            }
        } while (true);
    }

    // Step 4: Final Venue Selection
    bool isValidInput = true;
    do {
        isValidInput = true;  // Reset the flag for each iteration
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::FINAL_VENUE_SELECTION_MENU_HEADER);
        cin.clear();
        size_t index;
        index = INDICES_START_AT_ONE;
        for (const auto& venueForEmails : temporaryFilteredVenuesForEmails) {
            output << index++ << ". " << venueForEmails.name << '\n';
            output << "State: " << venueForEmails.state << '\n';
            output << "City: " << venueForEmails.city << '\n';
            output << "Capacity: " << venueForEmails.capacity << '\n';
        }

#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::FINAL_VENUE_SELECTION_MESSAGE);
        cin.clear();
        string finalIndices;
        getline(input, finalIndices);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        transform(finalIndices.begin(), finalIndices.end(), finalIndices.begin(), ::tolower);

        if (finalIndices.empty()) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cin.clear();
            continue;
        }
        
        if (finalIndices == "all" || finalIndices == "ALL") {
            selectedVenuesForEmails = temporaryFilteredVenuesForEmails;
            break;
            } else {
            istringstream finalIss(finalIndices);
            string indexStr;
            vector<size_t> finalSelectedIndices;
            while (getline(finalIss, indexStr, CSV_DELIMITER)) {
                try {
                    size_t finalIndex = stoul(indexStr);
                    finalSelectedIndices.push_back(finalIndex);
                } catch (const exception& e) {
                    ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                    ConsoleUtils::clearInputBuffer();
                    continue;
                }
            }

            if (!isValidInput) {
                continue;  // Invalid input, restart the do-while loop
            }

            for (size_t finalIndex : finalSelectedIndices) {
                if (finalIndex < 1 || finalIndex > temporaryFilteredVenuesForEmails.size()) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
                    cin.clear();
                    isValidInput = false;
                    break;
                }
                // Check if the venue is already selected
                auto it = find_if(selectedVenuesForEmails.begin(), selectedVenuesForEmails.end(), 
                    [this, finalIndex](const SelectedVenueForEmails& venueForEmails) {
                        return venueForEmails.name == this->temporaryFilteredVenuesForEmails[finalIndex - 1].name && 
                               venueForEmails.state == this->temporaryFilteredVenuesForEmails[finalIndex - 1].state &&
                               venueForEmails.city == this->temporaryFilteredVenuesForEmails[finalIndex - 1].city && 
                               venueForEmails.capacity == this->temporaryFilteredVenuesForEmails[finalIndex - 1].capacity;
                    });

                if (it == selectedVenuesForEmails.end()) {
                    // The venue has not been selected yet, so add it
                    selectedVenuesForEmails.push_back(temporaryFilteredVenuesForEmails[finalIndex - 1]);
                } else {
                    ostringstream duplicateVenueDetails;
                    duplicateVenueDetails << it->name << endl
                                          << it->city << endl
                                          << it->state;
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::VENUE_ALREADY_SELECTED_ERROR, duplicateVenueDetails.str());
                    cin.clear();
                    continue;
                }
            }

            if (!isValidInput) {
                continue;  // Invalid input, restart the do-while loop
            }
            break;
        }
    } while (true);
    
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::VENUES_ADDED_MENU_HEADER);
    cin.clear();
}

// Function to process venue selection based on user input
void VenueFilter::processVenueSelectionForTemplates(const vector<VenueForTemplates>& venuesForTemplates,
                                        vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                                        istream& input,
                                        ostream& output) {
    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::VENUE_SELECTION_MENU_HEADER);

    // Clear the temporary filtered venue vectors
    VenueFilter::clearTemporaryFilteredVenuesVectorsForTemplates();


    // Step 1: Start with all venues
    for (const auto& venueForTemplates : venuesForTemplates) {
        temporaryFilteredVenuesForTemplates.push_back(venueUtilities.convertToSelectedVenueForTemplates(venueForTemplates));
    }

    // If temporaryFilteredVenuesForTemplates is empty we return to Main Menu
    if (temporaryFilteredVenuesForTemplates.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMPTY_VENUE_LIST_ERROR);
        cin.clear();
        return;
    }

    // Step 2: Filter by country
    set<string> uniqueCountriesForTemplates = venueUtilities.getUniqueCountriesForTemplates(venuesForTemplates);
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::AVAILABLE_COUNTRIES_MESSAGE);

    // Limit the number of countries to show (for example, only show the first 5)
    size_t maxCountriesToShow = MAX_COUNTRIES_TO_DISPLAY; // Set the desired maximum number of countries to show
    string selectedCountry;
    bool countrySelected = false;  // Flag to check if a country is selected
    
    do {
        size_t index = INDICES_START_AT_ONE;
        for (const auto& country : uniqueCountriesForTemplates) {
            if (index > maxCountriesToShow) {
                break; // Stop displaying countries after reaching the limit
            }
            output << index++ << ". " << country << " ";
        }

#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::SELECT_COUNTRY_MESSAGE);
        cin.clear();
        size_t selectedIndex;
        input >> selectedIndex;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        cin.ignore();

        if (input.fail()) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
            cin.clear();
            continue;
        }

        // Do not allow the user to enter an input less than MAX_INPUT_LENGTH
        if (to_string(selectedIndex).length() <= MIN_INPUT_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_LENGTH_ERROR);
            cin.clear();
            continue;
        }

        // Validate input is not greater than MAX_INPUT_LENGTH
        if (to_string(selectedIndex).length() > MAX_INPUT_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_LENGTH_ERROR);
            cin.clear();
            continue;
        }

        if (selectedIndex > uniqueCountriesForTemplates.size() || selectedIndex < 1 || selectedIndex > maxCountriesToShow) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
            cin.clear();
        } else {
            auto it = uniqueCountriesForTemplates.begin();
            advance(it, selectedIndex - 1);
            selectedCountry = *it;
            countrySelected = true;  // Set the flag to true
            break;  // Break out of the loop
        }
    } while (!countrySelected);

    temporaryFilteredVenuesBufferForTemplates.clear();
    for (const auto& venueForTemplates : temporaryFilteredVenuesForTemplates) {
        if (venueForTemplates.country == selectedCountry) {
            temporaryFilteredVenuesBufferForTemplates.push_back(venueForTemplates);
        }
    }
    temporaryFilteredVenuesForTemplates = temporaryFilteredVenuesBufferForTemplates;
    vector<string> filterTypes = {"state", "city", "capacity", "genre"};

    // Step 3: Loop over filter types (state, city, capacity, genre)
    for (const auto& filterType : filterTypes) {
        do {
            temporaryFilteredVenuesBufferForTemplates.clear();

            vector<VenueForTemplates> convertedVenuesForTemplates;
            for (const auto& selectedVenueForTemplates : temporaryFilteredVenuesForTemplates) {
                convertedVenuesForTemplates.push_back(venueUtilities.convertToVenueForTemplates(selectedVenueForTemplates));
            }

            auto uniqueOptionsVariantForTemplates = venueUtilities.getUniqueOptionsForTemplates(convertedVenuesForTemplates, filterType);

            output << "Available " << displayFilterType(filterType) << ": " << endl;
            size_t localIndex = INDICES_START_AT_ONE;
            vector<int> localCapacityVector;

            visit([&](auto&& arg) {
                using T = decay_t<decltype(arg)>;
                if constexpr (is_same_v<T, set<int>>) {
                    localCapacityVector.assign(arg.begin(), arg.end());
                    for (const auto& option : localCapacityVector) {
                        output << localIndex++ << ". " << option << " ";
                    }
                } else {
                    for (const auto& option : arg) {
                        output << localIndex++ << ". " << option << " ";
                    }
                }
            }, uniqueOptionsVariantForTemplates);

    #ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    #endif
            output << "\nSelect a " << filterType << " using (comma-separated) indices: ";
            string inputIndices;
            getline(input, inputIndices);
    #ifndef UNIT_TESTING
            ConsoleUtils::resetColor();
    #endif

            // Convert the input to lowercase for case-insensitive comparison
            transform(inputIndices.begin(), inputIndices.end(), inputIndices.begin(), ::tolower);

            if (inputIndices.empty()) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                cin.clear();
                continue;
            }

            if (inputIndices == "all" || inputIndices == "ALL") {
                // User selected all options, no need to filter
                temporaryFilteredVenuesBufferForTemplates = temporaryFilteredVenuesForTemplates;
            } else {
                // Check if the input contains only digits and commas
                if (!all_of(inputIndices.begin(), inputIndices.end(), [](char c) { return isdigit(c) || c == ','; })) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                    cin.clear();
                    continue;
                }

                // Parse selected indices
                vector<size_t> selectedIndices;
                istringstream iss(inputIndices);
                string indexStr;
                while (getline(iss, indexStr, CSV_DELIMITER)) {
                    try {
                        size_t selectedIndex = stoul(indexStr);
                        if (selectedIndex == 0 || selectedIndex > localIndex - SQLITE_TABLE_INDEX) {
                            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                            cin.clear();
                            continue;
                        }
                        selectedIndex--;
                        selectedIndices.push_back(selectedIndex);
                    } catch (const invalid_argument& e) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                        cin.clear();
                        continue;
                    }
                }

                // Filter venues based on selected indices
                for (size_t selectedIndex : selectedIndices) {
                    visit([&](auto&& arg) {
                        using T = decay_t<decltype(arg)>;
                        auto it = arg.begin();
                        advance(it, selectedIndex);

                        if constexpr (is_same_v<T, set<string>>) {
                            for (const auto& venueForTemplates : temporaryFilteredVenuesForTemplates) {
                                if (venueForTemplates.state == *it || venueForTemplates.city == *it || venueForTemplates.genre == *it) {
                                    temporaryFilteredVenuesBufferForTemplates.push_back(venueForTemplates);
                                }
                            }
                        } else if constexpr (is_same_v<T, set<int>>) {
                            for (const auto& venueForTemplates : temporaryFilteredVenuesForTemplates) {
                                if (venueForTemplates.capacity == *it) {
                                    temporaryFilteredVenuesBufferForTemplates.push_back(venueForTemplates);
                                }
                            }
                        }
                    }, uniqueOptionsVariantForTemplates);
                }
            }

            if (!temporaryFilteredVenuesBufferForTemplates.empty()) {
                temporaryFilteredVenuesForTemplates = temporaryFilteredVenuesBufferForTemplates;
                break;  // Move to next iteration only if input is valid
            }
            else {
                continue;  // Restart the selection process for this step
            }
        } while (true);
    }

    // Step 4: Final Venue Selection
    bool isValidInput = true;
    do {
        isValidInput = true;  // Reset the flag for each iteration
        MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::FINAL_VENUE_SELECTION_MENU_HEADER);
        cin.clear();
        size_t index;
        index = INDICES_START_AT_ONE;
        for (const auto& venueForTemplates : temporaryFilteredVenuesForTemplates) {
            output << index++ << ". " << venueForTemplates.name << '\n';
            output << "State: " << venueForTemplates.state << '\n';
            output << "City: " << venueForTemplates.city << '\n';
            output << "Capacity: " << venueForTemplates.capacity << '\n';
        }

#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::FINAL_VENUE_SELECTION_MESSAGE);
        cin.clear();
        string finalIndices;
        getline(input, finalIndices);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        transform(finalIndices.begin(), finalIndices.end(), finalIndices.begin(), ::tolower);

        if (finalIndices.empty()) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cin.clear();
            continue;
        }
        
        if (finalIndices == "all" || finalIndices == "ALL") {
            selectedVenuesForTemplates = temporaryFilteredVenuesForTemplates;
            break;
            } else {
            istringstream finalIss(finalIndices);
            string indexStr;
            vector<size_t> finalSelectedIndices;
            while (getline(finalIss, indexStr, CSV_DELIMITER)) {
                try {
                    size_t finalIndex = stoul(indexStr);
                    finalSelectedIndices.push_back(finalIndex);
                } catch (const exception& e) {
                    ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                    ConsoleUtils::clearInputBuffer();
                    continue;
                }
            }

            if (!isValidInput) {
                continue;  // Invalid input, restart the do-while loop
            }

            for (size_t finalIndex : finalSelectedIndices) {
                if (finalIndex < 1 || finalIndex > temporaryFilteredVenuesForTemplates.size()) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
                    cin.clear();
                    isValidInput = false;
                    break;
                }
                // Check if the venue is already selected
                auto it = find_if(selectedVenuesForTemplates.begin(), selectedVenuesForTemplates.end(), 
                    [this, finalIndex](const SelectedVenueForTemplates& venueForTemplates) {
                        return venueForTemplates.name == this->temporaryFilteredVenuesForTemplates[finalIndex - 1].name && 
                               venueForTemplates.state == this->temporaryFilteredVenuesForTemplates[finalIndex - 1].state &&
                               venueForTemplates.city == this->temporaryFilteredVenuesForTemplates[finalIndex - 1].city && 
                               venueForTemplates.capacity == this->temporaryFilteredVenuesForTemplates[finalIndex - 1].capacity;
                    });

                if (it == selectedVenuesForTemplates.end()) {
                    // The venue has not been selected yet, so add it
                    selectedVenuesForTemplates.push_back(temporaryFilteredVenuesForTemplates[finalIndex - 1]);
                } else {
                    ostringstream duplicateVenueDetails;
                    duplicateVenueDetails << it->name << endl
                                          << it->city << endl
                                          << it->state;
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::VENUE_ALREADY_SELECTED_ERROR, duplicateVenueDetails.str());
                    cin.clear();
                    continue;
                }
            }
        
            if (!isValidInput) {
                continue;  // Invalid input, restart the do-while loop
            }        
            break;
        }
    } while (true);

    MenuTitleHandler::displayMenuTitle(MenuTitleHandler::MenuTitleType::VENUES_ADDED_MENU_HEADER);
    cin.clear();
}