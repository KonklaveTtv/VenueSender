#include "include/filtercriteria.h"

// Use the standard namespace
using namespace std;

// Utility function to convert a Venue object to a SelectedVenue object
SelectedVenue VenueUtilities::convertToSelectedVenue(const Venue& venue) {
    // Create a SelectedVenue instance based on Venue data
    SelectedVenue selectedVenue;
    selectedVenue.name = venue.name;
    selectedVenue.email = venue.email;
    selectedVenue.country = venue.country;
    selectedVenue.state = venue.state;
    selectedVenue.city = venue.city;   
    selectedVenue.capacity = venue.capacity;
    selectedVenue.genre = venue.genre;
    return selectedVenue;
}

Venue VenueUtilities::convertToVenue(const SelectedVenue& selectedVenue) {
    // Create a Venue instance based on SelectedVenue data
    Venue venue;
    venue.name = selectedVenue.name;
    venue.email = selectedVenue.email;
    venue.country = selectedVenue.country;
    venue.state = selectedVenue.state;
    venue.city = selectedVenue.city;   
    venue.capacity = selectedVenue.capacity;
    venue.genre = selectedVenue.genre;
    return venue;
}

// Utility function to get unique countries from a list of venues
set<string> VenueUtilities::getUniqueCountries(const vector<Venue>& venues) {
    set<string> uniqueCountries;
    for (const auto& venue : venues) {
        if (uniqueCountries.find(venue.country) == uniqueCountries.end()) {
            uniqueCountries.insert(venue.country);
        }
    }
    return uniqueCountries;
}

// Utility function to get unique states from a list of venues
set<string> VenueUtilities::getUniqueStates(const vector<Venue>& venues) {
    set<string> uniqueStates;
    for (const auto& venue : venues) {
        if (uniqueStates.find(venue.state) == uniqueStates.end()) {
            uniqueStates.insert(venue.state);
        }
    }
    return uniqueStates;
}

// Utility function to get unique cities from a list of venues
set<string> VenueUtilities::getUniqueCities(const vector<Venue>& venues) {
    set<string> uniqueCities;
    for (const auto& venue : venues) {
        if (uniqueCities.find(venue.city) == uniqueCities.end()) {
            uniqueCities.insert(venue.city);
        }
    }
    return uniqueCities;
}

// Utility function to get unique capacities from a list of venues
set<int> VenueUtilities::getUniqueCapacities(const vector<Venue>& venues) {
    set<int> uniqueCapacities;
    for (const Venue& venue : venues) {
        uniqueCapacities.insert(venue.capacity);
    }
    return uniqueCapacities;
}

// Utility function to get unique genres from a list of venues
set<string> VenueUtilities::getUniqueGenres(const vector<Venue>& venues) {
    set<string> uniqueGenres;
    for (const auto& venue : venues) {
        if (uniqueGenres.find(venue.genre) == uniqueGenres.end()) {
            uniqueGenres.insert(venue.genre);
        }
    }
    return uniqueGenres;
}

// Utility function to get unique options for a given filterType from a list of venues
variant<set<string>, set<int>> VenueUtilities::getUniqueOptions(const vector<Venue>& venues, const string& filterType) {
    if (filterType == "state") {
        return getUniqueStates(venues);
    } else if (filterType == "city") {
        return getUniqueCities(venues);
    } else if (filterType == "capacity") {
        return getUniqueCapacities(venues);
    } else if (filterType == "genre") {
        return getUniqueGenres(venues);
    } else {
        // Handle invalid filterType
        return set<string>{}; // or some other default value
    }
}

// Utility to clear the temporary filtered venue vectors
void VenueFilter::clearTemporaryFilteredVenuesVectors() {
    temporaryFilteredVenues.clear();
    temporaryFilteredVenuesBuffer.clear();
}

// Function to process venue selection based on user input
void VenueFilter::processVenueSelection(const vector<Venue>& venues,
                                        vector<SelectedVenue>& selectedVenuesForEmail,
                                        vector<SelectedVenue>& selectedVenuesForTemplates,
                                        istream& input,
                                        ostream& output) {
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    output << "===========================" << endl;
    output << "      Venue Selection      " << endl;
    output << "===========================" << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    // Clear the temporary filtered venue vectors
    VenueFilter::clearTemporaryFilteredVenuesVectors();


    // Step 1: Start with all venues
    for (const auto& venue : venues) {
        temporaryFilteredVenues.push_back(venueUtilities.convertToSelectedVenue(venue));
    }

    // If temporaryFilteredVenues is empty we return to Main Menu
    if (temporaryFilteredVenues.empty()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMPTY_VENUE_LIST_ERROR);
        return;
    }

    // Step 2: Filter by country
    set<string> uniqueCountries = venueUtilities.getUniqueCountries(venues);
    output << "Available Countries: ";

    // Limit the number of countries to show (for example, only show the first 5)
    size_t index = INDICES_START_AT_ONE;
    size_t maxCountriesToShow = MAX_COUNTRIES_TO_DISPLAY; // Set the desired maximum number of countries to show
    for (const auto& country : uniqueCountries) {
        if (index > maxCountriesToShow) {
            break; // Stop displaying countries after reaching the limit
        }
        output << index++ << ". " << country << " ";
    }

