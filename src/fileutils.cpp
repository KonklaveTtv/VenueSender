#include "include/fileutils.h"

#include "errorhandler.h" // Include here due to circular dependency between fileutils.h and errorhandler.h

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

using Sqlite3Ptr = unique_ptr<sqlite3, Sqlite3Deleter>;
using Sqlite3StmtPtr = unique_ptr<sqlite3_stmt, Sqlite3StmtDeleter>;

// Namespace to hold configuration file paths
namespace confPaths {
string venuesCsvPath = "db/venues.csv";
string configJsonPath = "config.json";
string mockVenuesCsvPath = "src/test/mock_venues.csv";
string mockConfigJsonPath = "src/test/mock_config.json";
string sqliteEncryptedDatabasePath = "db/venues.db";
string registrationKeyPath = "registration_key";
}

void ConsoleUtils::clearConsole() {
    // Platform-specific code to clear the console
    // Windows:
#ifdef _WIN32
    system(WINDOWS_CLEAR_CONSOLE);
#else 
    // Mac/Linux:
    system(MAC_LINUX_CLEAR_CONSOLE);
#endif
}

bool ConsoleUtils::caseSensitiveStringCompare(const string& str1, const string& str2) {
    return (str1 == str2);
}

string ConsoleUtils::passwordEntry(bool& initColor) {
    string password;
    string confirm;

    // Disable terminal echoing and enable manual input capture
    #ifdef _WIN32
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hStdin, &mode);
        SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    #else
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
    #endif

    while (true) {
#ifndef UNIT_TESTING
        bool isOn = false;

        #ifdef __linux__
            // Initialize X11 for Linux
            X11Singleton& x11 = X11Singleton::getInstance();
            x11.openDisplay();
            isOn = x11.isCapsLockOn();
            x11.closeDisplay();
        #elif defined(_WIN32)
            // Check for Caps Lock on Windows
            SHORT state = GetKeyState(VK_CAPITAL);
            isOn = (state & CAPS_LOCK) != 0;
        #elif defined(__APPLE__)
            // Check for Caps Lock on macOS
            #if MACOS_MONTEREY_OR_NEWER
                isOn = isMacOSCapsLockOn();
            #else
                X11Singleton& x11 = X11Singleton::getInstance();
                x11.openDisplay();
                isOn = x11.isCapsLockOn();
                x11.closeDisplay();
            #endif
        #endif

        if (isOn) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::X11_CAPS_LOCK_ERROR);
            cin.clear();
        }
#endif
        if (initColor) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::INIT_ENTER_PASSWORD_MESSAGE);
            cin.clear();
        } else {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_CONFIG_ENTER_PASSWORD_MESSAGE);
            cin.clear();
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
            if (ch == ASCII_BACKSPACE || ch == ASCII_DELETE) {
                if (!password.empty()) {
                    cout << "\b \b";  // Move cursor back, overwrite with space, move cursor back again
                    password.pop_back();
                }
                continue;
            }

            // Check for password length
            if (password.length() >= MAX_PASSWORD_LENGTH) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MAX_LENGTH_ERROR);
                cin.clear();
                break;
            }

            password += ch;

            if (initColor) {
                putchar(PASSWORD_MASK);
            } else {
#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                putchar(PASSWORD_MASK);
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor(); // Reset color
#endif
            }
        }

        // Check for minimum password length
        if (password.length() < MIN_PASSWORD_LENGTH) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MIN_LENGTH_ERROR);
            cin.clear();
            continue;
        }

        if (initColor) {
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::INIT_CONFIRM_PASSWORD_MESSAGE);
            cin.clear();
        } else {
#ifndef UNIT_TESTING
            ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE); // Orange for input
