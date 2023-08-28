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
    ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
    output << "===========================" << endl;
    output << "      Venue Selection      " << endl;
    output << "===========================" << endl;
    ConsoleUtils::resetColor();

    // Step 1: Start with all venues
    for (const auto& venue : venues) {
        temporaryFilteredVenues.push_back(venueUtilities.convertToSelectedVenue(venue));
    }

    // Step 2: Filter by country
    std::set<std::string> uniqueCountries = venueUtilities.getUniqueCountries(venues);
    output << "Available Countries: ";
    size_t index = 1;
    for (const auto& country : uniqueCountries) {
        output << index++ << ". " << country << " ";
    }
    output << "\nPlease select a country index: ";
    size_t selectedIndex;
    input >> selectedIndex;
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (selectedIndex > uniqueCountries.size() || selectedIndex < 1) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_ERROR);
        return;
    }
    auto it = uniqueCountries.begin();
    std::advance(it, selectedIndex - 1);
    std::string selectedCountry = *it;

    temporaryFilteredVenuesBuffer.clear();
    for (const auto& venue : temporaryFilteredVenues) {
        if (venue.country == selectedCountry) {
            temporaryFilteredVenuesBuffer.push_back(venue);
        }
    }
    temporaryFilteredVenues = temporaryFilteredVenuesBuffer;
    
    // Step 3: Loop over filter types (state, city, capacity, genre)
    std::vector<std::string> filterTypes = {"state", "city", "capacity", "genre"};
    for (const auto& filterType : filterTypes) {
        temporaryFilteredVenuesBuffer.clear();
        
        std::vector<Venue> convertedVenues;
        for (const auto& selectedVenue : temporaryFilteredVenues) {
            convertedVenues.push_back(venueUtilities.convertToVenue(selectedVenue));
        }
        
        auto uniqueOptionsVariant = venueUtilities.getUniqueOptions(convertedVenues, filterType);
        
        output << "Available " << filterType << "s: " << endl;
        size_t localIndex = 1;
        std::vector<int> localCapacityVector;

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::set<int>>) {
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

        output << "\nPlease type 'all' to select all, or select " << filterType << " indices (comma-separated): ";
        std::string inputIndices;
        std::getline(input, inputIndices);
        std::transform(inputIndices.begin(), inputIndices.end(), inputIndices.begin(), ::tolower);

        std::vector<size_t> selectedIndices;
        std::istringstream iss(inputIndices);
        std::string indexStr;
        while (std::getline(iss, indexStr, ',')) {
            try {
                size_t selectedIndex = std::stoul(indexStr);
                if (selectedIndex == 0) {
                    ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INDEX_FORMAT_ERROR);
                    return;
                }
                selectedIndex--; 
                selectedIndices.push_back(selectedIndex);
            } catch (const std::invalid_argument& e) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_INPUT_ERROR);
                return;
            }
        }

        for (size_t selectedIndex : selectedIndices) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                auto it = arg.begin();
                std::advance(it, selectedIndex);

                if constexpr (std::is_same_v<T, std::set<std::string>>) {
                    for (const auto& venue : temporaryFilteredVenues) {
                        if (venue.state == *it || venue.city == *it || venue.genre == *it) {
                            temporaryFilteredVenuesBuffer.push_back(venue);
                        }
                    }
                } else if constexpr (std::is_same_v<T, std::set<int>>) {
                    for (const auto& venue : temporaryFilteredVenues) {
                        if (venue.capacity == *it) {
                            temporaryFilteredVenuesBuffer.push_back(venue);
                        }
                    }
                }
            }, uniqueOptionsVariant);
        }

        temporaryFilteredVenues = temporaryFilteredVenuesBuffer;
    }

    // Step 4: Final Venue Selection
    output << "--------- Final Venue Selection ---------\n";
    index = 1;
    for (const auto& venue : temporaryFilteredVenues) {
        output << index++ << ". " << venue.name << '\n';
    }
    output << "Please select the final venue indices (comma-separated): ";
    std::string finalIndices;
    std::getline(input, finalIndices);
    std::istringstream finalIss(finalIndices);
    std::string indexStr;
    std::vector<size_t> finalSelectedIndices;
    while (std::getline(finalIss, indexStr, ',')) {
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

