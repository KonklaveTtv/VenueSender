#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "encryption.h"
#include "structs.h"

#include <algorithm>
#include <fstream> 
#include <limits>
#include <sstream>
#include <vector>

#include "json/json.h"

// Function to trim leading and trailing spaces from a string
std::string trim(const std::string& str);

// Function to clear input buffer
void clearInputBuffer();

// Function to read CSV file and populate data
void readCSV(std::vector<Venue>& venues, const std::string& venuesCsvPath);

// Function to load the settings config.json data and encrypt and decrypt email/smtp passwords
bool loadConfigSettings(const std::string& configFilePath,
                        std::string& smtpServer, int& smtpPort,
                        std::string& smtpUsername, std::string& smtpPass,
                        std::string& venuesCsvPath, std::string& mailPass,
                        std::string& senderEmail, int& senderSmtpPort);

// Function to reset the config.json file password fields on exit
void resetConfigFile(const std::string& configFilePath);

#endif // FILEUTILS_H