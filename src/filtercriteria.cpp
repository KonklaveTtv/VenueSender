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
std::set<std::string> VenueUtilities::getUniqueCountries(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueCountries;
    for (const auto& venue : venues) {
        if (uniqueCountries.find(venue.country) == uniqueCountries.end()) {
            uniqueCountries.insert(venue.country);
        }
    }
    return uniqueCountries;
}

// Utility function to get unique states from a list of venues
std::set<std::string> VenueUtilities::getUniqueStates(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueStates;
    for (const auto& venue : venues) {
        if (uniqueStates.find(venue.state) == uniqueStates.end()) {
            uniqueStates.insert(venue.state);
        }
    }
    return uniqueStates;
}

// Utility function to get unique cities from a list of venues
std::set<std::string> VenueUtilities::getUniqueCities(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueCities;
    for (const auto& venue : venues) {
        if (uniqueCities.find(venue.city) == uniqueCities.end()) {
            uniqueCities.insert(venue.city);
        }
    }
    return uniqueCities;
}

// Utility function to get unique capacities from a list of venues
std::set<int> VenueUtilities::getUniqueCapacities(const std::vector<Venue>& venues) {
    std::set<int> uniqueCapacities;
    for (const Venue& venue : venues) {
        uniqueCapacities.insert(venue.capacity);
    }
    return uniqueCapacities;
}

// Utility function to get unique genres from a list of venues
std::set<std::string> VenueUtilities::getUniqueGenres(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueGenres;
    for (const auto& venue : venues) {
        if (uniqueGenres.find(venue.genre) == uniqueGenres.end()) {
            uniqueGenres.insert(venue.genre);
        }
    }
    return uniqueGenres;
}

// Utility function to get unique options for a given filterType from a list of venues
std::variant<std::set<std::string>, std::set<int>> VenueUtilities::getUniqueOptions(const std::vector<Venue>& venues, const std::string& filterType) {
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
        return std::set<std::string>{}; // or some other default value
    }
}

