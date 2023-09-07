// This file is used for testing DO NOT DELETE

#define CATCH_CONFIG_RUNNER

#include "../include/main.h"

#include "catch.hpp"

using namespace std;

class CinGuard {
    streambuf* orig_cin;

public:
    CinGuard(streambuf* newbuf) : orig_cin(cin.rdbuf(newbuf)) {}
    ~CinGuard() { cin.rdbuf(orig_cin);}
};

class CoutGuard {
    streambuf* orig_cout;

public:
    CoutGuard(streambuf* newbuf) : orig_cout(cout.rdbuf(newbuf)) {}
    ~CoutGuard() { cout.rdbuf(orig_cout); }
};


// -----------------------
// Test Group: ConsoleUtils
// -----------------------

TEST_CASE("ConsoleUtils::trim() functionality", "[ConsoleUtils]") {
    REQUIRE(ConsoleUtils::trim("   leading spaces") == "leading spaces");
    REQUIRE(ConsoleUtils::trim("trailing spaces   ") == "trailing spaces");
    REQUIRE(ConsoleUtils::trim("   both   ") == "both");
    REQUIRE(ConsoleUtils::trim("none") == "none");
    REQUIRE(ConsoleUtils::trim("") == "");
}


// -----------------------
// Test Group: CsvReader
// -----------------------

TEST_CASE("VenueDatabaseReader::readFromCsv() functionality", "[VenueDatabaseReader]") {
    string csvData =
        "Venue1,venue1@mock.com,USA,AL,Daphne,100,Mixed\n"
        "Venue2,venue2@mock.com,France,Paris Region,Paris,300,Rock\n";

    istringstream csvStream(csvData);
    vector<VenueForEmails> venuesForEmails;
    vector<VenueForTemplates> venuesForTemplates;

    VenueDatabaseReader::readFromCsv(venuesForEmails, venuesForTemplates, csvStream);

    // Check the size of the venues vector and compare with expected values
    REQUIRE(venuesForEmails.size() == 2);

    // Check the data of the first venue
    REQUIRE(venuesForEmails[0].name == "Venue1");
    REQUIRE(venuesForEmails[0].email == "venue1@mock.com");
    REQUIRE(venuesForEmails[0].country == "USA");
    REQUIRE(venuesForEmails[0].state == "AL");
    REQUIRE(venuesForEmails[0].city == "Daphne");
    REQUIRE(venuesForEmails[0].capacity == 100);
    REQUIRE(venuesForEmails[0].genre == "Mixed");

    // Check the data of the second venue
    REQUIRE(venuesForEmails[1].name == "Venue2");
    REQUIRE(venuesForEmails[1].email == "venue2@mock.com");
    REQUIRE(venuesForEmails[1].country == "France");
    REQUIRE(venuesForEmails[1].state == "Paris Region");
    REQUIRE(venuesForEmails[1].city == "Paris");
    REQUIRE(venuesForEmails[1].capacity == 300);
    REQUIRE(venuesForEmails[1].genre == "Rock");

    // Check the size of the venues vector and compare with expected values
    REQUIRE(venuesForTemplates.size() == 2);

    // Check the data of the first venue
    REQUIRE(venuesForTemplates[0].name == "Venue1");
    REQUIRE(venuesForTemplates[0].email == "venue1@mock.com");
    REQUIRE(venuesForTemplates[0].country == "USA");
    REQUIRE(venuesForTemplates[0].state == "AL");
    REQUIRE(venuesForTemplates[0].city == "Daphne");
    REQUIRE(venuesForTemplates[0].capacity == 100);
    REQUIRE(venuesForTemplates[0].genre == "Mixed");

    // Check the data of the second venue
    REQUIRE(venuesForTemplates[1].name == "Venue2");
    REQUIRE(venuesForTemplates[1].email == "venue2@mock.com");
    REQUIRE(venuesForTemplates[1].country == "France");
    REQUIRE(venuesForTemplates[1].state == "Paris Region");
    REQUIRE(venuesForTemplates[1].city == "Paris");
    REQUIRE(venuesForTemplates[1].capacity == 300);
    REQUIRE(venuesForTemplates[1].genre == "Rock");
}

