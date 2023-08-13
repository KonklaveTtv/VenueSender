#ifndef MAIN_H
#define MAIN_H

#include "encryption.h"
#include "filtercriteria.h"

bool isValidMenuChoice(int choice);
void viewEmailSettings(const std::string& smtpServer, int smtpPort, const std::string& senderEmail,
                       int senderSmtpPort, const std::string& smtpPassDecrypted, const std::string& mailPassDecrypted);
int displayMenuOptions();
void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

#endif // MAIN_H