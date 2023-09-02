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

const unsigned char DATABASE_AES_KEY[] = {
    0x85, 0x02, 0xb3, 0x28, 0x80, 0xad, 0x47, 0x27,
    0xe6, 0xdd, 0xeb, 0x43, 0x13, 0x52, 0x0f, 0x30,
    0x0d, 0x44, 0xbb, 0xbe, 0xfb, 0xce, 0x98, 0xef,
    0x61, 0xf4, 0x19, 0x76, 0x8a, 0x00, 0x95, 0xd5
};

const unsigned char DATABASE_AES_IV[] = {
    0x81, 0x8b, 0x67, 0x1c, 0xf2, 0xa7, 0x5c, 0x38,
    0x96, 0x08, 0xe2, 0x1c, 0xda, 0x7a, 0xaf, 0x84
};

const unsigned char REGISTRATION_AES_KEY[] = {
    0x19, 0x97, 0x54, 0x4a, 0x2e, 0x23, 0xfa, 0xa3, 
    0x0f, 0x1b, 0x29, 0xe0, 0xb9, 0x12, 0xaf, 0x1c, 
    0x83, 0xfc, 0x86, 0x16, 0xc8, 0xc6, 0x3a, 0xb3, 
    0xd9, 0x68, 0x56, 0xdf, 0x94, 0xed, 0xd9, 0x44
};

const unsigned char REGISTRATION_AES_IV[] = {
    0xff, 0x76, 0xd7, 0xe3, 0x68, 0xb5, 0x7f, 0x8c, 
    0x58, 0x26, 0x5a, 0x5e, 0x04, 0x6a, 0xc9, 0x12
};

// Namespace to hold configuration file paths
namespace confPaths {
string venuesCsvPath = "venues.csv";
string configJsonPath = "config.json";
string mockVenuesCsvPath = "src/test/mock_venues.csv";
string mockConfigJsonPath = "src/test/mock_config.json";
string sqliteEncryptedDatabasePath = "src/db/venues.db";
string registrationKeyPath = "registration_key.txt";
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

// Function to decrypt SQLite database using AES-256-CBC and store it in memory
bool VenueDatabaseReader::decryptRegistrationKey(const std::string& registrationKeyPath, std::vector<unsigned char>& decryptedRegistrationKeyData) {
    // Initialize OpenSSL
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        // Handle error
        std::cerr << "Failed to initialize OpenSSL context.\n";
        return false;
    }

    // Open the encrypted SQLite file
    std::ifstream encryptedRegistrationKeyFile(registrationKeyPath, std::ios::binary);
    if (!encryptedRegistrationKeyFile.is_open()) {
        // Handle error
        return false;
    }

    // Read encrypted data into a buffer
    std::vector<unsigned char> encryptedRegistrationKeyData((std::istreambuf_iterator<char>(encryptedRegistrationKeyFile)),
                                              std::istreambuf_iterator<char>());
    encryptedRegistrationKeyFile.close();

    // Prepare buffer to store decrypted data
    decryptedRegistrationKeyData.resize(encryptedRegistrationKeyData.size());

    // Decrypt the data
    int decryptedLen = 0;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, REGISTRATION_AES_KEY, REGISTRATION_AES_IV) != 1 ||
        EVP_DecryptUpdate(ctx, decryptedRegistrationKeyData.data(), &decryptedLen, encryptedRegistrationKeyData.data(), encryptedRegistrationKeyData.size()) != 1) {
        // Handle error
        return false;
    }

    int finalLen = 0;
    if (EVP_DecryptFinal_ex(ctx, decryptedRegistrationKeyData.data() + decryptedLen, &finalLen) != 1) {
        // Handle error
        return false;
    }

    decryptedRegistrationKeyData.resize(decryptedLen + finalLen);
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

