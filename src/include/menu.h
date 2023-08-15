#ifndef MENU_H
#define MENU_H

#include "encryption.h"
#include "filtercriteria.h"

// Function to validate user input
bool isValidMenuChoice(int choice);

// Function to display the menu to the user
int displayMenuOptions();

// Function to display the email settings set in config.json to the user
void viewEmailSettings(const string& smtpServer, int smtpPort, const string& senderEmail,
                       int senderSmtpPort, const string& smtpPassDecrypted, const string& mailPassDecrypted,
                       bool useSSL);

// Function to display selected venues to the user
void displaySelectedVenues(const vector<SelectedVenue>& selectedVenues);

#endif // MENU_H