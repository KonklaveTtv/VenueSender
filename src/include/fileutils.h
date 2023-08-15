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

namespace confPaths {
    const string venuesCsvPath = "venues.csv";
    const string configJsonPath = "config.json";
    const string mockVenuesCsvPath = "src/test/mock_venues.csv";
    const string mockConfigJsonPath = "src/test/mock_config.json";
}

// Function to trim leading and trailing spaces from a string
string trim(const string& str);

// Function to clear input buffer
void clearInputBuffer();

// Function to read CSV file and populate data
void readCSV(vector<Venue>& venues, const string& venuesCsvPath);

// Function to load the settings config.json data and encrypt and decrypt email/smtp passwords
bool loadConfigSettings(string& smtpServer, int& smtpPort,
                        string& smtpUsername, string& smtpPass,
                        string& venuesCsvPath, string& mailPass,
                        string& senderEmail, int& senderSmtpPort,
                        bool& useSSL, bool& verifyPeer, bool& verifyHost);

// Function to reset the config.json file password fields on exit
void resetConfigFile();

#endif // FILEUTILS_H