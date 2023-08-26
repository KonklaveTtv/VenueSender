#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "fileutils.h"
#include "structs.h"

#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

extern std::vector<SelectedVenue> temporaryFilteredVenues;

// Structure to hold filter criteria for venues
struct FilterCriteria {
    std::string genre;
    std::string country;
    int capacity;
    std::string state;
    std::string city;
};

// Utility class to perform common venue-related operations
class VenueUtilities {
public:
    // Utility function to convert a Venue object to a SelectedVenue object
    inline static SelectedVenue convertToSelectedVenue(const Venue& venue) {
        // Create a SelectedVenue instance based on Venue data
        SelectedVenue selectedVenue;
        selectedVenue.name = venue.name;
        selectedVenue.email = venue.email;
        selectedVenue.genre = venue.genre;
        selectedVenue.country = venue.country;
        selectedVenue.capacity = venue.capacity;
        selectedVenue.state = venue.state;
        selectedVenue.city = venue.city;        
        return selectedVenue;
    }

    // Utility function to get unique genres from a list of venues
    inline std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues) {
        std::set<std::string> uniqueGenres;
        for (const auto& venue : venues) {
            if (uniqueGenres.find(venue.genre) == uniqueGenres.end()) {
                uniqueGenres.insert(venue.genre);
            }
        }
        return uniqueGenres;
    }

    // Utility function to get unique countries from a list of venues
    inline std::set<std::string> getUniqueCountries(const std::vector<Venue>& venues) {
        std::set<std::string> uniqueCountries;
        for (const auto& venue : venues) {
            if (uniqueCountries.find(venue.country) == uniqueCountries.end()) {
                uniqueCountries.insert(venue.country);
            }
        }
        return uniqueCountries;
    }

    // Utility function to get unique capacities from a list of venues
    inline std::set<int> getUniqueCapacities(const std::vector<Venue>& venues) {
        std::set<int> uniqueCapacities;
        for (const Venue& venue : venues) {
            uniqueCapacities.insert(venue.capacity);
        }
        return uniqueCapacities;
    }

    // Utility function to get unique states from a list of venues
    inline std::set<std::string> getUniqueStates(const std::vector<Venue>& venues) {
        std::set<std::string> uniqueStates;
        for (const auto& venue : venues) {
            if (uniqueStates.find(venue.state) == uniqueStates.end()) {
                uniqueStates.insert(venue.state);
            }
        }
        return uniqueStates;
    }

    // Utility function to get unique cities from a list of venues
    inline std::set<std::string> getUniqueCities(const std::vector<Venue>& venues) {
        std::set<std::string> uniqueCities;
        for (const auto& venue : venues) {
            if (uniqueCities.find(venue.city) == uniqueCities.end()) {
                uniqueCities.insert(venue.city);
            }
        }
        return uniqueCities;
    }
};

// Class to handle venue filtering logic
class VenueFilter {
private:
    // Max length for venue selection indices input
    static constexpr std::string::size_type MAX_INPUT_LENGTH = 256;
    
    // Delimiter used in CSV files
    static constexpr char CSV_DELIMITER = ','; // Delimiter used in CSV files

    std::vector<SelectedVenue> selectedVenuesForEmail;
    std::vector<SelectedVenue> temporaryFilteredVenues;

    std::set<std::string> uniqueOptions;
    std::string filterType;

    // Common function to handle venue filtering by options like Genre, State, City
    std::vector<SelectedVenue> filterByOptionCommon(const std::vector<Venue>& venues,
                                                    const std::set<std::string>& uniqueOptions,
                                                    const std::string& filterType,
                                                    std::vector<SelectedVenue>& temporaryFilteredVenues);
    
public:
    // Function to process user input and select venues based on it
    static void processVenueSelection(const std::vector<SelectedVenue>& temporaryFilteredVenues,
                               std::vector<SelectedVenue>& selectedVenuesForEmail,
                               std::vector<SelectedVenue>& selectedVenuesForTemplates,
                               std::istream& input = std::cin,
                               std::ostream& output = std::cout) ;

    // Function to display the filtered venues to the user
    static void displayFilteredVenues(const std::vector<SelectedVenue>& selectedVenuesForDisplay);


    // Function to filter venues based on a specific option (Genre, State, etc.)
    std::vector<SelectedVenue> filterByOption(const std::vector<Venue>& venues,
                                              const std::string& filterType,
                                              const std::set<std::string>& uniqueOptions,
                                              std::vector<SelectedVenue>& temporaryFilteredVenues);

    // Function to filter venues based on capacity
    std::vector<SelectedVenue> filterByCapacity(const std::vector<Venue>& venues,
                                                const std::set<int>& uniqueCapacities,
                                                std::vector<SelectedVenue>& temporaryFilteredVenues);

    const std::vector<SelectedVenue>& getFilteredResults() const { return temporaryFilteredVenues; }
};

#endif // FILTERCRITERIA_H
