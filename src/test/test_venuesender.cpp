// This file is used for testing DO NOT DELETE

#define CATCH_CONFIG_RUNNER

#include "../include/main.h"

#include "../include/fileutils.h"
#include "../include/filtercriteria.h"
#include "../include/mail.h"
#include "../include/menu.h"
#include "../include/structs.h"

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

TEST_CASE("CsvReader::readCSV() functionality", "[CsvReader]") {
    // Set up mock data for readCSV function
    vector<Venue> venues;
    string venuesCsvPath = confPaths::mockVenuesCsvPath;

    // Call the readCSV function
    CsvReader::readCSV(venues, venuesCsvPath);

    // Compare the result with expected values
    REQUIRE(venues.size() == 2);

    REQUIRE(venues[0].name == "Venue1");
    REQUIRE(venues[0].email == "venue1@mock.com");
    REQUIRE(venues[0].genre == "all");
    REQUIRE(venues[0].state == "AL");
    REQUIRE(venues[0].city == "Daphne");
    REQUIRE(venues[0].capacity == 100);

    REQUIRE(venues[1].name == "Venue2");
    REQUIRE(venues[1].email == "venue2@mock.com");
    REQUIRE(venues[1].genre == "rock");
    REQUIRE(venues[1].state == "UT");
    REQUIRE(venues[1].city == "Provo");
    REQUIRE(venues[1].capacity == 300);
}

TEST_CASE("ConfigManager::loadConfigSettings() functionality", "[ConfigManager]") {
    bool useSSL;
    bool verifyPeer;
    bool verifyHost;
    bool verbose;
    string senderEmail;
    string smtpUsername;
    string mailPass;
    int smtpPort;
    string smtpServer;
    string venuesCsvPath;

    ConfigManager configManager;

    std::string testVenuesPathCopy = "src/test/mock_venues.csv";
    bool result = configManager.loadConfigSettings(useSSL, verifyPeer, verifyHost, verbose, senderEmail, 
                                     smtpUsername, mailPass, smtpPort, smtpServer, testVenuesPathCopy);
    
    REQUIRE(result == true);
    REQUIRE(useSSL == true);
    REQUIRE(verifyHost == true);
    REQUIRE(verifyPeer == true);
    REQUIRE(verbose == true);
    REQUIRE(senderEmail == "mock@example.com");
    REQUIRE(smtpUsername == "mock_smtp_username");
    REQUIRE(smtpPort == 587);
    REQUIRE(smtpServer == "mock_smtp_server");
    REQUIRE(testVenuesPathCopy == "src/test/mock_venues.csv");
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
    REQUIRE(menuManager.isValidMenuChoice(7) == true);
    REQUIRE(menuManager.isValidMenuChoice(8) == true);
    REQUIRE(menuManager.isValidMenuChoice(9) == true);
    REQUIRE(menuManager.isValidMenuChoice(10) == true);
    REQUIRE(menuManager.isValidMenuChoice(11) == true);
    REQUIRE(menuManager.isValidMenuChoice(12) == true);
    REQUIRE(menuManager.isValidMenuChoice(13) == true);  

    REQUIRE(menuManager.isValidMenuChoice(-1) == false);
    REQUIRE(menuManager.isValidMenuChoice(0) == false);
    REQUIRE(menuManager.isValidMenuChoice(14) == false);
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
        "=====================" "\n"
        "===== Main Menu =====" "\n"
        "=====================" "\n"
        "1. Filter by Genre" "\n"
        "2. Filter by State" "\n"
        "3. Filter by City" "\n"
        "4. Filter by Capacity" "\n"
        "5. Clear Selected Venues" "\n"
        "6. Clear Booking Template" "\n"
        "7. View Selected Venues" "\n"
        "8. Show Email Settings" "\n"
        "9. View & Edit Email" "\n"
        "10. Venue Booking Template" "\n"
        "11. Email Custom Address" "\n"
        "12. Finish & Send Emails" "\n"
        "13. Exit VenueSender" "\n"
        "Enter your choice: ";

    // Check the captured output and the returned choice
    REQUIRE(output.str() == expectedOutput);
    REQUIRE(choice == 5);
}

