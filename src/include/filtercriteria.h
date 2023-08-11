#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "venue.h"

#include <set>
#include <vector>

// Structure to hold filter criteria
struct FilterCriteria {
    bool filterFlag = false;
    bool filterByGenre = false;
    bool filterByState = false;
    bool filterByCity = false;
    bool filterByCapacity = false;
    std::string genre;
    std::string state;
    std::string city;
    int capacity = 0; // Change to int type
};

// Get a set of unique genres/states/cities/capacities from a vector of venues
std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues);
std::set<std::string> getUniqueStates(const std::vector<Venue>& venues);
std::set<std::string> getUniqueCities(const std::vector<Venue>& venues);
std::set<int> getUniqueCapacities(const std::vector<Venue>& venues);

/**
 * Filters a vector of venues based on the selected option.
 *
 * @param venues The vector of Venue objects to filter.
 * @param optionType The type of option being filtered (e.g., "Genre", "State").
 * @param uniqueOptions A set of unique options for the selected type (e.g., unique genres, states).
 * @param filterValue The value of the selected option to filter by.
 * @return A vector of SelectedVenue objects that match the filter criteria.
 */
std::vector<SelectedVenue> filterByOption(const std::vector<Venue>& venues,
                                          const std::string& filterType,
                                          const std::set<std::string>& uniqueOptions,
                                          std::vector<SelectedVenue>& temporaryFilteredVenues);

/**
 * Filters a vector of venues based on the capacity.
 *
 * @param venues The vector of Venue objects to filter.
 * @param uniqueCapacities A set of unique capacity values.
 * @param filterValue The value of the selected capacity to filter by.
 * @return A vector of SelectedVenue objects that match the filter criteria.
 */
std::vector<SelectedVenue> filterByCapacity(const std::vector<Venue>& venues,
                                            const std::set<int>& uniqueCapacities,
                                            std::vector<SelectedVenue>& temporaryFilteredVenues);

#endif // FILTERCRITERIA_H