// Function to decrypt SQLite database using AES-256-CBC and store it in memory
bool VenueDatabaseReader::decryptSQLiteDatabase(const std::string& encryptedFilePath, std::vector<unsigned char>& decryptedData) {
    // Initialize OpenSSL
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        // Handle error
        std::cerr << "Failed to initialize OpenSSL context.\n";
        return false;
    }

    // Open the encrypted SQLite file
    std::ifstream encryptedFile(encryptedFilePath, std::ios::binary);
    if (!encryptedFile.is_open()) {
        // Handle error
        return false;
    }

    // Read encrypted data into a buffer
    std::vector<unsigned char> encryptedData((std::istreambuf_iterator<char>(encryptedFile)),
                                              std::istreambuf_iterator<char>());
    encryptedFile.close();

    // Prepare buffer to store decrypted data
    decryptedData.resize(encryptedData.size());

    // Decrypt the data
    int decryptedLen = 0;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, DATABASE_AES_KEY, DATABASE_AES_IV) != 1 ||
        EVP_DecryptUpdate(ctx, decryptedData.data(), &decryptedLen, encryptedData.data(), encryptedData.size()) != 1) {
        // Handle error
        return false;
    }

    int finalLen = 0;
    if (EVP_DecryptFinal_ex(ctx, decryptedData.data() + decryptedLen, &finalLen) != 1) {
        // Handle error
        return false;
    }

    decryptedData.resize(decryptedLen + finalLen);
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

// Function to initialize SQLite and read data from CSV or encrypted database
bool VenueDatabaseReader::initializeDatabaseAndReadVenueData(std::vector<Venue>& venues, const std::string& venuesCsvPath) {
    bool success = false;

    std::vector<unsigned char> decryptedRegistrationKeyData;
    bool decryptionSuccess = decryptRegistrationKey(confPaths::registrationKeyPath, decryptedRegistrationKeyData);
    if (!decryptionSuccess) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::REGISTRATION_KEY_INVALID_ERROR);
        return false;
    }

    // Try to read from CSV first
    std::ifstream csvFile(venuesCsvPath);
    if (csvFile.is_open()) {
        readFromCsv(venues, csvFile);
        csvFile.close();
        success = true;
    }

    // Fallback to SQLite if reading from CSV fails
    if (!success) {
        // Decrypt the SQLite database and store it in memory
        std::vector<unsigned char> decryptedData;
        bool decryptionSuccess = decryptSQLiteDatabase(confPaths::sqliteEncryptedDatabasePath, decryptedData);
        if (!decryptionSuccess) {
            std::cerr << "Failed to decrypt SQLite database.\n";
            return false;
        }

        // Initialize an in-memory SQLite database from the decrypted data
        sqlite3* db = nullptr;
        if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
            std::cerr << "Failed to open in-memory SQLite database: " << sqlite3_errmsg(db) << "\n";
            return false;
        }

        // Allocate a separate buffer and copy the decrypted data into it
        unsigned char* sqliteBuffer = (unsigned char*) malloc(decryptedData.size());
        if (sqliteBuffer == nullptr) {
            std::cerr << "Failed to allocate memory for SQLite buffer.\n";
            return false;
        }
        std::copy(decryptedData.begin(), decryptedData.end(), sqliteBuffer);

        // Load the copied data into the in-memory SQLite database
        if (sqlite3_deserialize(db, "main", sqliteBuffer, decryptedData.size(), decryptedData.size(),
                                SQLITE_DESERIALIZE_RESIZEABLE | SQLITE_DESERIALIZE_FREEONCLOSE) != SQLITE_OK) {
            std::cerr << "Failed to load decrypted data into SQLite database: " << sqlite3_errmsg(db) << "\n";
            free(sqliteBuffer);  // Free the buffer if the operation fails
            return false;
        }

        // Read from the in-memory SQLite database
        readFromSQLite(venues, db);
        sqlite3_close(db);
        success = true;
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

    const char* query = "SELECT name, email, country, state, city, capacity, genre FROM \"venues\"";

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
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