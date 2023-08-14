#define CATCH_CONFIG_RUNNER

#include "main.h"

#include "fileutils.h"
#include "filtercriteria.h"
#include "mail.h"

#include "catch.hpp"

#include <cstdlib>
#include <ctime>

using namespace confPaths;


// -----------------------
// Test Group: Utilities
// -----------------------

TEST_CASE("Test trim function", "[FileUtils]") {
    REQUIRE(trim("   leading spaces") == "leading spaces");
    REQUIRE(trim("trailing spaces   ") == "trailing spaces");
    REQUIRE(trim("   both   ") == "both");
    REQUIRE(trim("none") == "none");
    REQUIRE(trim("") == "");
}


// -----------------------
// Test Group: File Operations
// -----------------------

TEST_CASE("Test Read CSV", "[csv]") {
    // Set up mock data for readCSV function
    std::vector<Venue> venues;
    std::string venuesCsvPath = confPaths::mockVenuesCsvPath;

    // Call the readCSV function
    readCSV(venues, venuesCsvPath);

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

TEST_CASE("LoadConfigSettingsTest", "[fileutils]") {
    std::string smtpServer;
    int smtpPort;
    std::string smtpUsername;
    std::string smtpPass;
    std::string venuesCsvPath;
    std::string mailPass;
    std::string senderEmail;
    int senderSmtpPort;
    
    bool result = loadConfigSettings(smtpServer, smtpPort, smtpUsername, smtpPass, 
                                     venuesCsvPath, mailPass, senderEmail, senderSmtpPort);
    
    REQUIRE(result == true);
    REQUIRE(smtpServer == "mock_smtp_server");
    REQUIRE(smtpPort == 587);
    REQUIRE(smtpUsername == "mock_smtp_username");
    REQUIRE(venuesCsvPath == confPaths::mockVenuesCsvPath);
    REQUIRE(senderEmail == "mock@example.com");
    REQUIRE(senderSmtpPort == 587);
}

// -----------------------
// Test Group: Menu Operations
// -----------------------

TEST_CASE("viewEmailSettings function", "[Display]") {
    std::ostringstream oss;
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    // Call the function
    viewEmailSettings("testServer", 123, "mock@example.com", 456, "smtpPass", "mailPass");

    std::cout.rdbuf(oldCoutStreamBuf);
    
    REQUIRE(oss.str() == "===== Email Settings =====\nSMTP Server: testServer\nSMTP Port: 123\nSender Email: mock@example.com\nSender SMTP Port: 456\nSMTP Password: smtpPass\nMail Password: mailPass\n===========================\n");
}

TEST_CASE("viewEmailSettings funcion", "[Display]") {
    std::ostringstream oss;
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    // Call the function
    viewEmailSettings("testServer", 123, "mock@example.com", 456, "smtpPass", "mailPass");

    std::cout.rdbuf(oldCoutStreamBuf);

    REQUIRE(oss.str() == "===== Email Settings =====\nSMTP Server: testServer\nSMTP Port: 123\nSender Email: mock@example.com\nSender SMTP Port: 456\nSMTP Password: smtpPass\nMail Password: mailPass\n===========================\n");
}

// -----------------------
// Test Group: Email Operations
// -----------------------

TEST_CASE("Test Email Validation", "[validation]") {
    // Set up mock data for email validation
    std::string validEmail = "venue1@mock.com";
    std::string invalidEmail = "invalid.email";

    // Call the isValidEmail function
    bool isValid1 = isValidEmail(validEmail);
    bool isValid2 = isValidEmail(invalidEmail);

    // Compare the result with expected values
    REQUIRE(isValid1 == true);
    REQUIRE(isValid2 == false);
}

TEST_CASE("Test View Email Sending Progress", "[email]") {
    // Set up mock data for viewEmailSendingProgress function
    CurlHandleWrapper curlWrapper;
    CURL* curl = curlWrapper.get();
    std::vector<SelectedVenue> selectedVenuesForEmail;
    std::string senderEmail = "mock@example.com";
    std::string subject = "Mock Subject";
    std::string message = "Mock Message";
    std::string smtpServer = "mock_smtp_server";
    int smtpPort = 587;
    std::string smtpUsername = "mock_smtp_username";
    std::string smtpPass = "mock_smtp_password";

    // Simulate adding some selected venues
    Venue testVenue1("Venue1", "venue1@mock.com", "all", "AL", "Daphne", 100);
    Venue testVenue2("Venue2", "venue2@mock.com", "rock", "UT", "Provo", 300);

    // Convert Venue objects to SelectedVenue objects
    SelectedVenue selectedVenue1(testVenue1);
    SelectedVenue selectedVenue2(testVenue2);

    // Populate the selectedVenuesForEmail vector
    selectedVenuesForEmail.push_back(selectedVenue1);
    selectedVenuesForEmail.push_back(selectedVenue2);

    // Redirect cout to capture console output
    std::stringstream outputCapture;
    std::streambuf* coutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(outputCapture.rdbuf());

    // Call the viewEmailSendingProgress function
    viewEmailSendingProgress(curl, selectedVenuesForEmail, senderEmail, subject, message, smtpServer, smtpPort, smtpUsername, smtpPass);

    // Restore cout
    std::cout.rdbuf(coutBuffer);

    // Compare the result with expected values
    std::string output = outputCapture.str();
    REQUIRE(output.find("Sending email 1 of 2 to: venue1@mock.com") != std::string::npos);
    REQUIRE(output.find("Sending email 2 of 2 to: venue2@mock.com") != std::string::npos);
    REQUIRE(output.find("Email sending progress completed.") != std::string::npos);
}


// -----------------------
// Test Group: Venue Operations
// -----------------------

TEST_CASE("Test Convert Venue to SelectedVenue", "[convertToSelectedVenue]") {
    // Create a mock Venue
    Venue mockVenue;
    mockVenue.name = "Venue1";
    mockVenue.email = "venue1@mock.com";
    mockVenue.city = "Daphne";
    mockVenue.genre = "all";
    mockVenue.state = "AL";
    mockVenue.capacity = 100;

    // Convert Venue to SelectedVenue using the function
    SelectedVenue selectedVenue = convertToSelectedVenue(mockVenue);

    // Compare the converted SelectedVenue with expected values
    REQUIRE(selectedVenue.name == "Venue1");
    REQUIRE(selectedVenue.email == "venue1@mock.com");
    REQUIRE(selectedVenue.city == "Daphne");
    REQUIRE(selectedVenue.genre == "all");
    REQUIRE(selectedVenue.state == "AL");
    REQUIRE(selectedVenue.capacity == 100);
}

TEST_CASE("Test Process Venue Selection", "[processVenueSelection]") {
    // Create a mock vector of temporary filtered venues
    Venue testVenue1("Venue1", "venue1@mock.com", "Daphne", "all", "AL", 100);
    Venue testVenue2("Venue2", "venue2@mock.com", "Provo", "rock", "UT", 300);
    std::vector<Venue> temporaryFilteredVenues = {testVenue1, testVenue2};

    // Convert Venue objects to SelectedVenue objects
    std::vector<SelectedVenue> selectedVenuesForEmail;
    for (const Venue& venue : temporaryFilteredVenues) {
        selectedVenuesForEmail.push_back(convertToSelectedVenue(venue));
    }

    // Simulate user input of selecting venues 1 and 2
    std::stringstream input;
    input << "1,2\n";
    std::cin.rdbuf(input.rdbuf());

    // Convert the selected venues using convertToSelectedVenue
    SelectedVenue selectedVenue1 = convertToSelectedVenue(testVenue1);
    SelectedVenue selectedVenue2 = convertToSelectedVenue(testVenue2);

    // Create a new vector to store the selected venues
    std::vector<SelectedVenue> selectedVenues;
    selectedVenues.push_back(selectedVenue1);
    selectedVenues.push_back(selectedVenue2);

    // Call the processVenueSelection function with the correct arguments
    processVenueSelection(selectedVenuesForEmail, selectedVenues);
}

// -----------------------
// Test Group: Encryption and Decryption
// -----------------------

TEST_CASE("Encrypt and decrypt SMTP password", "[encryption][decryption]") {
    // Initialize encryption parameters
    initializeEncryptionParams();

    std::string smtpPassword = "enter_smtp_password";

    std::string encryptedSmtpPass;
    REQUIRE(encryptPassword(smtpPassword, encryptedSmtpPass) == true);

    std::string smtpPassDecrypted;
    std::string decryptedSmtpPass = decryptPassword(encryptedSmtpPass);
    REQUIRE(decryptedSmtpPass == smtpPassword);

    REQUIRE(smtpPassword == decryptedSmtpPass);
}

TEST_CASE("Encrypt and decrypt email password", "[encryption][decryption]") {
    // Initialize encryption parameters
    initializeEncryptionParams();

    std::string mailPassword = "enter_email_password";

    std::string encryptedmailPass;
    REQUIRE(encryptPassword(mailPassword, encryptedmailPass) == true);

    std::string smtpPassDecrypted;
    std::string decryptedmailPass = decryptPassword(encryptedmailPass);
    REQUIRE(decryptedmailPass == mailPassword);

    REQUIRE(mailPassword == decryptedmailPass);
}

bool operator==(const SelectedVenue& lhs, const SelectedVenue& rhs) {
    return lhs.name == rhs.name && lhs.email == rhs.email && lhs.city == rhs.city &&
           lhs.genre == rhs.genre && lhs.state == rhs.state && lhs.capacity == rhs.capacity;
}

CATCH_CONFIG_RUNNER // This line will define Catch2's main function

int main( int argc, char* argv[] )
{
    // Run all the tests
    int result = Catch::Session().run(argc, argv);

    // Clean up the mock config file after all tests have been run
    resetConfigFile();

    // Return the test results
    return result;
}