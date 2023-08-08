#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "venue.h"

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

// Function signatures for filter functions
std::vector<Venue> filterByGenre(const std::vector<Venue>& venues, const std::vector<std::string>& uniqueGenres, const FilterCriteria& criteria);
std::vector<Venue> filterByState(const std::vector<Venue>& venues, const std::vector<std::string>& uniqueStates, const FilterCriteria& criteria);
std::vector<Venue> filterByCity(const std::vector<Venue>& venues, const std::vector<std::string>& uniqueCities, const FilterCriteria& criteria);
std::vector<Venue> filterByCapacity(const std::vector<Venue>& venues, const std::vector<int>& uniqueCapacities, const FilterCriteria& criteria);

#endif // FILTERCRITERIA_H
