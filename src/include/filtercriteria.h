#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "venue.h"

#include <set>
#include <string>
#include <vector>

struct FilterCriteria {
    bool filterByGenre = false;
    std::string genre;
    bool filterByState = false;
    std::string state;
    bool filterByCity = false;
    std::string city;
    bool filterByCapacity = false;
    int capacity = 0;
};

// Function to allow the user to select venues from filtered results
std::vector<SelectedVenue> selectVenuesFromFilteredResults(const std::vector<SelectedVenue>& filteredVenues);

// Declaration for interactively filtering venues based on genre
void filterByGenre(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues, const std::vector<std::string>& uniqueGenres);

// Declaration for interactively filtering venues based on state
void filterByState(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues, const std::vector<std::string>& uniqueStates);

// Declaration for interactively filtering venues based on city
void filterByCity(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues);

// Declaration for interactively filtering venues based on capacity
void filterByCapacity(std::vector<SelectedVenue>& selectedVenuesForEmail, const std::vector<Venue>& venues);

// Function to filter venues based on the selected criteria
std::vector<SelectedVenue> filterVenues(const std::vector<Venue>& venues, const FilterCriteria& criteria);

// Function to filter selected venues based on user's selected criteria
std::vector<SelectedVenue> filterSelectedVenues(const std::vector<Venue>& venues, const FilterCriteria& selectedCriteria);

int getGenreIndexFromName(const std::string& genreName, const std::set<std::string>& uniqueGenres);
int getStateIndexFromName(const std::string& stateName, const std::set<std::string>& uniqueStates);
int getCityIndexFromName(const std::string& cityName, const std::set<std::string>& uniqueCities);

#endif // FILTERCRITERIA_H
