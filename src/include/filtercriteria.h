#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "fileutils.h"
#include "structs.h"

#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

// Structure to hold filter criteria for venues
struct FilterCriteria {
    std::string genre;
    std::string state;
    std::string city;
};

// Utility class to perform common venue-related operations
class VenueUtilities {
public:
    // Convert a Venue object to a SelectedVenue object
    static SelectedVenue convertToSelectedVenue(const Venue& venue);

    // Functions to get unique genres, states, cities, and capacities from a list of venues
    std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues);
    std::set<std::string> getUniqueStates(const std::vector<Venue>& venues);
    std::set<std::string> getUniqueCities(const std::vector<Venue>& venues);
    std::set<int> getUniqueCapacities(const std::vector<Venue>& venues);
};

// Class to handle venue filtering logic
class VenueFilter {
private:
    // Max length for venue selection indices input
    const std::string::size_type MAX_INPUT_LENGTH = 256;
    
    // Delimiter used in CSV files
    const char CSV_DELIMITER = ','; // Delimiter used in CSV files

    // Common function to handle venue filtering by options like Genre, State, City
    std::vector<SelectedVenue> filterByOptionCommon(const std::vector<Venue>& venues,
                                                    const std::set<std::string>& uniqueOptions,
                                                    const std::string& filterType,
                                                    std::vector<SelectedVenue>& temporaryFilteredVenues);

public:
    // Function to process user input and select venues based on it
    void processVenueSelection(const std::vector<SelectedVenue>& temporaryFilteredVenues,
                               std::vector<SelectedVenue>& selectedVenuesForEmail,
                               std::istream& input = std::cin,
                               std::ostream& output = std::cout);

    // Function to display the filtered venues to the user
    void displayFilteredVenues(const std::vector<SelectedVenue>& selectedVenuesForDisplay);


    // Function to filter venues based on a specific option (Genre, State, etc.)
    std::vector<SelectedVenue> filterByOption(const std::vector<Venue>& venues,
                                              const std::string& filterType,
                                              const std::set<std::string>& uniqueOptions,
                                              std::vector<SelectedVenue>& temporaryFilteredVenues);

    // Function to filter venues based on capacity
    std::vector<SelectedVenue> filterByCapacity(const std::vector<Venue>& venues,
                                                const std::set<int>& uniqueCapacities,
                                                std::vector<SelectedVenue>& temporaryFilteredVenues);
};

#endif // FILTERCRITERIA_H
