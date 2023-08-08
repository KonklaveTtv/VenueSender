#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "venuesender.h"

#include <fstream>

#include <json/json.h>

// Function to load the CSV path from config.json
std::string getVenuesCsvPathFromConfig() {
    Json::Value config;
    std::ifstream configFile("config.json");
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open config.json.");
    }

    configFile >> config;
    std::string venuesCsvPath = config["venues_csv_path"].asString();
    configFile.close();
    return venuesCsvPath;
}

// Test readCSV function
TEST_CASE("Read CSV file and populate data") {
    std::vector<Venue> venues;
    std::string venuesCsvPath = getVenuesCsvPathFromConfig();

    // Assuming venues.csv contains at least one venue entry
    readCSV(venues, venuesCsvPath);
    REQUIRE(!venues.empty());

    // Check properties of the first venue
    const Venue& firstVenue = venues[0];
    REQUIRE(firstVenue.name == "Venue 1");
    REQUIRE(firstVenue.email == "venue1@example.com");
    REQUIRE(firstVenue.genre == "Rock");
    REQUIRE(firstVenue.state == "State 1");
    REQUIRE(firstVenue.city == "City 1");
    REQUIRE(firstVenue.capacity == 100);
}

// Test isValidEmail function
TEST_CASE("Check if an email address is valid") {
    REQUIRE(isValidEmail("test@example.com"));
    REQUIRE(!isValidEmail("invalid_email"));
    // ... Add more test cases
}

// Test getUniqueValues function
TEST_CASE("Get unique values from venues") {
    // Initialize venues with sample data
    std::vector<Venue> venues = {
        {"Venue 1", "venue1@example.com", "Rock", "State 1", "City 1", 100},
        {"Venue 2", "venue2@example.com", "Pop", "State 2", "City 2", 150},
        {"Venue 3", "venue3@example.com", "Jazz", "State 3", "City 3", 200},
        {"Venue 4", "venue4@example.com", "Rock", "State 4", "City 4", 120}
    };

    // Get unique genres, cities, states, and capacities
    std::vector<std::string> uniqueGenres = getUniqueValues(venues, "genre");
    std::vector<std::string> uniqueStates = getUniqueValues(venues, "state");
    std::vector<std::string> uniqueCities = getUniqueValues(venues, "city");
    std::vector<std::string> uniqueCapacities = getUniqueValues(venues, "capacity");

    // Add assertions to check the uniqueness of values
    REQUIRE(uniqueGenres.size() == 3); // Rock, Pop, Jazz
    REQUIRE(std::find(uniqueGenres.begin(), uniqueGenres.end(), "Rock") != uniqueGenres.end());
    REQUIRE(std::find(uniqueGenres.begin(), uniqueGenres.end(), "Pop") != uniqueGenres.end());
    REQUIRE(std::find(uniqueGenres.begin(), uniqueGenres.end(), "Jazz") != uniqueGenres.end());

    REQUIRE(uniqueStates.size() == 3); // State 1, State 2, State 3
    REQUIRE(std::find(uniqueStates.begin(), uniqueStates.end(), "State 1") != uniqueStates.end());
    REQUIRE(std::find(uniqueStates.begin(), uniqueStates.end(), "State 2") != uniqueStates.end());
    REQUIRE(std::find(uniqueStates.begin(), uniqueStates.end(), "State 3") != uniqueStates.end());

    REQUIRE(uniqueCities.size() == 3); // City 1, City 2, City 3
    REQUIRE(std::find(uniqueCities.begin(), uniqueCities.end(), "City 1") != uniqueCities.end());
    REQUIRE(std::find(uniqueCities.begin(), uniqueCities.end(), "City 2") != uniqueCities.end());
    REQUIRE(std::find(uniqueCities.begin(), uniqueCities.end(), "City 3") != uniqueCities.end());

    REQUIRE(uniqueCapacities.size() == 4); // 100, 150, 200, 120
    REQUIRE(std::find(uniqueCapacities.begin(), uniqueCapacities.end(), "100") != uniqueCapacities.end());
    REQUIRE(std::find(uniqueCapacities.begin(), uniqueCapacities.end(), "150") != uniqueCapacities.end());
    REQUIRE(std::find(uniqueCapacities.begin(), uniqueCapacities.end(), "200") != uniqueCapacities.end());
    REQUIRE(std::find(uniqueCapacities.begin(), uniqueCapacities.end(), "120") != uniqueCapacities.end());
    // ... Add more test cases for other filtering functions if needed
}
