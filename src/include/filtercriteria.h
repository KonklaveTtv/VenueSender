#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "fileutils.h"
#include "structs.h"

#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

// Structure to hold filter criteria
struct FilterCriteria {
    std::string genre;
    std::string state;
    std::string city;
};

class VenueUtilities {
public:
// Convert Venue to SelectedVenue
static SelectedVenue convertToSelectedVenue(const Venue& venue);

// Get a set of unique genres/states/cities/capacities from a vector of venues
std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues);
std::set<std::string> getUniqueStates(const std::vector<Venue>& venues);
std::set<std::string> getUniqueCities(const std::vector<Venue>& venues);
std::set<int> getUniqueCapacities(const std::vector<Venue>& venues);
};

class VenueFilter {
private:
    // Venue selection indices max char length
    const std::string::size_type MAX_INPUT_LENGTH = 256;
    const char CSV_DELIMITER = ','; // Delimiter used in CSV files

    // Common function for filtering by an option (Genre, State, City)
    std::vector<SelectedVenue> filterByOptionCommon(const std::vector<Venue>& venues,
                                                    const std::set<std::string>& uniqueOptions,
                                                    const std::string& filterType,
                                                    std::vector<SelectedVenue>& temporaryFilteredVenues);

public:
// Function to process user input and select venues
void processVenueSelection(const std::vector<SelectedVenue>& temporaryFilteredVenues,
                           std::vector<SelectedVenue>& selectedVenuesForEmail,
                           std::istream& input = std::cin,
                           std::ostream& output = std::cout);

// Function to display filtered venues to the user
void displayFilteredVenues(const std::vector<SelectedVenue>& selectedVenuesForDisplay);


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
};

#endif // FILTERCRITERIA_H