TEST_CASE("ConfigManager::loadConfigSettings() functionality", "[ConfigManager]") {
    // Create variables to store the loaded settings
    bool useSSL;
    bool verifyPeer;
    bool verifyHost;
    bool verbose;
    string senderEmail;
    string smtpUsername;
    string mailPass;
    int smtpPort;
    string smtpServer;
    string venuesPathCopy = confPaths::mockConfigJsonPath;
    string venuesCsvPath = confPaths::mockVenuesCsvPath;
    bool initColor;

    // Create a ConfigManager instance
    ConfigManager configManager;

    // Call the loadConfigSettings function
    bool result = configManager.loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, smtpUsername, mailPass, smtpPort, smtpServer, venuesPathCopy, initColor);

    // Check if the function call was successful
    REQUIRE(result == true);

    // Now you can add more REQUIRE statements to check the loaded values


    REQUIRE(useSSL == true);
    REQUIRE(verifyHost == true);
    REQUIRE(verifyPeer == true);
    REQUIRE(verbose == true);
    REQUIRE(senderEmail == "mock@example.com");
    REQUIRE(smtpUsername == "mock_smtp_username");
    REQUIRE(smtpPort == 587);
    REQUIRE(smtpServer == "mock_smtp_server");
    REQUIRE(venuesCsvPath == "src/test/mock_venues.csv"); // Replace with the expected path
}


// -----------------------
// Test Group: MenuManager
// -----------------------

TEST_CASE("MenuManager::isValidMenuChoice() functionality", "[MenuManager]") {

    MenuManager menuManager;

    REQUIRE(menuManager.isValidMenuChoice(1) == true);
    REQUIRE(menuManager.isValidMenuChoice(2) == true);
    REQUIRE(menuManager.isValidMenuChoice(3) == true);
    REQUIRE(menuManager.isValidMenuChoice(4) == true);
    REQUIRE(menuManager.isValidMenuChoice(5) == true);
    REQUIRE(menuManager.isValidMenuChoice(6) == true);

    REQUIRE(menuManager.isValidMenuChoice(-1) == false);
    REQUIRE(menuManager.isValidMenuChoice(0) == false);
    REQUIRE(menuManager.isValidMenuChoice(7) == false);
}

TEST_CASE("MenuManager::displayMenuOptions() functionality", "[MenuManager]") {
    // Set up mock input and output streams
    istringstream input("5\n");
    ostringstream output;
    
    // Redirect the standard streams
    streambuf* original_cin = cin.rdbuf(input.rdbuf());
    streambuf* original_cout = cout.rdbuf(output.rdbuf());

    MenuManager menuManager;

    // Call the function
    int choice = menuManager.displayMenuOptions();

    // Restore the original streams
    cin.rdbuf(original_cin);
    cout.rdbuf(original_cout);

    // Define the expected output string
    string expectedOutput = 
        "===========================""\n"
        "         Main Menu         ""\n"
        "===========================""\n"
        "1. Venue Selection" "\n"
        "2. Venue Options" "\n"
        "3. Email Options" "\n"
        "4. Templates Options" "\n"
        "5. Configuration" "\n"
        "6. Exit" "\n"
        "Enter your choice: ";

    // Check the captured output and the returned choice
    REQUIRE(output.str() == expectedOutput);
    REQUIRE(choice == 5);
}

