#ifndef VENUEUTILS_H
#define VENUEUTILS_H

#include "venue.h"

#include <vector>
#include <string>
#include <algorithm>

class Venue;

// Declaration for getUniqueValues with a member pointer to a string
std::vector<std::string> getUniqueValues(const std::vector<Venue>& venues, std::string Venue::* memberPtr);

// Declaration for getUniqueValues with a member pointer to an integer
std::vector<int> getUniqueValues(const std::vector<Venue>& venues, int Venue::* memberPtr);

#endif // VENUEUTILS_H
