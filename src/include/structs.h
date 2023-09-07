#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <sstream>
#include <utility>

// Structure to represent a venue
struct VenueForEmails {
    std::string name;
    std::string email;
    std::string country;
    std::string state;
    std::string city;
    int capacity = 0;
    std::string genre;

    // Default constructor to allow for uninitialized Venue objects
    VenueForEmails() = default;

    // Parameterized constructor to initialize all members of the struct
    VenueForEmails(std::string  name, std::string  email, std::string  country,
          std::string  state, std::string  city, int capacity, std::string  genre)
          : name(std::move(name)), email(std::move(email)), country(std::move(country)), state(std::move(state)), city(std::move(city)), capacity(capacity), genre(std::move(genre)) {}
};


// Structure to represent a venue selected for email sending
struct SelectedVenueForEmails {
    std::string name;
    std::string email;
    std::string country;
    std::string state;
    std::string city;
    int capacity = 0;
    std::string genre;   

    // Default constructor to allow for uninitialized SelectedVenue objects
    SelectedVenueForEmails() = default;

    // Parameterized constructor to initialize all members of the struct
    SelectedVenueForEmails(std::string  name, std::string  email, std::string  country,
                  std::string  state, std::string  city, int capacity, std::string  genre)
                  : name(std::move(name)), email(std::move(email)), country(std::move(country)), state(std::move(state)), city(std::move(city)), capacity(capacity), genre(std::move(genre)) {}
};

// Structure to represent a venue
struct VenueForTemplates {
    std::string name;
    std::string email;
    std::string country;
    std::string state;
    std::string city;
    int capacity = 0;
    std::string genre;

    // Default constructor to allow for uninitialized Venue objects
    VenueForTemplates() = default;

    // Parameterized constructor to initialize all members of the struct
    VenueForTemplates(std::string  name, std::string  email, std::string  country,
          std::string  state, std::string  city, int capacity, std::string  genre)
          : name(std::move(name)), email(std::move(email)), country(std::move(country)), state(std::move(state)), city(std::move(city)), capacity(capacity), genre(std::move(genre)) {}
};


// Structure to represent a venue selected for email sending
struct SelectedVenueForTemplates {
    std::string name;
    std::string email;
    std::string country;
    std::string state;
    std::string city;
    int capacity = 0;
    std::string genre;   

    // Default constructor to allow for uninitialized SelectedVenue objects
    SelectedVenueForTemplates() = default;

    // Parameterized constructor to initialize all members of the struct
    SelectedVenueForTemplates(std::string  name, std::string  email, std::string  country,
                  std::string  state, std::string  city, int capacity, std::string  genre)
                  : name(std::move(name)), email(std::move(email)), country(std::move(country)), state(std::move(state)), city(std::move(city)), capacity(capacity), genre(std::move(genre)) {}
};
#endif // STRUCTS_H