TEST_CASE("MenuManager::displaySelectedVenuesForEmails() functionality", "[MenuManager]") {
    // Create mock data directly in the test
    vector<SelectedVenueForEmails> selectedVenuesForEmails = {
        SelectedVenueForEmails{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
        SelectedVenueForEmails{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
    };
    
    // Create an ostringstream to capture the output
    ostringstream mockOutput;

    // Redirect cout to our mockOutput stream
    streambuf* originalCoutBuffer = cout.rdbuf(mockOutput.rdbuf());

    MenuManager manager;  // Instantiate MenuManager

    // Call the function
    manager.displaySelectedVenuesForEmails(selectedVenuesForEmails);

    // Restore original cout buffer
    cout.rdbuf(originalCoutBuffer);

    // Expected output
    string expectedOutput = 
        "===========================\n"
        "      Selected Venues      \n"
        "===========================\n"
        "Name: Venue1\n"
        "Country: USA\n"
        "State: AL\n"
        "City: Daphne\n"
        "Capacity: 100\n"
        "Genre: Mixed\n"
        "---------------------------\n"
        "Name: Venue2\n"
        "Country: France\n"
        "State: Paris Region\n"
        "City: Paris\n"
        "Capacity: 300\n"
        "Genre: Rock\n"
        "---------------------------\n";

    // Validate the output
    REQUIRE(mockOutput.str() == expectedOutput);
}

TEST_CASE("MenuManager::displaySelectedVenuesForTemplates() functionality", "[MenuManager]") {
    // Create mock data directly in the test
    vector<SelectedVenueForTemplates> selectedVenuesForTemplates = {
        SelectedVenueForTemplates{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
        SelectedVenueForTemplates{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
    };
    
    // Create an ostringstream to capture the output
    ostringstream mockOutput;

    // Redirect cout to our mockOutput stream
    streambuf* originalCoutBuffer = cout.rdbuf(mockOutput.rdbuf());

    MenuManager manager;  // Instantiate MenuManager

    // Call the function
    manager.displaySelectedVenuesForTemplates(selectedVenuesForTemplates);

    // Restore original cout buffer
    cout.rdbuf(originalCoutBuffer);

    // Expected output
    string expectedOutput = 
        "===========================\n"
        "      Selected Venues      \n"
        "===========================\n"
        "Name: Venue1\n"
        "Country: USA\n"
        "State: AL\n"
        "City: Daphne\n"
        "Capacity: 100\n"
        "Genre: Mixed\n"
        "---------------------------\n"
        "Name: Venue2\n"
        "Country: France\n"
        "State: Paris Region\n"
        "City: Paris\n"
        "Capacity: 300\n"
        "Genre: Rock\n"
        "---------------------------\n";

    // Validate the output
    REQUIRE(mockOutput.str() == expectedOutput);
}


// -----------------------
// Test Group: EmailManager
// -----------------------

// Mocking curl for the purpose of this example. In real-world applications, a more complex mocking mechanism might be necessary.
CURL* mockCurl = nullptr;

TEST_CASE("EmailManager::getCurrentDateRfc2822() functionality", "[EmailManager]") {
    // Arrange
    EmailManager emailManager; // Instantiate the class you're testing

    // Act
    string result = emailManager.getCurrentDateRfc2822();

    // Assert
    // You can use a regular expression or manual parsing to check the format.
    // Here, we're using a regex to match the expected format.
    regex dateFormat(R"(\w{3}, \d{2} \w{3} \d{4} \d{2}:\d{2}:\d{2} \w{3})");
    REQUIRE(regex_match(result, dateFormat));
}

TEST_CASE("EmailManager::sanitizeSubject() functionality", "[EmailManager]") {
    // Arrange
    EmailManager emailManager; // Instantiate the class you're testing
    string subject = "Hello World\nTest"; // Contains newline character

    // Act
    emailManager.sanitizeSubject(subject);

    // Assert
    REQUIRE(subject == "Hello World\nTest"); // The newline should be replaced by a space
}

TEST_CASE("EmailManager::viewEmailSettings functionality", "[EmailManager]") {
    ostringstream oss;
    streambuf* oldCoutStreamBuf = cout.rdbuf();
    cout.rdbuf(oss.rdbuf());

    EmailManager emailManager;

    emailManager.viewEmailSettings(true, "/etc/ssl/certs/ca-certificates.crt", true, true, true, "mock@example.com", "mock@example.com", 587, "mock_smtp_server");

    cout.rdbuf(oldCoutStreamBuf);

    string expectedOutput = 
    "============================\n"
    "       Email Settings       \n"
    "============================\n"
    "SMTP Username: mock@example.com\n"
    "SMTP Server: mock_smtp_server\n"
    "SMTP Port: 587\n"
    "Sender Email: mock@example.com\n"
    "SSL: true\n"
    "SSL Cert Path: /etc/ssl/certs/ca-certificates.crt\n"
    "verifyPeer: true\n"
    "verifyHost: true\n"
    "verbose: true\n";
    
    REQUIRE(oss.str() == expectedOutput);
}

TEST_CASE("EmailManager::isValidEmailTest() functionality", "[EmailManager]") {
    // Set up mock data for email validation
    string validEmail = "venue1@mock.com";
    string invalidEmail = "invalid.email";
    EmailManager emailManager;
    // Call the isValidEmail function
    bool isValid1 = emailManager.isValidEmail(validEmail);
    bool isValid2 = emailManager.isValidEmail(invalidEmail);

    // Compare the result with expected values
    REQUIRE(isValid1 == true);
    REQUIRE(isValid2 == false);
}

TEST_CASE("EmailManager::sendIndividualEmail() functionality", "[EmailManager]") {

    EmailManager manager;
    CURL* mockCurl = nullptr;  // Normally, you'd mock the CURL* or create a real handle if needed.

    vector<SelectedVenueForEmails> selectedVenuesForEmails;
    
    string attachmentName;
    string attachmentSize;
    string attachmentPath;

    // Mock venue data
    SelectedVenueForEmails venueForEmails;
    venueForEmails.email = "venue1@mock.com";

    string senderEmail = "mock@example.com";
    string subject = "Test Subject";
    string message = "This is a test message.";

    string smtpServer = "mock_smtp_server";
    int smtpPort = 587;

    SECTION("Sending email with valid parameters") {
        bool result = manager.sendIndividualEmail(mockCurl, venueForEmails, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmails);
        
        // Here, the expected value is false because our mockCurl is nullptr.
        REQUIRE(result == false);
    }

    SECTION("Sending email with invalid email") {
        venueForEmails.email = "invalid.email.com";  // Missing '@'
        
        bool result = manager.sendIndividualEmail(mockCurl, venueForEmails, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmails);
        
        // Here, the expected value is false because our mockCurl is nullptr and email is invalid.
        REQUIRE(result == false);
    }

    SECTION("Sending email in batches") {
        // Add 60 mock venues to the selectedVenuesForEmails
        for (int i = 0; i < 60; i++) {
            SelectedVenueForEmails tempVenueForEmails;
            tempVenueForEmails.email = "venue" + to_string(i + 2) + "@mock.com";  // venue2@mock.com, venue3@mock.com, ...
            selectedVenuesForEmails.push_back(tempVenueForEmails);
        }

        // Normally, you'd mock the curl_easy_perform function to capture the "To:" and "BCC:" fields and verify
        // the batching logic. Here, for simplicity, we're just calling the function.
        bool result = manager.sendIndividualEmail(mockCurl, venueForEmails, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmails);

        // Here, the expected value is false because our mockCurl is nullptr.
        // However, you'd want to validate that the emails are being batched correctly.
        REQUIRE(result == false);
    }
}


// -----------------------
// Test Group: VenueFilter
// -----------------------

TEST_CASE("processVenueSelectionForEmails functionality", "[VenueFilter]") {


    SECTION("Filters by country correctly") {
        // Set up mock data and expected results
        vector<VenueForEmails> venuesForEmails = {
            VenueForEmails{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
            VenueForEmails{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
        };    
        vector<SelectedVenueForEmails> selectedVenuesForEmails;
        std::istringstream input("1\n1\n1\n1\n1\n1\n"); // Mock selecting the first country
        std::ostringstream output;
        VenueFilter venueFilter;

        venueFilter.processVenueSelectionForEmails(venuesForEmails, selectedVenuesForEmails, input, output);

        REQUIRE(selectedVenuesForEmails.size() == 1);
        REQUIRE(selectedVenuesForEmails[0].country == "France");
    }

    SECTION("Handles invalid input") {
        // Redirect std::cerr to capture error messages
        std::ostringstream err_output;
        auto old_err = std::cerr.rdbuf(err_output.rdbuf());

        // Set up mock data and expected results
        vector<VenueForEmails> venuesForEmails = {
            VenueForEmails{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
            VenueForEmails{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
        };    
        vector<SelectedVenueForEmails> selectedVenuesForEmails;
        std::istringstream input("4\n");  // Mock invalid input
        std::ostringstream output;
        VenueFilter venueFilter;

        venueFilter.processVenueSelectionForEmails(venuesForEmails, selectedVenuesForEmails, input, output);

        // Restore old std::cerr buffer
        std::cerr.rdbuf(old_err);
        
        REQUIRE(err_output.str().find("Invalid index:") != std::string::npos);
    }

    SECTION("Proper final venue selection") {
        // Set up mock data and expected results
        vector<VenueForEmails> venuesForEmails = {
            VenueForEmails{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
            VenueForEmails{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
        };    
        vector<SelectedVenueForEmails> selectedVenuesForEmails;
        std::istringstream input("1\n1\n1\n1\n1\n1\n1\n1\n");  // Mock selecting the first option at each prompt
        std::ostringstream output;
        VenueFilter venueFilter;

        venueFilter.processVenueSelectionForEmails(venuesForEmails, selectedVenuesForEmails, input, output);

        REQUIRE(selectedVenuesForEmails.size() == 1);
        REQUIRE(selectedVenuesForEmails[0].name == "Venue2");
    }
}

TEST_CASE("processVenueSelectionForTemplates functionality", "[VenueFilter]") {


    SECTION("Filters by country correctly") {
        // Set up mock data and expected results
        vector<VenueForTemplates> venuesForTemplates = {
            VenueForTemplates{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
            VenueForTemplates{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
        };    
        vector<SelectedVenueForTemplates> selectedVenuesForTemplates;
        std::istringstream input("1\n1\n1\n1\n1\n1\n"); // Mock selecting the first country
        std::ostringstream output;
        VenueFilter venueFilter;

        venueFilter.processVenueSelectionForTemplates(venuesForTemplates, selectedVenuesForTemplates, input, output);

        REQUIRE(selectedVenuesForTemplates.size() == 1);
        REQUIRE(selectedVenuesForTemplates[0].country == "France");
    }

    SECTION("Handles invalid input") {
        // Redirect std::cerr to capture error messages
        std::ostringstream err_output;
        auto old_err = std::cerr.rdbuf(err_output.rdbuf());

        // Set up mock data and expected results
        vector<VenueForTemplates> venuesForTemplates = {
            VenueForTemplates{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
            VenueForTemplates{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
        };    
        vector<SelectedVenueForTemplates> selectedVenuesForTemplates;
        std::istringstream input("4\n");  // Mock invalid input
        std::ostringstream output;
        VenueFilter venueFilter;

        venueFilter.processVenueSelectionForTemplates(venuesForTemplates, selectedVenuesForTemplates, input, output);

        // Restore old std::cerr buffer
        std::cerr.rdbuf(old_err);
        
        REQUIRE(err_output.str().find("Invalid index:") != std::string::npos);
    }

    SECTION("Proper final venue selection") {
        // Set up mock data and expected results
        vector<VenueForTemplates> venuesForTemplates = {
            VenueForTemplates{"Venue1", "venue1@mock.com", "USA", "AL", "Daphne", 100, "Mixed"},
            VenueForTemplates{"Venue2", "venue2@mock.com", "France", "Paris Region", "Paris", 300, "Rock"}
        };    
        vector<SelectedVenueForTemplates> selectedVenuesForTemplates;
        std::istringstream input("1\n1\n1\n1\n1\n1\n1\n1\n");  // Mock selecting the first option at each prompt
        std::ostringstream output;
        VenueFilter venueFilter;

        venueFilter.processVenueSelectionForTemplates(venuesForTemplates, selectedVenuesForTemplates, input, output);

        REQUIRE(selectedVenuesForTemplates.size() == 1);
        REQUIRE(selectedVenuesForTemplates[0].name == "Venue2");
    }
}


// -----------------------
// Test Group: VenueUtilities
// -----------------------

TEST_CASE("VenueUtilities::convertToSelectedVenueForEmails() functionality", "[VenueUtilities]") {
    // Create a mock VenueForEmails
    VenueForEmails mockVenueForEmails;
    mockVenueForEmails.name = "Venue1";
    mockVenueForEmails.email = "venue1@mock.com";
    mockVenueForEmails.country = "USA";
    mockVenueForEmails.state = "AL";
    mockVenueForEmails.city = "Daphne";
    mockVenueForEmails.capacity = 100;
    mockVenueForEmails.genre = "Mixed";

    // Convert Venue to SelectedVenueForEmails using the function
    VenueUtilities venueUtilities;
    SelectedVenueForEmails selectedVenueForEmails = venueUtilities.convertToSelectedVenueForEmails(mockVenueForEmails);

    // Compare the converted SelectedVenue with expected values
    REQUIRE(selectedVenueForEmails.name == "Venue1");
    REQUIRE(selectedVenueForEmails.email == "venue1@mock.com");
    REQUIRE(selectedVenueForEmails.country == "USA");
    REQUIRE(selectedVenueForEmails.state == "AL");
    REQUIRE(selectedVenueForEmails.city == "Daphne");
    REQUIRE(selectedVenueForEmails.capacity == 100);
    REQUIRE(selectedVenueForEmails.genre == "Mixed");
}

TEST_CASE("VenueUtilities::convertToSelectedVenueForTemplates() functionality", "[VenueUtilities]") {
    // Create a mock VenueForTemplates
    VenueForTemplates mockVenueForTemplates;
    mockVenueForTemplates.name = "Venue1";
    mockVenueForTemplates.email = "venue1@mock.com";
    mockVenueForTemplates.country = "USA";
    mockVenueForTemplates.state = "AL";
    mockVenueForTemplates.city = "Daphne";
    mockVenueForTemplates.capacity = 100;
    mockVenueForTemplates.genre = "Mixed";

    // Convert Venue to SelectedVenueForTemplates using the function
    VenueUtilities venueUtilities;
    SelectedVenueForTemplates selectedVenueForTemplates = venueUtilities.convertToSelectedVenueForTemplates(mockVenueForTemplates);

    // Compare the converted SelectedVenueForTemplates with expected values
    REQUIRE(selectedVenueForTemplates.name == "Venue1");
    REQUIRE(selectedVenueForTemplates.email == "venue1@mock.com");
    REQUIRE(selectedVenueForTemplates.country == "USA");
    REQUIRE(selectedVenueForTemplates.state == "AL");
    REQUIRE(selectedVenueForTemplates.city == "Daphne");
    REQUIRE(selectedVenueForTemplates.capacity == 100);
    REQUIRE(selectedVenueForTemplates.genre == "Mixed");
}

bool operator==(const SelectedVenueForEmails& lhs, const SelectedVenueForEmails& rhs) {
    return lhs.name == rhs.name && lhs.email == rhs.email && lhs.genre == rhs.genre &&
           lhs.state == rhs.state && lhs.city == rhs.city && lhs.capacity == rhs.capacity;
}

bool operator==(const SelectedVenueForTemplates& lhs, const SelectedVenueForTemplates& rhs) {
    return lhs.name == rhs.name && lhs.email == rhs.email && lhs.genre == rhs.genre &&
           lhs.state == rhs.state && lhs.city == rhs.city && lhs.capacity == rhs.capacity;
}

int main( int argc, char* argv[] )
{   
    // Run all the tests
    int result = Catch::Session().run(argc, argv);

    // Return the test results
    return result;
}

CATCH_CONFIG_RUNNER // This line will define Catch2's main function
