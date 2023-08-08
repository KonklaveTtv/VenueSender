#include "venueutils.h"
#include "venue.h"

#include <algorithm>

// Implement getUniqueValues with a member pointer to a string
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

// Implement getUniqueValues with a member pointer to an integer
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