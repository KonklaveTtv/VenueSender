#include "venueutils.h"
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