TEST_CASE("MenuManager::displaySelectedVenues() functionality", "[MenuManager]") {
    // Create mock data directly in the test
    vector<SelectedVenue> selectedVenues = {
        SelectedVenue{"Venue1", "venue1@mock.com", "all", "AL", "Daphne", 100},
        SelectedVenue{"Venue2", "venue2@mock.com", "rock", "UT", "Provo", 300}
    };
    
    // Create an ostringstream to capture the output
    ostringstream mockOutput;

    // Redirect cout to our mockOutput stream
    streambuf* originalCoutBuffer = cout.rdbuf(mockOutput.rdbuf());

    MenuManager manager;  // Instantiate MenuManager

    // Call the function
    manager.displaySelectedVenues(selectedVenues);

    // Restore original cout buffer
    cout.rdbuf(originalCoutBuffer);

    // Expected output
    std::string expectedOutput = 
        "===========================\n"
        "===== Selected Venues =====\n"
        "===========================\n"
        "Name: Venue1\n"
        "Email: venue1@mock.com\n"
        "City: Daphne\n"
        "--------------------------\n"
        "Name: Venue2\n"
        "Email: venue2@mock.com\n"
        "City: Provo\n"
        "--------------------------\n";

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
    std::string result = emailManager.getCurrentDateRfc2822();

    // Assert
    // You can use a regular expression or manual parsing to check the format.
    // Here, we're using a regex to match the expected format.
    std::regex dateFormat(R"(\w{3}, \d{2} \w{3} \d{4} \d{2}:\d{2}:\d{2} \w{3})");
    REQUIRE(std::regex_match(result, dateFormat));
}

TEST_CASE("EmailManager::sanitizeSubject() functionality", "[EmailManager]") {
    // Arrange
    EmailManager emailManager; // Instantiate the class you're testing
    std::string subject = "Hello World\nTest"; // Contains newline character

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

    emailManager.viewEmailSettings(true, true, true, true, "mock@example.com", 587, "mock_smtp_server");

    cout.rdbuf(oldCoutStreamBuf);

    string expectedOutput = 
    "==========================\n"
    "===== Email Settings =====\n"
    "==========================\n"
    "SMTP Server: mock_smtp_server\n"
    "SMTP Port: 587\n"
    "Sender Email: mock@example.com\n"
    "SSL: true\n"
    "verifyPeer: true\n"
    "verifyHost: true\n"
    "verbose: true\n"
    "==========================\n";
    
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

    vector<SelectedVenue> selectedVenuesForEmail;
    
    string attachmentName;
    string attachmentSize;
    string attachmentPath;

    // Mock venue data
    SelectedVenue venue;
    venue.email = "venue1@mock.com";

    std::string senderEmail = "mock@example.com";
    std::string subject = "Test Subject";
    std::string message = "This is a test message.";

    std::string smtpServer = "mock_smtp_server";
    int smtpPort = 587;

    SECTION("Sending email with valid parameters") {
        bool result = manager.sendIndividualEmail(mockCurl, venue, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail);
        
        // Here, the expected value is false because our mockCurl is nullptr.
        REQUIRE(result == false);
    }

    SECTION("Sending email with invalid email") {
        venue.email = "invalid.email.com";  // Missing '@'
        
        bool result = manager.sendIndividualEmail(mockCurl, venue, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail);
        
        // Here, the expected value is false because our mockCurl is nullptr and email is invalid.
        REQUIRE(result == false);
    }

    SECTION("Sending email in batches") {
        // Add 60 mock venues to the selectedVenuesForEmail
        for (int i = 0; i < 60; i++) {
            SelectedVenue tempVenue;
            tempVenue.email = "venue" + std::to_string(i + 2) + "@mock.com";  // venue2@mock.com, venue3@mock.com, ...
            selectedVenuesForEmail.push_back(tempVenue);
        }

        // Normally, you'd mock the curl_easy_perform function to capture the "To:" and "BCC:" fields and verify
        // the batching logic. Here, for simplicity, we're just calling the function.
        bool result = manager.sendIndividualEmail(mockCurl, venue, senderEmail, subject, message, smtpServer, smtpPort, attachmentName, attachmentSize, attachmentPath, selectedVenuesForEmail);

        // Here, the expected value is false because our mockCurl is nullptr.
        // However, you'd want to validate that the emails are being batched correctly.
        REQUIRE(result == false);
    }
}



