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

// Include for database
#include "../db/encrypted_database.h"

// Include headers for AES decryption
#include <openssl/aes.h>
#include <openssl/evp.h>

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
    extern std::string encryptedDatabasePath;
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
    static const bool debugMode =true;

    // Constructor to initialize encryptedDatabase
    VenueDatabaseReader() {
        encryptedDatabase.assign(confPaths::encryptedDatabasePath.begin(), confPaths::encryptedDatabasePath.end());
    }
    // Destructor
    ~VenueDatabaseReader() {};

    // Function to initialize SQLite and read data from CSV or encrypted database
    bool initializeDatabaseAndReadVenueData(std::vector<Venue>& venues, const std::string& venuesCsvPath) {

        bool success = false;
        if (ConsoleUtils::fileExists(venuesCsvPath)) {
            std::ifstream file(venuesCsvPath.c_str());
            if (file.is_open()) {
                readFromCsv(venues, file);
                file.close();
                success = true;
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, venuesCsvPath);
            }
        } else {
            try {
                sqlite3* db = initializeAndLoadDatabaseIntoMemory();
                readFromSQLite(venues, db);
                success = true;
            } catch (const ErrorHandlerException& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                success = false;
            }
        }
        if (debugMode) {
            std::cout << "Database initialization and reading completed. Success: " << success << std::endl;
        }
        return success;
    }

    // Function to decrypt and load database into memory, then wipe the decrypted data
    sqlite3* initializeAndLoadDatabaseIntoMemory() {
        std::vector<unsigned char> decryptedData = decryptDatabase(encryptedDatabase);
        sqlite3* db = loadDatabaseIntoMemory(decryptedData);

        std::fill(decryptedData.begin(), decryptedData.end(), 0);
        return db;
    }



    std::vector<unsigned char> base64_decode(std::string const& encoded_string) {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::vector<unsigned char> ret;

        while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i ==4) {
                for (i = 0; i <4; i++)
                    char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if (i) {
            for (j = i; j <4; j++)
                char_array_4[j] = 0;

            for (j = 0; j <4; j++)
                char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
        }

        return ret;
    }

    static inline bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }       

    // Function to decrypt the database
    std::vector<unsigned char> decryptDatabase(const std::vector<unsigned char>& encryptedDatabase) {
        if (debugMode) {
            std::cout << "Starting decryption of database" << std::endl;
        }

        if (encryptedDatabase.empty()) {
            std::cerr << "Error: Encrypted data is empty." << std::endl;
            return {};
        }

        std::vector<unsigned char> decoded_key = base64_decode("f4c8847c6bc95e65d1c1734cdce1fd3ee02a0909e33e8a09834f7b5e4409883d");
        std::vector<unsigned char> decoded_iv = base64_decode("6fe3b482ab7e0ca1572aee4236d13bea");

        // Step 1: AES decrypt the byte array
        std::vector<unsigned char> decryptedData(encryptedDatabase.size());
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            std::cerr << "Error: Could not create cipher context." << std::endl;
            return {};
        }

        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, decoded_key.data(), decoded_iv.data()) != 1) {
            std::cerr << "Error: Could not initialize decryption." << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return {};
        }

        int len;
        if (EVP_DecryptUpdate(ctx, decryptedData.data(), &len, encryptedDatabase.data(), encryptedDatabase.size()) != 1) {
            std::cerr << "Error: Could not update decryption." << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return {};
        }

        int paddingLen;
        if (EVP_DecryptFinal_ex(ctx, decryptedData.data() + len, &paddingLen) != 1) {
            std::cerr << "Error: Could not finalize decryption." << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return {};
        }

        decryptedData.resize(len + paddingLen);
        EVP_CIPHER_CTX_free(ctx);

        // Step 2: Convert the decrypted byte array back to a binary file (.bin)
        std::ofstream outFile("decrypted.bin", std::ios::binary);
        outFile.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
        outFile.close();

        // Step 3: AES decrypt the .bin file using OpenSSL
        std::string command = std::string("openssl enc -d -aes-256-cbc -in decrypted.bin -out decrypted_and_decoded.bin -K ") + "f4c8847c6bc95e65d1c1734cdce1fd3ee02a0909e33e8a09834f7b5e4409883d" + " -iv " + "6fe3b482ab7e0ca1572aee4236d13bea";
        if (system(command.c_str()) != 0) {
            std::cerr << "Error: OpenSSL decryption failed." << std::endl;
            return {};
        }

        // Step 4: Base64 decode the result to get the original SQLite database
        std::string decryptedString(decryptedData.begin(), decryptedData.end());
        std::vector<unsigned char> decodedData = base64_decode(decryptedString);

        if (decodedData.empty()) {
            ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::DATABASE_OPEN_ERROR, "Failed to decode decrypted database");
        }

        if (debugMode) {
            std::cout << "Decryption and decoding completed" << std::endl;
        }

        return decodedData;
    }

    // Function to read venue data from a CSV file
    static void readFromCsv(std::vector<Venue>& venues, std::istream& stream) {
        if (!stream) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR, confPaths::venuesCsvPath);
            return;
        }

        std::string line;
#ifndef UNIT_TESTING
        getline(stream, line); // Skip the header line
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
            } else {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_DATA_IN_CSV_ERROR, confPaths::venuesCsvPath);
            }
        }
    }

    // Function to read venue data from an SQLite database
    static void readFromSQLite(std::vector<Venue>& venues, sqlite3* db) {
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

    // Function to load database into memory
    sqlite3* loadDatabaseIntoMemory(const std::vector<unsigned char>& decryptedData) {
        sqlite3* db;
        if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
            std::cerr << "Error: Could not create in-memory SQLite database" << std::endl;
            return nullptr;
        }

        // Populate the in-memory SQLite database
        const char* tail;
        sqlite3_stmt* stmt;
        for (size_t offset = 0; offset < decryptedData.size(); ) {
            if (sqlite3_prepare_v2(db, (const char*) &decryptedData[offset], decryptedData.size() - offset, &stmt, &tail) != SQLITE_OK) {
                std::cerr << "Error: Could not prepare SQLite statement" << std::endl;
                sqlite3_close(db);
                return nullptr;
            }

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error: Could not execute SQLite statement" << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return nullptr;
            }

            sqlite3_finalize(stmt);
            offset = tail - (const char*) decryptedData.data();
        }

        return db;
    }

private:
    // Initialize encrypted database
    std::vector<unsigned char> encryptedDatabase;
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
