#include "include/fileutils.h"

#include "errorhandler.h" // Include here due to circular dependency between fileutils.h and errorhandler.h

// For Linux (X11)
#ifdef __linux__
#include <X11/XKBlib.h>
#endif

// For macOS
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hidsystem/ev_keymap.h>
#endif

// For Windows
#ifdef _WIN32
#include <windows.h>
#endif

// Use the standard namespace
using namespace std;

// Namespace to hold configuration file paths
namespace confPaths {
string venuesCsvPath = "venues.csv";
string configJsonPath = "config.json";
string mockVenuesCsvPath = "src/test/mock_venues.csv";
string mockConfigJsonPath = "src/test/mock_config.json";
string sqliteDatabasePath = "src/db/venues.db";
}

void ConsoleUtils::clearConsole() {
    // Platform-specific code to clear the console
    // Windows:
#ifdef _WIN32
    system("cls");
#else 
    // Mac/Linux:
    system("clear");
#endif
}

bool ConsoleUtils::isCapsLockOn() {
#ifdef __linux__
    Display *d = XOpenDisplay((char*)nullptr);
    unsigned n;
    if (d) {
        XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
        XCloseDisplay(d);
        return (n & 0x01) == 1;
    }
    return false;
#elif defined(__APPLE__)
    io_service_t keyService = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleEmbeddedKeyboard"));
    if (keyService) {
        CFTypeRef state = IORegistryEntryCreateCFProperty(keyService, CFSTR(kIOHIDKeyboardCapsLockState), kCFAllocatorDefault, 0);
        if (state) {
            bool isOn = CFBooleanGetValue((CFBooleanRef)state);
            CFRelease(state);
            IOObjectRelease(keyService);
            return isOn;
        }
    }
    return false;
#elif defined(_WIN32)
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
#else
    return false; // Fallback for other platforms
#endif
}

std::string ConsoleUtils::passwordEntry(bool& initColor) {
    std::string password;
    std::string confirm;

    // Disable terminal echoing and enable manual input capture
    struct termios oldt, newt;
    memset(&oldt, 0, sizeof(oldt));
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::TERMINAL_GET_ATTRIBUTES_ERROR);
        return "";
    }
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::TERMINAL_SET_ATTRIBUTES_ERROR);
        return "";
    }

    while (true) {
        // Check for Caps Lock
        if (isCapsLockOn()) {
            std::cout << "Warning: Caps Lock is on!" << std::endl;
        }

        if (initColor) {
            std::cout << "Enter your email password: ";
        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE); // Orange for input
#endif
            std::cout << "Enter your email password: ";
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor(); // Reset color
#endif
        }

        password.clear();
        char ch;
        while (true) {
            ch = getchar();

            // Enter (newline) is the delimiter
            if (ch == '\n') {
                break;
            }

            // Handle backspace or delete
            if (ch == 127 || ch == '\b') {
                if (!password.empty()) {
                    std::cout << "\b \b";  // Move cursor back, overwrite with space, move cursor back again
                    password.pop_back();
                }
                continue;
            }

            // Check for password length
            if (password.length() >= MAX_PASSWORD_LENGTH) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MAX_LENGTH_ERROR);
                break;
            }

            // Add the character to the password
            password += ch;
            std::putchar('*');
        }

        // Check for minimum password length
        if (password.length() < MIN_PASSWORD_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MIN_LENGTH_ERROR);
            continue;
        }

        if (initColor) {
           std::cout << std::endl << "Confirm your email password: ";
        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE); // Orange for input
#endif
            std::cout << std::endl << "Confirm your email password: ";
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor(); // Reset color
#endif
        }
        confirm.clear();
        while (true) {
            ch = getchar();

            // Enter (newline) is the delimiter
            if (ch == '\n') {
                break;
            }

            // Handle backspace or delete
            if (ch == 127 || ch == '\b') {
                if (!confirm.empty()) {
                    std::cout << "\b \b";  // Move cursor back, overwrite with space, move cursor back again
                    confirm.pop_back();
                }
                continue;
            }

            // Check for password length
            if (confirm.length() >= MAX_PASSWORD_LENGTH) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MAX_LENGTH_ERROR);
                break;
            }

            // Add the character to the password
            confirm += ch;
            std::putchar('*');
        }

        if (password == confirm) {
            // Passwords match, exit the loop
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MISMATCH_ERROR);
        }
    }

    // If passwords match give confirmation
#ifndef UNIT_TESTING
    ConsoleUtils::setColor(ConsoleUtils::Color::GREEN); // Green for success
#endif
    std::cout << std::endl << "Password matches!" << std::endl;
#ifndef UNIT_TESTING
    ConsoleUtils::resetColor(); // Reset color
