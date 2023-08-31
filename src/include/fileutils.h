#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "errorhandler.h"
#include "structs.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <sstream>
#include <termios.h>  
#include <thread>
#include <unistd.h>
#include <vector>

// Include headers for SQLite
#include <sqlite3.h>

// Include headers for JSON processing
#include <jsoncpp/json/json.h>

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

// Namespace to hold configuration file paths
namespace confPaths {
    // Declare external string variables for file paths
    extern std::string venuesCsvPath;
    extern std::string configJsonPath;
    extern std::string mockVenuesCsvPath;
    extern std::string mockConfigJsonPath;
    extern std::string sqliteDatabasePath;
}

// Class for utility functions related to the console
class ConsoleUtils {
public:
    static inline const int MIN_PASSWORD_LENGTH = 1;
    static inline const int MAX_PASSWORD_LENGTH = 100;

#ifndef UNIT_TESTING
    enum class Color {
        RED,
        GREEN,
        BLUE,
        MAGENTA,
        YELLOW,
        ORANGE,
        CYAN,
        LIGHT_BLUE,
        PURPLE
    };

    static void setColor(ConsoleUtils::Color color) {
        // Platform-specific code to set console colors.
        // Windows:
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        switch(color) {
            case ConsoleUtils::Color::RED:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                break;
            case ConsoleUtils::Color::GREEN:
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
                break;
            case ConsoleUtils::Color::BLUE:
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
                break;
            case ConsoleUtils::Color::MAGENTA:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            case ConsoleUtils::Color::YELLOW:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
                break;
            case ConsoleUtils::Color::ORANGE:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            case ConsoleUtils::Color::CYAN:
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE);
                break;
            case ConsoleUtils::Color::LIGHT_BLUE:
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                break;
            case ConsoleUtils::Color::PURPLE:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE);
                break;
            default:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Default to white
                break;
        }
    #else
        // UNIX-like systems (using 256-color ANSI escape codes):
        switch(color) {
            case ConsoleUtils::Color::RED:
                std::cout << "\033[38;5;196m";
                break;
            case ConsoleUtils::Color::GREEN:
                std::cout << "\033[38;5;46m";
                break;
            case ConsoleUtils::Color::BLUE:
                std::cout << "\033[38;5;21m";
                break;
            case ConsoleUtils::Color::MAGENTA:
                std::cout << "\033[38;5;201m";
                break;
            case ConsoleUtils::Color::YELLOW:
                std::cout << "\033[38;5;226m";
                break;
            case ConsoleUtils::Color::ORANGE:
                std::cout << "\033[38;5;172m";
                break;
            case ConsoleUtils::Color::CYAN:
                std::cout << "\033[38;5;51m";
                break;
            case ConsoleUtils::Color::LIGHT_BLUE:
                std::cout << "\033[38;5;75m";
                break;
            case ConsoleUtils::Color::PURPLE:
                std::cout << "\033[38;5;93m";
                break;
            default:
                std::cout << "\033[0m"; // Reset to default
                break;
        }
    #endif
    }

    static void resetColor() {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset to white
    #else
        std::cout << "\033[0m"; // Reset to default for UNIX-like systems
    #endif
    }
#endif

    // Function to clear the input buffer
    inline static void clearInputBuffer() {
        // Clear the input buffer
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
    }

    // Function to check a file exists at a given path
    static bool fileExists(const std::string& filename);

    // Method to trim leading and trailing spaces from a string
    static std::string trim(const std::string& str);

    // Method to clear the console
    static void clearConsole();

    // Method to capture caps lock state
    static bool isCapsLockOn();

    // Method to securely enter a password
    static std::string passwordEntry(bool& initColor);
};

// Class for reading data from a CSV file or encrypted database
class VenueDatabaseReader {
public:
    // Constructor to initialize encryptedDatabase
    VenueDatabaseReader() {};
    // Destructor
    ~VenueDatabaseReader() {};

