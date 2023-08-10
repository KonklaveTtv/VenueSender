#ifndef VENUEUTILS_H
#define VENUEUTILS_H

#include "venue.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

class Venue;

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

// Definition of getSelectedIndices
template<class T>
std::vector<int> getSelectedIndices(const std::vector<T>& options, std::istream& input);


#endif // VENUEUTILS_H
