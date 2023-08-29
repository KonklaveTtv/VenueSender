#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "errorhandler.h"
#include "structs.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <limits>
#include <sstream>
#include <termios.h>  
#include <thread>
#include <unistd.h>
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
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
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

    // Method to trim leading and trailing spaces from a string
    static std::string trim(const std::string& str);

    // Method to clear the console
    static void clearConsole();

    // Method to securely enter a password
    static std::string passwordEntry();
};

// Class for reading data from a CSV file
class CsvReader {
public:
    // Function to read venue data from a CSV file
    static void readCSV(std::vector<Venue>& venues, std::string& venuesCsvPath) {
        std::ifstream file(venuesCsvPath);
        if (!file.is_open()) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR, confPaths::venuesCsvPath);
            return;
        }

        std::string line;
        getline(file, line); // Skip the header line

        while (getline(file, line)) {
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
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_DATA_IN_CSV, venuesCsvPath);
            }
        }

        file.close();
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
                            std::string& venuesCsvPath);
};

#endif // FILEUTILS_H
