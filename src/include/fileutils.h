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

class ConsoleUtils {
public:
    static void clearInputBuffer();
    static void clearConsole();
    static std::string trim(const std::string& str);
};

class CsvReader {
public:
    static void readCSV(std::vector<Venue>& venues, const std::string& venuesCsvPath);
};

class ConfigManager {
private:
    EncryptionManager encryptionManager;

public:
    ConfigManager();
    bool loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost,
                            std::string& senderEmail, std::string& smtpUsername,
                            std::string& mailPass, int& smtpPort, std::string& smtpServer,
                            std::string& venuesCsvPath);
    void resetConfigFile();
};

#endif // FILEUTILS_H