#endif
            MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::EMAIL_CONFIG_CONFIRM_PASSWORD_MESSAGE);
            cin.clear();
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
            if (ch == ASCII_BACKSPACE || ch == ASCII_DELETE) {
                if (!confirm.empty()) {
                    cout << "\b \b";  // Move cursor back, overwrite with space, move cursor back again
                    confirm.pop_back();
                }
                continue;
            }

            // Check for password length
            if (confirm.length() >= MAX_PASSWORD_LENGTH) {
                ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MAX_LENGTH_ERROR);
                cin.clear();
                break;
            }

            confirm += ch;

            if (initColor) {
                putchar(PASSWORD_MASK);
            } else {

#ifndef UNIT_TESTING
                ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif
                putchar(PASSWORD_MASK);
#ifndef UNIT_TESTING
                ConsoleUtils::resetColor(); // Reset color
#endif
            }
        }

        if (ConsoleUtils::caseSensitiveStringCompare(password, confirm)) {
            // Passwords match, exit the loop
            break;
        } else {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_MISMATCH_ERROR);
            cin.clear();
        }
    }
    
    // If passwords match give confirmation
    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::PASSWORD_MATCHES_MESSAGE);
    this_thread::sleep_for(chrono::seconds(1));
    ConsoleUtils::clearConsole();

    // Restore terminal settings
    #ifdef _WIN32
        SetConsoleMode(hStdin, mode);
    #else
        if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) != 0) {
            ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::TERMINAL_RESTORE_ATTRIBUTES_ERROR);
        }
    #endif

    cout << endl;  // Move to the next line after password entry
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

    string configPath = isUnitTest ? confPaths::mockConfigJsonPath : confPaths::configJsonPath;

    if (!boost::filesystem::exists(configPath)) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR);
        cin.clear();
        return false;
    }

    pt::read_json(configPath, config);

#ifdef UNIT_TESTING
    // Load plain text passwords from mock_config.json
    mailPass = "mock_email_password";
    initColor = true;
#else
    // Use the password entered by the user
    initColor = true;
    mailPass = ConsoleUtils::passwordEntry(initColor);
    initColor = false;
#endif

    // Load settings from the parsed JSON
    try {
        senderEmail = config.get<string>(isUnitTest ? "mock_sender_email" : "sender_email");
        smtpPort = config.get<int>(isUnitTest ? "mock_smtp_port" : "smtp_port");
        smtpServer = config.get<string>(isUnitTest ? "mock_smtp_server" : "smtp_server");
        smtpUsername = config.get<string>(isUnitTest ? "mock_smtp_username" : "smtp_username");
        venuesCsvPath = config.get<string>("venues_csv_path");

        useSSL = config.get<bool>("useSSL");
        verifyPeer = config.get<bool>("verifyPeer");
        verifyHost = config.get<bool>("verifyHost");
        verbose = config.get<bool>("verbose");
    } catch (pt::ptree_error& e) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        cin.clear();
        return false;
    }

    // Validation
    bool configLoadedSuccessfully = !(senderEmail.empty() || smtpServer.empty() || smtpUsername.empty() ||
                                      mailPass.empty() || smtpPort <= 0);

    if (configLoadedSuccessfully) {
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CONFIG_JSON_LOADED_MESSAGE);
        cin.clear();
        this_thread::sleep_for(chrono::seconds(1));
    } else {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_LOAD_ERROR);
        cin.clear();
    }

    return configLoadedSuccessfully;
}

// Function to check a file exists at a given path
bool ConsoleUtils::fileExists(const string& filename) {
    boost::filesystem::path filePath(filename);
    if (!boost::filesystem::exists(filePath)) {
        if (filename != confPaths::venuesCsvPath) { // Add this line to prevent error for venues.csv
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::FILESYSTEM_ERROR, filename);
            cin.clear();
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
bool VenueDatabaseReader::decryptRegistrationKey(const string& registrationKeyPath, vector<unsigned char>& decryptedRegistrationKeyData) {

    // Initialize OpenSSL
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::OPENSSL_INITIALIZATION_ERROR);
        cin.clear();
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
    vector<unsigned char> encryptedRegistrationKeyData(file.begin(), file.end());

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

    MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::REGISTRATION_KEY_VALID_MESSAGE);
    this_thread::sleep_for(chrono::seconds(1));

    return true;
}

