#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "fileutils.h"
#include "structs.h"

#include <iomanip>
#include <iostream>
#include <set>
#include <variant>
#include <vector>

// Structure to hold filter criteria for venues
struct FilterCriteria {
    std::string country;
    std::string state;
    std::string city;
    std::string genre;
};

// Utility class to perform common venue-related operations
class VenueUtilities {
public:
    // Utility function to convert a Venue object to a SelectedVenue object
    static SelectedVenue convertToSelectedVenue(const Venue& venue);
    
    // Utility function to convert a SelectedVenue object to a Venue object
    static Venue convertToVenue(const SelectedVenue& selectedVenue);

    // Utility function to get unique countries from a list of venues
    static std::set<std::string> getUniqueCountries(const std::vector<Venue>& venues);

    // Utility function to get unique states from a list of venues
    static std::set<std::string> getUniqueStates(const std::vector<Venue>& venues);

    // Utility function to get unique cities from a list of venues
    static std::set<std::string> getUniqueCities(const std::vector<Venue>& venues);

    // Utility function to get unique capacities from a list of venues
    static std::set<int> getUniqueCapacities(const std::vector<Venue>& venues);

    // Utility function to get unique genres from a list of venues
    static std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues);

    // Utility function to get unique options for a given filterType from a list of venues
    static std::variant<std::set<std::string>, std::set<int>> getUniqueOptions(const std::vector<Venue>& venues, const std::string& filterType);
};

// Class to handle venue filtering logic
class VenueFilter {
private:
    // Utility to clear the temporary filtered venue vectors
    void clearTemporaryFilteredVenuesVectors();

    // Indices starting number
    static constexpr std::string::size_type INDICES_START_AT_ONE = 1;

    // Max length for venue selection indices input
    static constexpr std::string::size_type MAX_INPUT_LENGTH = 256;
   
    // Delimiter used in CSV files
    static constexpr char CSV_DELIMITER = ','; // Delimiter used in CSV files

    // Max countries to display to prevent displaying the sqlite index names
    static constexpr std::string::size_type MAX_COUNTRIES_TO_DISPLAY = 2;

    VenueUtilities venueUtilities;

    std::vector<SelectedVenue> selectedVenuesForEmail;
    std::vector<SelectedVenue> selectedVenuesForTemplates;
    std::vector<SelectedVenue> temporaryFilteredVenues;
    std::vector<SelectedVenue> temporaryFilteredVenuesBuffer;

    std::set<std::string> uniqueOptions;
    std::string filterType;

public:

    // Function to process user input and select venues based on it
    void processVenueSelection(const std::vector<Venue>& venues,
                                      std::vector<SelectedVenue>& selectedVenuesForEmail,
                                      std::vector<SelectedVenue>& selectedVenuesForTemplates,
                                      std::istream& input = std::cin,
                                      std::ostream& output = std::cout);

#ifdef UNIT_TESTING
    const std::vector<SelectedVenue>& getFilteredResults() const { return temporaryFilteredVenues; }
#endif
};

#endif // FILTERCRITERIA_H
