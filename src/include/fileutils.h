#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "encryption.h"
#include "errorhandler.h"
#include "structs.h"

#include <algorithm>
#include <fstream> 
#include <limits>
#include <vector>

#include <json/json.h>
    
namespace confPaths {
    extern std::string venuesCsvPath;
    extern std::string configJsonPath;
    extern std::string mockVenuesCsvPath;
    extern std::string mockConfigJsonPath;
}

class ConsoleUtils {
public:
    static void clearInputBuffer();
    static std::string trim(const std::string& str);
};

class CsvReader {
public:
    static void readCSV(std::vector<Venue>& venues, std::string& venuesCsvPath);
};

class ConfigManager {
private:
    EncryptionManager encryptionManager;

public:
    ConfigManager();
    bool loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost, bool& verbose, 
                            std::string& senderEmail, std::string& smtpUsername, 
                            std::string& mailPass, int& smtpPort, std::string& smtpServer, 
                            std::string& venuesCsvPath);

    static void resetConfigFile();
};

#endif // FILEUTILS_H