// Function to decrypt SQLite database using AES-256-CBC and store it in memory
bool VenueDatabaseReader::decryptSQLiteDatabase(const string& encryptedFilePath, vector<unsigned char>& decryptedData) {
    // Initialize OpenSSL
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::OPENSSL_INITIALIZATION_ERROR);
        cin.clear();
        return false;
    }

    // Map the encrypted SQLite file into memory
    boost::iostreams::mapped_file_source mappedFile;
    mappedFile.open(encryptedFilePath);
    if (!mappedFile.is_open()) {
        return false;
    }

    // Read encrypted data into a buffer
    vector<unsigned char> encryptedData(mappedFile.begin(), mappedFile.end());

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
void VenueDatabaseReader::readFromCsv(vector<VenueForEmails>& venuesForEmails, vector<VenueForTemplates>& venuesForTemplates,istream& stream) {
    // Check if the venues.csv file exists
    bool venuesCsvExists = ConsoleUtils::fileExists(confPaths::venuesCsvPath);

    if (!stream) {
        if (venuesCsvExists) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::CONFIG_OPEN_ERROR, confPaths::venuesCsvPath);
            cin.clear();
        }
        return;
    }

    string line;
#ifndef UNIT_TESTING
    getline(stream, line);  // Skip the header line
#endif

    while (getline(stream, line)) {
        boost::tokenizer<boost::escaped_list_separator<char>> tok(line);
        vector<string> rowData(tok.begin(), tok.end());

        if (rowData.size() == CSV_TOTAL_ROW_COUNT) {
            VenueForEmails venueForEmails;
            venueForEmails.name = rowData[CSV_NAME_ROW_NUMBER];
            venueForEmails.email = rowData[CSV_EMAIL_ROW_NUMBER];
            venueForEmails.country = rowData[CSV_COUNTRY_ROW_NUMBER];
            venueForEmails.state = rowData[CSV_STATE_ROW_NUMBER];
            venueForEmails.city = rowData[CSV_CITY_ROW_NUMBER];
            venueForEmails.capacity = stoi(rowData[CSV_CAPACITY_ROW_NUMBER]);
            venueForEmails.genre = rowData[CSV_GENRE_ROW_NUMBER];
            venuesForEmails.push_back(venueForEmails);
        } else if (venuesCsvExists) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_DATA_IN_CSV_ERROR, confPaths::venuesCsvPath);
            cin.clear();
        }
        if (rowData.size() == CSV_TOTAL_ROW_COUNT) {
            VenueForTemplates venueForTemplates;
            venueForTemplates.name = rowData[CSV_NAME_ROW_NUMBER];
            venueForTemplates.email = rowData[CSV_EMAIL_ROW_NUMBER];
            venueForTemplates.country = rowData[CSV_COUNTRY_ROW_NUMBER];
            venueForTemplates.state = rowData[CSV_STATE_ROW_NUMBER];
            venueForTemplates.city = rowData[CSV_CITY_ROW_NUMBER];
            venueForTemplates.capacity = stoi(rowData[CSV_CAPACITY_ROW_NUMBER]);
            venueForTemplates.genre = rowData[CSV_GENRE_ROW_NUMBER];
            venuesForTemplates.push_back(venueForTemplates);
        } else if (venuesCsvExists) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::INVALID_DATA_IN_CSV_ERROR, confPaths::venuesCsvPath);
            cin.clear();
        }
    }
}

