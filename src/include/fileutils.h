#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "encryption.h"
#include "structs.h"

#include <algorithm>
#include <fstream> 
#include <limits>
#include <vector>

// Include headers for JSON processing
#include <jsoncpp/json/json.h>

// Forward declaration to resolve circular dependency with errorhandler.h
class ErrorHandler; // Forward declaration due to circular dependency between fileutils.h and errorhandler.h

// Namespace to hold configuration file paths
namespace confPaths {
    // Declare external string variables for file paths
    extern std::string venuesCsvPath;
    extern std::string configJsonPath;
    extern std::string mockVenuesCsvPath;
    extern std::string mockConfigJsonPath;
}

// Class for utility functions related to the console
class ConsoleUtils {
public:
    // Method to clear the input buffer
    static void clearInputBuffer();

    // Method to trim leading and trailing spaces from a string
    static std::string trim(const std::string& str);
};

// Class for reading data from a CSV file
class CsvReader {
public:
    // Method to read CSV data and populate a vector of Venue objects
    static void readCSV(std::vector<Venue>& venues, std::string& venuesCsvPath);
};

// Class for managing configuration settings
class ConfigManager {
private:
    // Instance of EncryptionManager for password encryption/decryption
    EncryptionManager encryptionManager;

public:
    // Default constructor
    ConfigManager();

    // Method to load settings from a configuration file
    bool loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost, bool& verbose, 
                            std::string& senderEmail, std::string& smtpUsername, 
                            std::string& mailPass, int& smtpPort, std::string& smtpServer, 
                            std::string& venuesCsvPath);

    // Static method to reset the configuration file
    static void resetConfigFile();
};

#endif // FILEUTILS_H
