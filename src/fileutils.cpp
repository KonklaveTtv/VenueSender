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

using Sqlite3Ptr = std::unique_ptr<sqlite3, Sqlite3Deleter>;
using Sqlite3StmtPtr = std::unique_ptr<sqlite3_stmt, Sqlite3StmtDeleter>;

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

bool ConsoleUtils::caseSensitiveStringCompare(const std::string& str1, const std::string& str2) {
    return boost::algorithm::equals(str1, str2);
}

bool ConsoleUtils::isCapsLockOn() {
#if BOOST_OS_LINUX
    Display *d = XOpenDisplay((char*)nullptr);
    unsigned n;
    if (d) {
        XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
        XCloseDisplay(d);
        return (n & 0x01) == 1;
    }
    return false;
#elif BOOST_OS_MACOS
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
#elif BOOST_OS_WINDOWS
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
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            std::cout << "Warning: Caps Lock is on!" << std::endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor(); // Reset color
#endif
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

        if (isCapsLockOn()) {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
            std::cout << "Warning: Caps Lock is on!" << std::endl;
#ifndef UNIT_TESTING
            ConsoleUtils::resetColor(); // Reset color
#endif
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

        if (ConsoleUtils::caseSensitiveStringCompare(password, confirm)) {
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
    bool isUnitTest = true;
#else
    bool isUnitTest = false;
#endif


    namespace pt = boost::property_tree;
    pt::ptree config;

    std::string configPath = isUnitTest ? confPaths::mockConfigJsonPath : confPaths::configJsonPath;

    if (!boost::filesystem::exists(configPath)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR);
        return false;
    }

    pt::read_json(configPath, config);

#ifdef UNIT_TESTING
    // Load plain text passwords from mock_config.json
    mailPass = "mock_email_password";
    initColor = true;
#else
    // Use the password entered by the user
    mailPass = ConsoleUtils::passwordEntry(initColor);
    initColor = false;
#endif

    // Load settings from the parsed JSON
    try {
        senderEmail = config.get<std::string>(isUnitTest ? "mock_sender_email" : "sender_email");
        smtpPort = config.get<int>(isUnitTest ? "mock_smtp_port" : "smtp_port");
        smtpServer = config.get<std::string>(isUnitTest ? "mock_smtp_server" : "smtp_server");
        smtpUsername = config.get<std::string>(isUnitTest ? "mock_smtp_username" : "smtp_username");
        venuesCsvPath = config.get<std::string>("venues_csv_path");

        useSSL = config.get<bool>("useSSL");
        verifyPeer = config.get<bool>("verifyPeer");
        verifyHost = config.get<bool>("verifyHost");
        verbose = config.get<bool>("verbose");
    } catch (pt::ptree_error& e) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        return false;
    }

    // Validation
    bool configLoadedSuccessfully = !(senderEmail.empty() || smtpServer.empty() || smtpUsername.empty() ||
                                      mailPass.empty() || smtpPort <= 0);

    if (configLoadedSuccessfully) {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
        std::cout << "Configuration from config.json Loaded" << std::endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ConsoleUtils::clearConsole();
    } else {
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
    }

    return configLoadedSuccessfully;
}

// Function to check a file exists at a given path
bool ConsoleUtils::fileExists(const std::string& filename) {
    boost::filesystem::path filePath(filename);
    if (!boost::filesystem::exists(filePath)) {
        if (filename != confPaths::venuesCsvPath) { // Add this line to prevent error for venues.csv
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, filename);
        }
        return false;
    }
    return true;
}

