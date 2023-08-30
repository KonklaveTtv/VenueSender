#include "include/fileutils.h"

#include "errorhandler.h" // Include here due to circular dependency between fileutils.h and errorhandler.h

// Use the standard namespace
using namespace std;

// Namespace to hold configuration file paths
namespace confPaths {
string venuesCsvPath = "venues.csv";
string configJsonPath = "config.json";
string mockVenuesCsvPath = "src/test/mock_venues.csv";
string mockConfigJsonPath = "src/test/mock_config.json";
}

// Function to trim leading and trailing spaces from a string
string ConsoleUtils::trim(const string& str){
    // Trimming function
    const auto notSpace = [](int ch) {return !isspace(ch); };
    auto first = find_if(str.begin(), str.end(), notSpace);
    auto last = find_if(str.rbegin(), str.rend(), notSpace).base();
    return (first < last ? string(first, last) : string());
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

std::string ConsoleUtils::passwordEntry() {
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
#ifndef UNIT_TESTING
        ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
        cout << "============================================" << endl;
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
        cout << "Enter your email password: ";

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

            // Add the character to the password
            password += ch;

            // Echo an asterisk
            std::putchar('*');
        }

        std::cout << std::endl << "Confirm your email password: ";
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

            // Add the character to the password
            confirm += ch;

            // Echo an asterisk
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
                                       string& venuesCsvPath) {


#ifdef UNIT_TESTING
    // Load plain text passwords from mock_config.json
    mailPass = "mock_email_password";
#else
    // Use the password entered by the user
    mailPass = ConsoleUtils::passwordEntry();
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
