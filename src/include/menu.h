#ifndef MENU_H
#define MENU_H

#include "encryption.h"
#include "filtercriteria.h"

bool isValidMenuChoice(int choice);
int displayMenuOptions();
void displaySelectedVenues(const std::vector<SelectedVenue>& selectedVenues);

#endif // MENU_H