#endif
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ConsoleUtils::clearConsole();

    // Restore terminal settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) != 0) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::TERMINAL_RESTORE_ATTRIBUTES_ERROR);
    }

    std::cout << std::endl;  // Move to the next line after password entry
    return password;
}

// Default constructor for ConfigManager
ConfigManager::ConfigManager() = default;

// Function to load settings from a configuration file
bool ConfigManager::loadConfigSettings(bool& useSSL, bool& verifyPeer, bool& verifyHost, bool& verbose, 
                                       string& senderEmail, string& smtpUsername, 
                                       string& mailPass, int& smtpPort, string& smtpServer, 
                                       string& venuesCsvPath, bool& initColor) {


#ifdef UNIT_TESTING
    // Load plain text passwords from mock_config.json
    mailPass = "mock_email_password";
    initColor = true;
#else
    // Use the password entered by the user
    mailPass = ConsoleUtils::passwordEntry(initColor);
    initColor = false;
#endif

// Logic to read and load settings from a JSON file
// Includes conditional compilation for unit testing
// Validates the loaded settings and returns a boolean flag to indicate success or failure
#ifdef UNIT_TESTING
    Json::Value config;
    ifstream configFile(confPaths::mockConfigJsonPath);
    if (!configFile.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR);
        return false;
    }

    configFile >> config;

    // Load smtp user settings
    senderEmail = config["mock_sender_email"].asString();
    smtpPort = config["mock_smtp_port"].asInt();
    smtpServer = config["mock_smtp_server"].asString();
    smtpUsername = config["mock_smtp_username"].asString();

    // Load SSL settings
    useSSL = config["useSSL"].asBool();
    verifyPeer = config["verifyPeer"].asBool();
    verifyHost = config["verifyHost"].asBool();

    // Enable verbose for cURL
    verbose = config["verbose"].asBool();
#else
    Json::Value config;
    ifstream configFile(confPaths::configJsonPath);
    if (!configFile.is_open()) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR);
        return false;
    }

    configFile >> config;

    // Load smtp user settings
    senderEmail = config["sender_email"].asString();
    smtpPort = config["smtp_port"].asInt();
    smtpServer = config["smtp_server"].asString();
    smtpUsername = config["smtp_username"].asString();

    // Load SSL settings
    useSSL = config["useSSL"].asBool();
    verifyPeer = config["verifyPeer"].asBool();
    verifyHost = config["verifyHost"].asBool();

    // Enable verbose for cURL
    verbose = config["verbose"].asBool();
#endif

    // Define and initialize variables to track loaded settings
    bool smtpServerLoaded = !smtpServer.empty();
    bool smtpPortLoaded = smtpPort > 0;
    bool smtpUsernameLoaded = !smtpUsername.empty();
    bool venuesCsvPathLoaded = !venuesCsvPath.empty();
    bool mailPassLoaded = !mailPass.empty();
    bool senderEmailLoaded = !senderEmail.empty();

    // Check if the configuration settings are loaded successfully
    bool configLoadedSuccessfully = smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded &&
                                    venuesCsvPathLoaded && mailPassLoaded && senderEmailLoaded;

    // Display messages based on loaded settings
    if (smtpServerLoaded && smtpPortLoaded && smtpUsernameLoaded && venuesCsvPathLoaded && mailPassLoaded && senderEmailLoaded) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN); // Green for success
#endif
        cout << "Configuration from config.json Loaded" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset color
#endif
        configLoadedSuccessfully = true;
        // Clear the console for the Main Menu
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ConsoleUtils::clearConsole();
    } else {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED); // Red for error
#endif
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor(); // Reset color
#endif
    }
    return configLoadedSuccessfully;
}

// Function to check a file exists at a given path
bool ConsoleUtils::fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        if (filename != confPaths::venuesCsvPath) { // Add this line to prevent error for venues.csv
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, filename);
        }
        return false;
    }
    return file.good();
}

// Function to trim leading and trailing spaces from a string
string ConsoleUtils::trim(const string& str){
    // Trimming function
    const auto notSpace = [](int ch) {return !isspace(ch); };
    auto first = find_if(str.begin(), str.end(), notSpace);
    auto last = find_if(str.rbegin(), str.rend(), notSpace).base();
    return (first < last ? string(first, last) : string());
}

// Function to initialize SQLite and read data from CSV or encrypted database
bool VenueDatabaseReader::initializeDatabaseAndReadVenueData(std::vector<Venue>& venues, const std::string& venuesCsvPath) {
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
 void VenueDatabaseReader::readFromCsv(std::vector<Venue>& venues, std::istream& stream) {
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
void VenueDatabaseReader::readFromSQLite(std::vector<Venue>& venues, sqlite3* db) {
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