// Function to initialize SQLite and read data from CSV or encrypted database
bool VenueDatabaseReader::initializeDatabaseAndReadVenueData(vector<VenueForEmails>& venuesForEmails, vector<VenueForTemplates>& venuesForTemplates, const string& venuesCsvPath) {
    bool success = false;

    vector<unsigned char> decryptedRegistrationKeyData;
    bool decryptionSuccess = decryptRegistrationKey(confPaths::registrationKeyPath, decryptedRegistrationKeyData);
    if (!decryptionSuccess) {
        ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::REGISTRATION_KEY_INVALID_ERROR);
        cin.clear();
        return false;
    }

    // Try to read from CSV first
    ifstream csvFile(venuesCsvPath);
    if (csvFile.is_open()) {
        readFromCsv(venuesForEmails, venuesForTemplates, csvFile);
        csvFile.close();
        success = true;
        MessageHandler::handleMessageAndReturn(MessageHandler::MessageType::CSV_DATABASE_LOADED_MESSAGE);
        cin.clear();
        this_thread::sleep_for(chrono::seconds(1));
        ConsoleUtils::clearConsole();
    }


    // Fallback to SQLite if reading from CSV fails
    if (!success) {
        // Decrypt the SQLite database and store it in memory
        vector<unsigned char> decryptedData;
        bool decryptionSuccess = decryptSQLiteDatabase(confPaths::sqliteEncryptedDatabasePath, decryptedData);
        if (!decryptionSuccess) {
            ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::SQLITE_DATABASE_DECRYPTION_ERROR);
            cin.clear();
            return false;
        }

        // Initialize an in-memory SQLite database from the decrypted data
        sqlite3* db = nullptr;
        if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
            ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::SQLITE_MEMORY_ALLOCATION_ERROR);
            cin.clear();
            return false;
        }

        // Use boost::scoped_array for memory management
        boost::scoped_array<unsigned char> sqliteBuffer(new unsigned char[decryptedData.size()]);
        copy(decryptedData.begin(), decryptedData.end(), sqliteBuffer.get());

        // Load the copied data into the in-memory SQLite database
        if (sqlite3_deserialize(db, "main", sqliteBuffer.get(), decryptedData.size(), decryptedData.size(),
                                SQLITE_DESERIALIZE_RESIZEABLE) != SQLITE_OK) {
                                ErrorHandler::handleErrorAndThrow(ErrorHandler::ErrorType::SQLITE_DECRYPTED_DATABASE_LOAD_ERROR);
                                cin.clear();
                                return false;
        }

        // Read from the in-memory SQLite database
        readFromSQLite(venuesForEmails, venuesForTemplates, db);
        sqlite3_close(db);
        success = true;
    }

    return success;
}

// Function to read venue data from an SQLite database
void VenueDatabaseReader::readFromSQLite(vector<VenueForEmails>& venuesForEmails, vector<VenueForTemplates>& venuesForTemplates, sqlite3* db) {
    Sqlite3Ptr dbPtr(nullptr, Sqlite3Deleter());

    if (db == nullptr) {
        sqlite3* tempDb;
        if (sqlite3_open(":memory:", &tempDb) != SQLITE_OK) {
            ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::DATABASE_OPEN_ERROR, "In-memory database");
            cin.clear();
            return;
        }
        dbPtr.reset(tempDb);
    }

    const char* query = "SELECT name, email, country, state, city, capacity, genre FROM \"venues\"";
    sqlite3_stmt* tempStmt;
    if (sqlite3_prepare_v2(db, query, -1, &tempStmt, nullptr) != SQLITE_OK) {
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::SQLITE_STATEMENT_ERROR);
        cin.clear();
        return;
    }
    Sqlite3StmtPtr stmtPtr(tempStmt, Sqlite3StmtDeleter());

    while (sqlite3_step(stmtPtr.get()) == SQLITE_ROW) {
        VenueForEmails venueForEmails;
        venueForEmails.name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_NAME_ROW_NUMBER)));
        venueForEmails.email = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_EMAIL_ROW_NUMBER)));
        venueForEmails.country = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_COUNTRY_ROW_NUMBER)));
        venueForEmails.state = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_STATE_ROW_NUMBER)));
        venueForEmails.city = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_CITY_ROW_NUMBER)));
        venueForEmails.capacity = sqlite3_column_int(stmtPtr.get(), SQLITE_CAPACITY_ROW_NUMBER);
        venueForEmails.genre = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_GENRE_ROW_NUMBER)));

        venuesForEmails.push_back(venueForEmails);

        VenueForTemplates venueForTemplates;
        venueForTemplates.name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_NAME_ROW_NUMBER)));
        venueForTemplates.email = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_EMAIL_ROW_NUMBER)));
        venueForTemplates.country = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_COUNTRY_ROW_NUMBER)));
        venueForTemplates.state = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_STATE_ROW_NUMBER)));
        venueForTemplates.city = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_CITY_ROW_NUMBER)));
        venueForTemplates.capacity = sqlite3_column_int(stmtPtr.get(), SQLITE_CAPACITY_ROW_NUMBER);
        venueForTemplates.genre = string(reinterpret_cast<const char*>(sqlite3_column_text(stmtPtr.get(), SQLITE_GENRE_ROW_NUMBER)));

        venuesForTemplates.push_back(venueForTemplates);
    }

    // Resources are automatically released when the unique_ptr's go out of scope.
}
