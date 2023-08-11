#ifndef TEST_VENUESENDER_H
#define TEST_VENUESENDER_H

#include "../include/catch.hpp"
#include "venuesender.h"

#include <fstream>
#include <vector>

#include <json/json.h>

// Function to load the CSV path from config.json
std::string getVenuesCsvPathFromConfig();

// Test readCSV function
TEST_CASE("Read CSV file and populate data");

// Test isValidEmail function
TEST_CASE("Check if an email address is valid");

// Test getUniqueValues function
TEST_CASE("Get unique values from venues");

#endif // TEST_VENUESENDER_H