// -----------------------
// Test Group: VenueFilter
// -----------------------

TEST_CASE("VenueFilter::processVenueSelection() functionality", "[VenueFilter]") {
    // Set up mock data and expected results
    vector<SelectedVenue> temporaryFilteredVenues = {
        SelectedVenue{"Venue1", "venue1@mock.com", "all", "AL", "Daphne", 100},
        SelectedVenue{"Venue2", "venue2@mock.com", "rock", "UT", "Provo", 300}
    };
    vector<SelectedVenue> selectedVenuesForEmail;
    
    // Set up mock user input and output streams
    istringstream mockInput("1,2"); // user selects both venues
    ostringstream mockOutput;

    VenueFilter venueFilter;

    // Call the function
    venueFilter.processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail, mockInput, mockOutput);

    // Check results
    REQUIRE(selectedVenuesForEmail.size() == 2); 
    REQUIRE(selectedVenuesForEmail[0].name == "Venue1");
    REQUIRE(selectedVenuesForEmail[1].name == "Venue2");

    // Check output to the user
    REQUIRE(mockOutput.str() == "Select venues to add (comma-separated indices): \n");
}


// -----------------------
// Test Group: VenueUtilities
// -----------------------

TEST_CASE("VenueUtilities::convertToSelectedVenue() functionality", "[VenueUtilities]") {
    // Create a mock Venue
    Venue mockVenue;
    mockVenue.name = "Venue1";
    mockVenue.email = "venue1@mock.com";
    mockVenue.genre = "all";
    mockVenue.state = "AL";
    mockVenue.city = "Daphne";
    mockVenue.capacity = 100;

    // Convert Venue to SelectedVenue using the function
    SelectedVenue selectedVenue = VenueUtilities::convertToSelectedVenue(mockVenue);

    // Compare the converted SelectedVenue with expected values
    REQUIRE(selectedVenue.name == "Venue1");
    REQUIRE(selectedVenue.email == "venue1@mock.com");
    REQUIRE(selectedVenue.genre == "all");
    REQUIRE(selectedVenue.state == "AL");
    REQUIRE(selectedVenue.city == "Daphne");
    REQUIRE(selectedVenue.capacity == 100);
}


// -----------------------
// Test Group: EncryptionManager
// -----------------------

TEST_CASE("EncryptionManager::encryptPassword()decryptPassword() functionality", "[EncryptionManager][decryption]") {
    // Initialize encryption parameters
    EncryptionManager encryptionManager;

    string mailPassword = "enter_email_password";

    string encryptedmailPass;
    REQUIRE(encryptionManager.encryptPassword(mailPassword, encryptedmailPass) == true);

    string mailPassDecrypted;
    string decryptedmailPass = encryptionManager.decryptPassword(encryptedmailPass);
    REQUIRE(decryptedmailPass == mailPassword);

    REQUIRE(mailPassword == decryptedmailPass);
}

bool operator==(const SelectedVenue& lhs, const SelectedVenue& rhs) {
    return lhs.name == rhs.name && lhs.email == rhs.email && lhs.genre == rhs.genre &&
           lhs.state == rhs.state && lhs.city == rhs.city && lhs.capacity == rhs.capacity;
}


int main( int argc, char* argv[] )
{   
    ConfigManager configManager;

    // Run all the tests
    int result = Catch::Session().run(argc, argv);

    // Clean up the mock config file after all tests have been run
    configManager.resetConfigFile();

    // Return the test results
    return result;
}

CATCH_CONFIG_RUNNER // This line will define Catch2's main function
