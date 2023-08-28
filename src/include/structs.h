#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <sstream>

// Structure to represent a venue
struct Venue {
    std::string name;
    std::string email;
    std::string country;
    std::string state;
    std::string city;
    int capacity = 0;
    std::string genre;

    // Default constructor to allow for uninitialized Venue objects
    Venue() = default;

    // Parameterized constructor to initialize all members of the struct
    Venue(const std::string& name, const std::string& email, const std::string& country,
          const std::string& state, const std::string& city, int capacity, const std::string& genre)
        : name(name), email(email), country(country), state(state), city(city), capacity(capacity), genre(genre) {}
};


// Structure to represent a venue selected for email sending
struct SelectedVenue {
    std::string name;
    std::string email;
    std::string country;
    std::string state;
    std::string city;
    int capacity = 0;
    std::string genre;   

    // Default constructor to allow for uninitialized SelectedVenue objects
    SelectedVenue() = default;

    // Parameterized constructor to initialize all members of the struct
    SelectedVenue(const std::string& name, const std::string& email, const std::string& country,
                  const std::string& state, const std::string& city, int capacity, const std::string& genre)
                : name(name), email(email), country(country), state(state), city(city), capacity(capacity), genre(genre) {}
};
#endif // STRUCTS_H