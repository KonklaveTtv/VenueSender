#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>

// Structure to represent a venue
struct Venue {
    std::string name;
    std::string email;
    std::string genre;
    std::string state;
    std::string city;
    int capacity = 0;

    // Default constructor to allow for uninitialized Venue objects
    Venue() = default;

    // Parameterized constructor to initialize all members of the struct
    Venue(const std::string& name, const std::string& email, const std::string& genre,
          const std::string& state, const std::string& city, int capacity)
        : name(name), email(email), genre(genre), state(state), city(city), capacity(capacity) {}
};


// Structure to represent a venue selected for email sending
struct SelectedVenue {
    std::string name;
    std::string email;
    std::string genre;
    std::string state;
    std::string city;
    int capacity = 0;

    // Default constructor to allow for uninitialized SelectedVenue objects
    SelectedVenue() = default;

    // Parameterized constructor to initialize all members of the struct
    SelectedVenue(const std::string& name, const std::string& email, const std::string& genre,
          const std::string& state, const std::string& city, int capacity)
        : name(name), email(email), genre(genre), state(state), city(city), capacity(capacity) {}
};
#endif // STRUCTS_H