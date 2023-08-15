#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

// Venue structure
struct Venue {
    std::string name;
    std::string email;
    std::string genre;
    std::string state;
    std::string city;
    int capacity;

    // Default constructor
    Venue() = default;

    // Parameterized constructor to initialize all members
    Venue(const std::string& name, const std::string& email, const std::string& genre,
          const std::string& state, const std::string& city, int capacity)
        : name(name), email(email), genre(genre), state(state), city(city), capacity(capacity) {}
};


// Selected Venue structure (used for email sending)
struct SelectedVenue {
    std::string name;
    std::string email;
    std::string genre;
    std::string state;
    std::string city;
    int capacity;

    // Default constructor
    SelectedVenue() = default;

    // Copy constructor to initialize from a Venue object
    SelectedVenue(const std::string& name, const std::string& email, const std::string& genre,
          const std::string& state, const std::string& city, int capacity)
        : name(name), email(email), genre(genre), state(state), city(city), capacity(capacity) {}
};
#endif // STRUCTS_H