// Function to trim leading and trailing spaces from a string
string ConsoleUtils::trim(const string& str){
    return boost::algorithm::trim_copy(str);
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

    // Check if the file exists using Boost Filesystem
    if (!boost::filesystem::exists(registrationKeyPath)) {
        // Handle error
        return false;
    }

    // Map the encrypted SQLite file into memory using Boost Iostreams
    boost::iostreams::mapped_file_source file(registrationKeyPath);

    // Read encrypted data into a buffer
    std::vector<unsigned char> encryptedRegistrationKeyData(file.begin(), file.end());

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
        std::cerr << "Failed to initialize OpenSSL context.\n";
        return false;
    }

    // Map the encrypted SQLite file into memory
    boost::iostreams::mapped_file_source mappedFile;
    mappedFile.open(encryptedFilePath);
    if (!mappedFile.is_open()) {
        return false;
    }

    // Read encrypted data into a buffer
    std::vector<unsigned char> encryptedData(mappedFile.begin(), mappedFile.end());

    // Prepare buffer to store decrypted data
    decryptedData.resize(encryptedData.size());

    // Decrypt the data
    int decryptedLen = 0;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, DATABASE_AES_KEY, DATABASE_AES_IV) != 1 ||
        EVP_DecryptUpdate(ctx, decryptedData.data(), &decryptedLen, encryptedData.data(), encryptedData.size()) != 1) {
        return false;
    }

    int finalLen = 0;
    if (EVP_DecryptFinal_ex(ctx, decryptedData.data() + decryptedLen, &finalLen) != 1) {
        return false;
    }

    decryptedData.resize(decryptedLen + finalLen);
    EVP_CIPHER_CTX_free(ctx);

    return true;
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
        boost::tokenizer<boost::escaped_list_separator<char>> tok(line);
        std::vector<std::string> rowData(tok.begin(), tok.end());

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
    boost::iostreams::mapped_file_source csvMappedFile;
    csvMappedFile.open(venuesCsvPath);
    if (csvMappedFile.is_open()) {
        // Create a string from the iterators and then a stringstream
        std::string csvData(csvMappedFile.begin(), csvMappedFile.end());
        std::istringstream csvStream(csvData);
        readFromCsv(venues, csvStream);
        csvMappedFile.close();
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

        // Use boost::scoped_array for memory management
        boost::scoped_array<unsigned char> sqliteBuffer(new unsigned char[decryptedData.size()]);
        std::copy(decryptedData.begin(), decryptedData.end(), sqliteBuffer.get());

        // Load the copied data into the in-memory SQLite database
        if (sqlite3_deserialize(db, "main", sqliteBuffer.get(), decryptedData.size(), decryptedData.size(),
                                SQLITE_DESERIALIZE_RESIZEABLE | SQLITE_DESERIALIZE_FREEONCLOSE) != SQLITE_OK) {
            std::cerr << "Failed to load decrypted data into SQLite database: " << sqlite3_errmsg(db) << "\n";
            return false;
        }

        // Read from the in-memory SQLite database
        readFromSQLite(venues, db);
        sqlite3_close(db);
        success = true;
    }

    return success;
}

// Function to read venue data from an SQLite database
void VenueDatabaseReader::readFromSQLite(std::vector<Venue>& venues, sqlite3* db) {
    Sqlite3Ptr dbPtr(nullptr, Sqlite3Deleter());

    if (db == nullptr) {
        sqlite3* tempDb;
        if (sqlite3_open(":memory:", &tempDb) != SQLITE_OK) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DATABASE_OPEN_ERROR, "In-memory database");
            return;
        }
        dbPtr.reset(tempDb);
    }

    const char* query = "SELECT name, email, country, state, city, capacity, genre FROM \"venues\"";
    sqlite3_stmt* tempStmt;
    if (sqlite3_prepare_v2(db, query, -1, &tempStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    Sqlite3StmtPtr stmtPtr(tempStmt, Sqlite3StmtDeleter());

    while (sqlite3_step(stmtPtr.get()) == SQLITE_ROW) {
        Venue venue;
        venue.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), 0)));
        venue.email = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), 1)));
        venue.country = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), 2)));
        venue.state = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), 3)));
        venue.city = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), 4)));
        venue.capacity = sqlite3_column_int(stmtPtr.get(), 5);
        venue.genre = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), 6)));

        venues.push_back(venue);
    }

    // Resources are automatically released when the unique_ptr's go out of scope.
}