// Function to process venue selection based on user input
void VenueFilter::processVenueSelection(const std::vector<Venue>& venues,
                                        istream& input,
                                        ostream& output) {
    std::vector<std::string> filterTypes = {"state", "city", "capacity", "genre"};
    
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    output << "===========================" << endl;
    output << "      Venue Selection      " << endl;
    output << "===========================" << endl;
    ConsoleUtils::resetColor();

    // Get unique countries from the initial list of venues
    std::set<std::string> uniqueCountries = venueUtilities.getUniqueCountries(venues);
    output << "Available Countries: ";
    size_t index = 1;
    for (const auto& country : uniqueCountries) {
        output << index++ << ". " << country << " ";
    }
    output << "\nPlease select a country index: ";
    size_t selectedIndex;
    input >> selectedIndex;
    if (selectedIndex > uniqueCountries.size() || selectedIndex < 1) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
        return;
    }
    auto it = uniqueCountries.begin();
    std::advance(it, selectedIndex - 1);
    std::string selectedCountry = *it;

    // Filter venues by selected country
    for (const auto& venue : venues) {
        if (venue.country == selectedCountry) {
            temporaryFilteredVenues.push_back(venueUtilities.convertToSelectedVenue(venue));
        }
    }
    
    for (const auto& filterType : filterTypes) {
        temporaryFilteredVenuesBuffer.clear();
        
        std::vector<Venue> convertedVenues;
        for (const auto& selectedVenue : temporaryFilteredVenues) {
            convertedVenues.push_back(venueUtilities.convertToVenue(selectedVenue));
        }
        auto uniqueOptionsVariant = venueUtilities.getUniqueOptions(convertedVenues, filterType);
        output << "Available " << filterType << "s: " << endl;

        size_t localIndex = 1;  // Reset the index for each filter type
        std::visit([&](auto&& arg) {
            for (const auto& option : arg) {
                output << localIndex++ << ". " << option << " ";
            }
        }, uniqueOptionsVariant);

        output << "\nPlease type 'all' to select all, or select " << filterType << " indices (comma-separated): ";
        input.ignore();
        std::string inputIndices;
        std::getline(input, inputIndices);
        std::transform(inputIndices.begin(), inputIndices.end(), inputIndices.begin(), ::tolower);

        std::vector<size_t> selectedIndices;
        size_t maxIndex = localIndex - 1;  // maximum number of options available

        if (inputIndices != "all") {
            std::istringstream iss(inputIndices);
            std::string indexStr;

            while (std::getline(iss, indexStr, ',')) {
                try {
                    size_t selectedIndex = std::stoul(indexStr);
                    if (selectedIndex < 1 || selectedIndex > maxIndex) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
                        return;
                    }
                    selectedIndices.push_back(selectedIndex);
                } catch (const std::exception& e) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                    return;
                }
            }
        }

        for (size_t selectedIndex : selectedIndices) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                auto it = arg.begin();
                std::advance(it, selectedIndex - 1);  // "-1" because we started from 1

                if constexpr (std::is_same_v<T, std::set<std::string>>) {
                    for (const auto& venue : temporaryFilteredVenues) {
                        if ((filterType == "state" && venue.state == *it) ||
                            (filterType == "city" && venue.city == *it) ||
                            (filterType == "genre" && venue.genre == *it)) {
                            temporaryFilteredVenuesBuffer.push_back(venue);
                        }
                    }
                } else if constexpr (std::is_same_v<T, std::set<int>>) {
                    for (const auto& venue : temporaryFilteredVenues) {
                        if (filterType == "capacity" && venue.capacity == *it) {
                            temporaryFilteredVenuesBuffer.push_back(venue);
                        }
                    }
                }
            }, uniqueOptionsVariant);
        }

        if (inputIndices == "all" || std::find(selectedIndices.begin(), selectedIndices.end(), 1) != selectedIndices.end()) {
            temporaryFilteredVenuesBuffer = temporaryFilteredVenues;
        }

        temporaryFilteredVenues = temporaryFilteredVenuesBuffer;
    }

    output << "--------- Final Venue Selection ---------\n";
    index = 1;
    for (const auto& venue : temporaryFilteredVenues) {
        output << index++ << ". " << venue.name << '\n';
    }
    output << "Please select the final venue indices (comma-separated): ";
    std::string finalIndices;
    input >> finalIndices;
    std::istringstream iss(finalIndices);
    std::string indexStr;
    std::vector<size_t> finalSelectedIndices;
    while (std::getline(iss, indexStr, ',')) {
        try {
            size_t finalIndex = std::stoul(indexStr);
            finalSelectedIndices.push_back(finalIndex);
        } catch (const std::exception& e) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
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

// Common function for filtering venues by Genre, State, or City
vector<SelectedVenue> VenueFilter::filterByOptionCommon(const vector<Venue>& venues,
                                                const set<string>& uniqueOptions,
                                                const string& filterType,
                                                vector<SelectedVenue>& temporaryFilteredVenues) {
    vector<string> filterOptions(uniqueOptions.begin(), uniqueOptions.end());
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);

    cout << "==========================="<< endl;
    cout << "===== Filter By " << filterType << " =====" << endl;
    cout << "==========================="<< endl;
  //cout << "Available Options: " << endl;
    ConsoleUtils::resetColor();
    for (size_t i = 0; i < filterOptions.size(); ++i) {
        cout << i + 1 << ". " << filterOptions[i] << endl;
    }


    ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
    cout << "Enter comma-separated indices of options to select: ";
    ConsoleUtils::resetColor();
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
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cout << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer();
            cin.get(); // This will wait for a key press            
        }
    }

        for (size_t selectedIndex : selectedIndices) {
            if (selectedIndex < filterOptions.size()) {
                string filterValue = filterOptions[selectedIndex];

                for (const Venue& venue : venues) {
                    string venueValue;
                    if (filterType == "Countries") {
                        venueValue = venue.country;
                    } else if (filterType == "State") {
                        venueValue = venue.state;
                    } else if (filterType == "City") {
                        venueValue = venue.city;
                    } else if (filterType == "Genre") {
                        venueValue = venue.genre;
                    }


                    if ((filterType == "Countries" && venue.country == filterValue) ||
                        (filterType == "State" && venue.state == filterValue) ||
                        (filterType == "City" && venue.city == filterValue) ||
                        (filterType == "Genre" && venue.genre == filterValue)) {

                        SelectedVenue selectedVenue = venueUtilities.convertToSelectedVenue(venue);
                        temporaryFilteredVenues.push_back(selectedVenue);
                    }
                }
            } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cout << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer();
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
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
            cout << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer();
            cin.get(); // This will wait for a key press 
        }
    }

    // Now you have the validated selectedIndices vector to work with
    for (size_t selectedIndex : selectedIndices) {
        if (selectedIndex < filterOptions.size()) {
            int filterValue = filterOptions[selectedIndex];
            for (const Venue& venue : venues) {
                if (venue.capacity == filterValue) {
                    SelectedVenue selectedVenue = venueUtilities.convertToSelectedVenue(venue);
                    temporaryFilteredVenues.push_back(selectedVenue);
                }
            }
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR, std::to_string(selectedIndex));
            cout << "Press return to continue..." << endl;
            ConsoleUtils::clearInputBuffer();
            cin.get(); // This will wait for a key press 
        }
    }

    return temporaryFilteredVenues;
}
