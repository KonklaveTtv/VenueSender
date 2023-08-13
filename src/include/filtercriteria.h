#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "filtercriteria.h"
#include "venue.h"
#include "venuesender.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

class Venue;

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

// Convert Venue to SelectedVenue
SelectedVenue convertToSelectedVenue(const Venue& venue);

// Get a set of unique genres/states/cities/capacities from a vector of venues
std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues);
std::set<std::string> getUniqueStates(const std::vector<Venue>& venues);
std::set<std::string> getUniqueCities(const std::vector<Venue>& venues);
std::set<int> getUniqueCapacities(const std::vector<Venue>& venues);

// Declaration for getUniqueValues with a member pointer to a string
std::vector<std::string> getUniqueValues(const std::vector<Venue>& venues, std::string Venue::* memberPtr);

// Declaration for getUniqueValues with a member pointer to an integer
std::vector<int> getUniqueValues(const std::vector<Venue>& venues, int Venue::* memberPtr);

// Definition of getSelectedIndices for vector of any type
template<class T>
std::vector<int> getSelectedIndices(const std::vector<T>& options, std::istream& input) {
    // Get selected indices from user input and return as vector
    std::cout << "Enter the indices of options (comma-separated): ";
    std::string inputStr;
    std::getline(input, inputStr);
    std::istringstream iss(inputStr);
    std::string indexStr;
    std::vector<int> selectedIndices;
    while (std::getline(iss, indexStr, ',')) {
        int index = std::stoi(indexStr);
        if (index >= 1 && index <= static_cast<int>(options.size())) {
            selectedIndices.push_back(index - 1);
        }
    }
    return selectedIndices;
}

// Function to process user input and select venues
void processVenueSelection(const std::vector<SelectedVenue>& temporaryFilteredVenues,
                           std::vector<SelectedVenue>& selectedVenuesForEmail);
// Function to display filtered venues to the user
void displayFilteredVenues(const std::vector<SelectedVenue>& selectedVenuesForDisplay);

// Common function for filtering by an option (Genre, State, City)
std::vector<SelectedVenue> filterByOptionCommon(const std::vector<Venue>& venues,
                                                const std::set<std::string>& uniqueOptions,
                                                const std::string& filterType,
                                                std::vector<SelectedVenue>& temporaryFilteredVenues);

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

// Definition of getSelectedIndices
template<class T>
std::vector<int> getSelectedIndices(const std::vector<T>& options, std::istream& input);

#endif // FILTERCRITERIA_H