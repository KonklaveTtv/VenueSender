#ifndef VENUE_H
#define VENUE_H

#include <string>

// Venue structure
struct Venue {
    string name;
    string email;
    string genre;
    string state;
    string city;
    int capacity;

    // Default constructor
    Venue() = default;

    // Parameterized constructor to initialize all members
    Venue(const string& name, const string& email, const string& genre,
          const string& state, const string& city, int capacity)
        : name(name), email(email), genre(genre), state(state), city(city), capacity(capacity) {}
};


// Selected Venue structure (used for email sending)
struct SelectedVenue {
    string name;
    string email;
    string genre;
    string state;
    string city;
    int capacity;

    // Default constructor
    SelectedVenue() = default;

    // Copy constructor to initialize from a Venue object
    SelectedVenue(const string& name, const string& email, const string& genre,
          const string& state, const string& city, int capacity)
        : name(name), email(email), genre(genre), state(state), city(city), capacity(capacity) {}
};
#endif // VENUE_H