#ifndef MENU_H
#define MENU_H

#include "encryption.h"
#include "filtercriteria.h"

// Function to validate user input
bool isValidMenuChoice(int choice);

// Function to display the menu to the user
int displayMenuOptions();

// Function to display the email settings set in config.json to the user
void viewEmailSettings(const std::string& smtpServer, int smtpPort, const std::string& senderEmail,
                       int senderSmtpPort, const std::string& smtpPassDecrypted, const std::string& mailPassDecrypted);

// Function to display selected venues to the user
void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

#endif // MENU_H