#ifndef UNIT_TESTING
ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    output << "\nPlease select a country index: ";
    size_t selectedIndex;
    input >> selectedIndex;
    ConsoleUtils::clearInputBuffer();
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    if (input.fail()) {
        ConsoleUtils::clearInputBuffer();
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
        return;
    }
    
    // Do not allow the user to enter an input less than MAX_INPUT_LENGTH
    if (to_string(selectedIndex).length() <= MIN_INPUT_LENGTH) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INPUT_LENGTH_ERROR);
        return;
    }

    // Validate input is not greater than MAX_INPUT_LENGTH
    if (to_string(selectedIndex).length() > MAX_INPUT_LENGTH) {
        ConsoleUtils::clearInputBuffer();
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INPUT_LENGTH_ERROR);
        return;
    }

    if (selectedIndex > uniqueCountries.size() || selectedIndex < 1 || selectedIndex > maxCountriesToShow) {
        ConsoleUtils::clearInputBuffer();
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
        return;
    }
    auto it = uniqueCountries.begin();
    advance(it, selectedIndex - 1);
    string selectedCountry = *it;

    temporaryFilteredVenuesBuffer.clear();
    for (const auto& venue : temporaryFilteredVenues) {
        if (venue.country == selectedCountry) {
            temporaryFilteredVenuesBuffer.push_back(venue);
        }
    }
    temporaryFilteredVenues = temporaryFilteredVenuesBuffer;
    vector<string> filterTypes = {"state", "city", "capacity", "genre"};

    // Step 3: Loop over filter types (state, city, capacity, genre)
    for (const auto& filterType : filterTypes) {
        temporaryFilteredVenuesBuffer.clear();

        vector<Venue> convertedVenues;
        for (const auto& selectedVenue : temporaryFilteredVenues) {
            convertedVenues.push_back(venueUtilities.convertToVenue(selectedVenue));
        }

        auto uniqueOptionsVariant = venueUtilities.getUniqueOptions(convertedVenues, filterType);

        output << "Available " << filterType << "s: " << endl;
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
        }, uniqueOptionsVariant);

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
            return;
        }

        if (inputIndices == "all") {
            // User selected all options, no need to filter
            temporaryFilteredVenuesBuffer = temporaryFilteredVenues;
        } else {
            // Check if the input contains only digits and commas
            if (!all_of(inputIndices.begin(), inputIndices.end(), [](char c) { return isdigit(c) || c == ','; })) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                return;
            }

            // Parse selected indices
            vector<size_t> selectedIndices;
            istringstream iss(inputIndices);
            string indexStr;
            while (getline(iss, indexStr, CSV_DELIMITER)) {
                try {
                    size_t selectedIndex = stoul(indexStr);


                    if (selectedIndex == 0 || selectedIndex > localIndex -1) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                        return;
                    }
                    selectedIndex--;
                    selectedIndices.push_back(selectedIndex);
                } catch (const invalid_argument& e) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                    return;
                }
            }

            // Filter venues based on selected indices
            for (size_t selectedIndex : selectedIndices) {
                visit([&](auto&& arg) {
                    using T = decay_t<decltype(arg)>;
                    auto it = arg.begin();
                    advance(it, selectedIndex);

                    if constexpr (is_same_v<T, set<string>>) {
                        for (const auto& venue : temporaryFilteredVenues) {
                            if (venue.state == *it || venue.city == *it || venue.genre == *it) {
                                temporaryFilteredVenuesBuffer.push_back(venue);
                            }
                        }
                    } else if constexpr (is_same_v<T, set<int>>) {
                        for (const auto& venue : temporaryFilteredVenues) {
                            if (venue.capacity == *it) {
                                temporaryFilteredVenuesBuffer.push_back(venue);
                            }
                        }
                    }
                }, uniqueOptionsVariant);
            }
        }

        temporaryFilteredVenues = temporaryFilteredVenuesBuffer;
    }

    // Step 4: Final Venue Selection
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
    output << "===========================" << endl;
    output << "   Final Venue Selection   " << endl;
    output << "===========================\n";
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif

    index = INDICES_START_AT_ONE;
    for (const auto& venue : temporaryFilteredVenues) {
        output << index++ << ". " << venue.name << '\n';
        output << "State: " << venue.state << '\n';
        output << "City: " << venue.city << '\n';
        output << "Capacity: " << venue.capacity << '\n';
    }

#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
    output << "Please select the final venue using (comma-separated) indices or type 'ALL': ";
    string finalIndices;
    getline(input, finalIndices);
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
    transform(finalIndices.begin(), finalIndices.end(), finalIndices.begin(), ::tolower);

    if (finalIndices == "all") {
        selectedVenuesForEmail = temporaryFilteredVenues;
        selectedVenuesForTemplates = temporaryFilteredVenues;
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
                return;
            }
        }

        for (size_t finalIndex : finalSelectedIndices) {
            if (finalIndex < 1 || finalIndex > temporaryFilteredVenues.size()) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
                return;
            }
            selectedVenuesForEmail.push_back(temporaryFilteredVenues[finalIndex - 1]);
            selectedVenuesForTemplates.push_back(temporaryFilteredVenues[finalIndex - 1]);
        }
    }
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
    cout << "===========================" << endl;
    cout << "        Venues Added       " << endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor();
#endif
}