    // Function to initialize SQLite and read data from CSV or encrypted database
    bool initializeDatabaseAndReadVenueData(std::vector<Venue>& venues, const std::string& venuesCsvPath) {
        bool success = false;

        // Check if the venues.csv file exists
        bool venuesCsvExists = ConsoleUtils::fileExists(confPaths::venuesCsvPath);

        // Try to read from CSV first
        std::ifstream csvFile(venuesCsvPath);
        if (csvFile.is_open()) {
            readFromCsv(venues, csvFile);
            csvFile.close();
            success = true;
        }

        // Fallback to SQLite if reading from CSV fails
        if (!success) {
            // Check if the database file exists
            if (ConsoleUtils::fileExists(confPaths::sqliteDatabasePath)) {
                try {
                    sqlite3* db = nullptr;
                    int rc = sqlite3_open(confPaths::sqliteDatabasePath.c_str(), &db);
                    if (rc) {
                        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DATABASE_OPEN_ERROR, sqlite3_errmsg(db));
                        return false;
                    }
                    readFromSQLite(venues, db);
                    sqlite3_close(db);
                    success = true;
                } catch (const ErrorHandlerException& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                    success = false;
                }
            } else if (!venuesCsvExists) {
                // Only throw an error if both venues.csv and the SQLite database are missing
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, confPaths::sqliteDatabasePath);
            }
        }

        return success;
    }

    // Function to read venue data from a CSV file
    static void readFromCsv(std::vector<Venue>& venues, std::istream& stream) {
        // Check if the venues.csv file exists
        bool venuesCsvExists = ConsoleUtils::fileExists(confPaths::venuesCsvPath);

        if (!stream) {
            if (venuesCsvExists) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR, confPaths::venuesCsvPath);
            }
            return;
        }

        std::string line;
    #ifndef UNIT_TESTING
        getline(stream, line);  // Skip the header line
    #endif

        while (getline(stream, line)) {
            std::istringstream ss(line);
            std::string data;
            std::vector<std::string> rowData;

            while (getline(ss, data, ',')) {
                rowData.push_back(ConsoleUtils::trim(data));
            }

            if (rowData.size() == 7) {
                Venue venue;
                venue.name = rowData[0];
                venue.email = rowData[1];
                venue.country = rowData[2];
                venue.state = rowData[3];
                venue.city = rowData[4];
                venue.capacity = std::stoi(rowData[5]);
                venue.genre = rowData[6];
                venues.push_back(venue);
            } else if (venuesCsvExists) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_DATA_IN_CSV_ERROR, confPaths::venuesCsvPath);
            }
        }
    }
            

    // Function to read venue data from an SQLite database
    void readFromSQLite(std::vector<Venue>& venues, sqlite3* db) {
        int rc;
        bool shouldCloseDb = false;

        if (db == nullptr) {
            rc = sqlite3_open(":memory:", &db);
            if (rc) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DATABASE_OPEN_ERROR, "In-memory database");
                return;
            }
            shouldCloseDb = true;
        }

        const char* query = "SELECT name, email, country, state, city, capacity, genre FROM [venues]";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DATABASE_QUERY_ERROR, sqlite3_errmsg(db));
            if (shouldCloseDb) {
                sqlite3_close(db);
            }
            return;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            Venue venue;
            venue.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            venue.email = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            venue.country = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            venue.state = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            venue.city = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            venue.capacity = sqlite3_column_int(stmt, 5);
            venue.genre = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));

            venues.push_back(venue);
        }

        sqlite3_finalize(stmt);
        if (shouldCloseDb) {
            sqlite3_close(db);
        }
    }
};

// Class for managing configuration settings
class ConfigManager {
public:
    // Default constructor
    ConfigManager();

    // Method to load settings from a configuration file
    static bool loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost, bool& verbose,
                            std::string& senderEmail, std::string& smtpUsername, 
                            std::string& mailPass, int& smtpPort, std::string& smtpServer, 
                            std::string& venuesCsvPath, bool& initColor);
};

#endif // FILEUTILS_H
