#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "fileutils.h"
#include "messagehandler.h"
#include "menutitlehandler.h"
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
    static SelectedVenueForEmails convertToSelectedVenueForEmails(const VenueForEmails& venueForEmails);

    // Utility function to convert a Venue object to a SelectedVenue object
    static SelectedVenueForTemplates convertToSelectedVenueForTemplates(const VenueForTemplates& venueForTemplates);

    // Utility function to convert a SelectedVenue object to a Venue object
    static VenueForEmails convertToVenueForEmails(const SelectedVenueForEmails& selectedVenueForEmails);

    // Utility function to convert a SelectedVenue object to a Venue object
    static VenueForTemplates convertToVenueForTemplates(const SelectedVenueForTemplates& selectedVenueForTemplates);

    // Utility function to get unique countries from a list of venues
    static std::set<std::string> getUniqueCountriesForEmails(const std::vector<VenueForEmails>& venuesForEmails);

    // Utility function to get unique states from a list of venues
    static std::set<std::string> getUniqueStatesForEmails(const std::vector<VenueForEmails>& venuesForEmails);

    // Utility function to get unique cities from a list of venues
    static std::set<std::string> getUniqueCitiesForEmails(const std::vector<VenueForEmails>& venuesForEmails);

    // Utility function to get unique capacities from a list of venues
    static std::set<int> getUniqueCapacitiesForEmails(const std::vector<VenueForEmails>& venuesForEmails);

    // Utility function to get unique genres from a list of venues
    static std::set<std::string> getUniqueGenresForEmails(const std::vector<VenueForEmails>& venuesForEmails);

    // Utility function to get unique options for a given filterType from a list of venues
    static std::variant<std::set<std::string>, std::set<int>> getUniqueOptionsForEmails(const std::vector<VenueForEmails>& venuesForEmails, const std::string& filterType);

    // Utility function to get unique countries from a list of venues
    static std::set<std::string> getUniqueCountriesForTemplates(const std::vector<VenueForTemplates>& venuesForTemplates);

    // Utility function to get unique states from a list of venues
    static std::set<std::string> getUniqueStatesForTemplates(const std::vector<VenueForTemplates>& venuesForTemplates);

    // Utility function to get unique cities from a list of venues
    static std::set<std::string> getUniqueCitiesForTemplates(const std::vector<VenueForTemplates>& venuesForTemplates);

    // Utility function to get unique capacities from a list of venues
    static std::set<int> getUniqueCapacitiesForTemplates(const std::vector<VenueForTemplates>& venuesForTemplates);

    // Utility function to get unique genres from a list of venues
    static std::set<std::string> getUniqueGenresForTemplates(const std::vector<VenueForTemplates>& venuesForTemplates);

    // Utility function to get unique options for a given filterType from a list of venues
    static std::variant<std::set<std::string>, std::set<int>> getUniqueOptionsForTemplates(const std::vector<VenueForTemplates>& venuesForTemplates, const std::string& filterType);
};

// Class to handle venue filtering logic
class VenueFilter {
private:
    // Utility to clear the temporary filtered venue vectors
    void clearTemporaryFilteredVenuesVectorsForEmails();

    // Utility to clear the temporary filtered venue vectors
    void clearTemporaryFilteredVenuesVectorsForTemplates();

    // Indices starting number
    static constexpr size_t INDICES_START_AT_ONE = 1;

    // Max length for venue selection indices input
    static constexpr size_t MAX_INPUT_LENGTH = 256;

    // Min length for venue selection indices input
    static constexpr size_t MIN_INPUT_LENGTH = 0;
   
    static constexpr int SQLITE_TABLE_INDEX = 1;

    // Delimiter used in CSV files
    static constexpr char CSV_DELIMITER = ','; // Delimiter used in CSV files

    // Max countries to display to prevent displaying the sqlite index names
    static constexpr std::string::size_type MAX_COUNTRIES_TO_DISPLAY = 2;

    VenueUtilities venueUtilities;

    std::vector<SelectedVenueForEmails> selectedVenuesForEmails;
    std::vector<SelectedVenueForTemplates> selectedVenuesForTemplates;
    std::vector<SelectedVenueForEmails> temporaryFilteredVenuesForEmails;
    std::vector<SelectedVenueForEmails> temporaryFilteredVenuesBufferForEmails;
    std::vector<SelectedVenueForTemplates> temporaryFilteredVenuesForTemplates;
    std::vector<SelectedVenueForTemplates> temporaryFilteredVenuesBufferForTemplates;

    std::set<std::string> uniqueOptions;
    std::string filterType;

public:

    // Function to process user input and select venues based on it
    void processVenueSelectionForEmails(const std::vector<VenueForEmails>& venues,
                               std::vector<SelectedVenueForEmails>& selectedVenuesForEmails,
                               std::istream& input = std::cin,
                               std::ostream& output = std::cout);

    // Function to process user input and select venues based on it
    void processVenueSelectionForTemplates(const std::vector<VenueForTemplates>& venuesForTemplates,
                               std::vector<SelectedVenueForTemplates>& selectedVenuesForTemplates,
                               std::istream& input = std::cin,
                               std::ostream& output = std::cout);

#ifdef UNIT_TESTING
    const std::vector<SelectedVenueForEmails>& getFilteredResultsForEmails() const { return temporaryFilteredVenuesForEmails; }
    const std::vector<SelectedVenueForTemplates>& getFilteredResultsForTemplates() const { return temporaryFilteredVenuesForTemplates; }
#endif
};

#endif // FILTERCRITERIA_H
