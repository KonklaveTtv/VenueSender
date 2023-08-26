#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <sstream>

// Structure to represent a venue
struct Venue {
    std::string name;
    std::string email;
    std::string genre;
    std::string country;
    int capacity = 0;
    std::string state;
    std::string city;

    // Method to convert the struct content to a string
    std::string toString() const {
        std::ostringstream oss;
        oss << "Venue { "
            << "Name: " << name << ", "
            << "Email: " << email << ", "
            << "Genre: " << genre << ", "
            << "Country: " << country << ", "
            << "Capacity: " << capacity << ", "
            << "State: " << state << ", "
            << "City: " << city
            << " }";
        return oss.str();
    }

    // Default constructor to allow for uninitialized Venue objects
    Venue() = default;

    // Parameterized constructor to initialize all members of the struct
    Venue(const std::string& name, const std::string& email, const std::string& genre,
          const std::string& country, int capacity, const std::string& state, const std::string& city)
        : name(name), email(email), genre(genre), country(country), capacity(capacity), state(state), city(city) {}
};


// Structure to represent a venue selected for email sending
struct SelectedVenue {
    std::string name;
    std::string email;
    std::string genre;
    std::string country;
    int capacity = 0;
    std::string state;
    std::string city;    

    // Method to convert the struct content to a string
    std::string toString() const {
        std::ostringstream oss;
        oss << "SelectedVenue { "
            << "Name: " << name << ", "
            << "Email: " << email << ", "
            << "Genre: " << genre << ", "
            << "Country: " << country << ", "
            << "Capacity: " << capacity << ", "
            << "State: " << state << ", "
            << "City: " << city
            << " }";
        return oss.str();
    }

    // Default constructor to allow for uninitialized SelectedVenue objects
    SelectedVenue() = default;

    // Parameterized constructor to initialize all members of the struct
    SelectedVenue(const std::string& name, const std::string& email, const std::string& genre,
          const std::string& country, int capacity, const std::string& state, const std::string& city)
        : name(name), email(email), genre(genre), country(country), capacity(capacity), state(state), city(city) {}
};
#endif // STRUCTS_H