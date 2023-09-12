#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "errorhandler.h"
#include "messagehandler.h"
#include "structs.h"

#ifndef UNIT_TESTING
#include "x11.h"
#endif

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <termios.h>  
#include <thread>
#include <unistd.h>
#include <vector>

// Boost Libraries
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/optional.hpp>
#include <boost/predef.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/scoped_array.hpp>
#include <boost/tokenizer.hpp>

// Include headers for AES decryption
#include <openssl/evp.h>
#include <openssl/sha.h>

// Include headers for SQLite
#include <sqlite3.h>

// Include headers for JSON processing
#include <json/json.h>

extern const unsigned char DATABASE_AES_KEY[];

extern const unsigned char DATABASE_AES_IV[];

extern const unsigned char REGISTRATION_AES_KEY[];

extern const unsigned char REGISTRATION_AES_IV[];

// Custom deleters for sqlite3 and sqlite3_stmt
struct Sqlite3Deleter {
    void operator()(sqlite3* db) const {
        sqlite3_close(db);
    }
};

struct Sqlite3StmtDeleter {
    void operator()(sqlite3_stmt* stmt) const {
        sqlite3_finalize(stmt);
    }
};

// Namespace to hold configuration file paths
namespace confPaths {
    // Declare external string variables for file paths
    extern std::string venuesCsvPath;
    extern std::string configJsonPath;
    extern std::string mockVenuesCsvPath;
    extern std::string mockConfigJsonPath;
    extern std::string sqliteEncryptedDatabasePath;
    extern std::string registrationKeyPath;
}

// Constant for calculating attachment size
constexpr int KIBIBYTES = 976.563;

constexpr int WHITESPACE_SIZE = 1;

// Class for utility functions related to the console
class ConsoleUtils {
public:
    // Email Passwork Min/Max Lengths
    static constexpr int MIN_PASSWORD_LENGTH = 1;
    static constexpr int MAX_PASSWORD_LENGTH = 100;

    // ASCII Constants
    static constexpr int ASCII_BACKSPACE = 8;
    static constexpr int ASCII_DELETE = 127;
    
    // Password Entry Constants
    static constexpr char PASSWORD_MASK = '*';

    // Console Constants
    static constexpr char WINDOWS_CLEAR_CONSOLE = "cls";
    static constexpr char MAC_LINUX_CLEAR_CONSOLE = "clear";

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
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Function to check a file exists at a given path
    static bool fileExists(const std::string& filename);

    // Method to trim leading and trailing spaces from a string
    static std::string trim(const std::string& str);

    // Method to clear the console
    static void clearConsole();

    // Method to check case-sensitivity of a string
    static bool caseSensitiveStringCompare(const std::string& str1, const std::string& str2);

    // Method to securely enter a password
    static std::string passwordEntry(bool& initColor);
};

// Class for reading data from a CSV file or encrypted database
class VenueDatabaseReader {
public:
    // Constants for CSV file reading
    static constexpr int CSV_TOTAL_ROW_COUNT = 7;
    static constexpr int CSV_NAME_ROW_NUMBER = 0;
    static constexpr int CSV_EMAIL_ROW_NUMBER = 1;
    static constexpr int CSV_COUNTRY_ROW_NUMBER = 2;
    static constexpr int CSV_STATE_ROW_NUMBER = 3;
    static constexpr int CSV_CITY_ROW_NUMBER = 4;
    static constexpr int CSV_CAPACITY_ROW_NUMBER = 5;
    static constexpr int CSV_GENRE_ROW_NUMBER = 6;

    // Constants for SQLite database reading
    static constexpr int SQLITE_NAME_ROW_NUMBER = 0;
    static constexpr int SQLITE_EMAIL_ROW_NUMBER = 1;
    static constexpr int SQLITE_COUNTRY_ROW_NUMBER = 2;
    static constexpr int SQLITE_STATE_ROW_NUMBER = 3;
    static constexpr int SQLITE_CITY_ROW_NUMBER = 4;
    static constexpr int SQLITE_CAPACITY_ROW_NUMBER = 5;
    static constexpr int SQLITE_GENRE_ROW_NUMBER = 6;

    // Constructor to initialize encryptedDatabase
    VenueDatabaseReader() = default;
    // Destructor
    ~VenueDatabaseReader() = default;

    static bool decryptSQLiteDatabase(const std::string& encryptedFilePath, std::vector<unsigned char>& decryptedData);

    static bool decryptRegistrationKey(const std::string& registrationKeyPath, std::vector<unsigned char>& decryptedRegistrationKeyData);

    // Function to initialize SQLite and read data from CSV or encrypted database
    static bool initializeDatabaseAndReadVenueData(std::vector<VenueForEmails>& venuesForEmails, std::vector<VenueForTemplates>& venuesForTemplates, const std::string& venuesCsvPath);

    // Function to read venue data from a CSV file
    static void readFromCsv(std::vector<VenueForEmails>& venuesForEmails, std::vector<VenueForTemplates>& venuesForTemplates, std::istream& stream);
             
    // Function to read venue data from an SQLite database
    static void readFromSQLite(std::vector<VenueForEmails>& venuesForEmails, std::vector<VenueForTemplates>& venuesForTemplates, sqlite3* db);
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
