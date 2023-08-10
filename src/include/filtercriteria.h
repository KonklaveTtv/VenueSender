#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "venue.h"

#include <set>
#include <vector>

struct FilterCriteria {
    bool filterByGenre = false;
    bool filterByState = false;
    bool filterByCity = false;
    bool filterByCapacity = false;
    std::string genre;
    std::string state;
    std::string city;
    int capacity = 0; // Change to int type
};

/**
 * Filters a vector of venues based on the selected option.
 *
 * @param venues The vector of Venue objects to filter.
 * @param optionType The type of option being filtered (e.g., "Genre", "State").
 * @param uniqueOptions A set of unique options for the selected type (e.g., unique genres, states).
 * @param filterFlag A boolean flag indicating whether filtering is enabled.
 * @param filterValue The value of the selected option to filter by.
 * @return A vector of SelectedVenue objects that match the filter criteria.
 */
template <typename T>
std::vector<SelectedVenue> filterByOption(const std::vector<Venue>& venues,
                                          const std::string& optionType,
                                          const std::set<T>& uniqueOptions,
                                          bool& filterFlag, T filterValue);

// Function to filter venues based on the capacity
std::vector<SelectedVenue> filterByCapacity(const std::vector<Venue>& venues,
                                            const std::set<int>& uniqueCapacities,
                                            bool& filterFlag, int filterValue);

#endif // FILTERCRITERIA_H
