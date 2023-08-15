#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "mail.h"
#include "structs.h"

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
    string genre;
    string state;
    string city;
    int capacity = 0; // Change to int type
};

// Venue selection indices max char length
const int MAX_INPUT_LENGTH = 256;

// Definition of getSelectedIndices for vector of any type
template<class T>
vector<int> getSelectedIndices(const vector<T>& options, istream& input) {
    // Get selected indices from user input and return as vector
    cout << "Enter the indices of options (comma-separated): ";
    string inputStr;
    getline(input, inputStr);
    istringstream iss(inputStr);
    string indexStr;
    vector<int> selectedIndices;
    while (getline(iss, indexStr, ',')) {
        int index = stoi(indexStr);
        if (index >= 1 && index <= static_cast<int>(options.size())) {
            selectedIndices.push_back(index - 1);
        }
    }
    return selectedIndices;
}

// Convert Venue to SelectedVenue
SelectedVenue convertToSelectedVenue(const Venue& venue);

// Get a set of unique genres/states/cities/capacities from a vector of venues
set<string> getUniqueGenres(const vector<Venue>& venues);
set<string> getUniqueStates(const vector<Venue>& venues);
set<string> getUniqueCities(const vector<Venue>& venues);
set<int> getUniqueCapacities(const vector<Venue>& venues);

// Declaration for getUniqueValues with a member pointer to a string
vector<string> getUniqueValues(const vector<Venue>& venues, string Venue::* memberPtr);

// Declaration for getUniqueValues with a member pointer to an integer
vector<int> getUniqueValues(const vector<Venue>& venues, int Venue::* memberPtr);

// Function to process user input and select venues
void processVenueSelection(const vector<SelectedVenue>& temporaryFilteredVenues,
                           vector<SelectedVenue>& selectedVenuesForEmail,
                           istream& input = cin,
                           ostream& output = cout);

// Function to display filtered venues to the user
void displayFilteredVenues(const vector<SelectedVenue>& selectedVenuesForDisplay);

// Common function for filtering by an option (Genre, State, City)
vector<SelectedVenue> filterByOptionCommon(const vector<Venue>& venues,
                                                const set<string>& uniqueOptions,
                                                const string& filterType,
                                                vector<SelectedVenue>& temporaryFilteredVenues);

/**
 * Filters a vector of venues based on the selected option.
 *
 * @param venues The vector of Venue objects to filter.
 * @param optionType The type of option being filtered (e.g., "Genre", "State").
 * @param uniqueOptions A set of unique options for the selected type (e.g., unique genres, states).
 * @param filterValue The value of the selected option to filter by.
 * @return A vector of SelectedVenue objects that match the filter criteria.
 */
vector<SelectedVenue> filterByOption(const vector<Venue>& venues,
                                          const string& filterType,
                                          const set<string>& uniqueOptions,
                                          vector<SelectedVenue>& temporaryFilteredVenues);

/**
 * Filters a vector of venues based on the capacity.
 *
 * @param venues The vector of Venue objects to filter.
 * @param uniqueCapacities A set of unique capacity values.
 * @param filterValue The value of the selected capacity to filter by.
 * @return A vector of SelectedVenue objects that match the filter criteria.
 */
vector<SelectedVenue> filterByCapacity(const vector<Venue>& venues,
                                            const set<int>& uniqueCapacities,
                                            vector<SelectedVenue>& temporaryFilteredVenues);

#endif // FILTERCRITERIA_H
