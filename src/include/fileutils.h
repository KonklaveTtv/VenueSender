#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "curl.h"
#include "encryption.h"
#include "structs.h"

#include <algorithm>
#include <fstream> 
#include <limits>
#include <sstream>
#include <vector>

#include "json/json.h"

namespace confPaths {
    const std::string venuesCsvPath = "venues.csv";
    const std::string configJsonPath = "config.json";
    const std::string mockVenuesCsvPath = "src/test/mock_venues.csv";
    const std::string mockConfigJsonPath = "src/test/mock_config.json";
}

// Function to trim leading and trailing spaces from a string
std::string trim(const std::string& str);

// Function to clear input buffer
void clearInputBuffer();

// Function to read CSV file and populate data
void readCSV(std::vector<Venue>& venues, const std::string& venuesCsvPath);

// Function to load the settings config.json data and encrypt and decrypt email/smtp passwords
bool loadConfigSettings(std::string& smtpServer, int& smtpPort,
                        std::string& smtpUsername, std::string& smtpPass,
                        std::string& venuesCsvPath, std::string& mailPass,
                        std::string& senderEmail, int& senderSmtpPort,
                        bool& useSSL, bool& verifyPeer, bool& verifyHost);

// Function to reset the config.json file password fields on exit
void resetConfigFile();

#endif // FILEUTILS_H