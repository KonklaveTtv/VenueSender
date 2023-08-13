#define CATCH_CONFIG_MAIN

#include "fileutils.h"
#include "filtercriteria.h"
#include "venuesender.h"

#include "catch.hpp"

#include <cstdlib>
#include <ctime>

TEST_CASE("Test Read CSV", "[csv]") {
    // Set up mock data for readCSV function
    std::vector<Venue> venues;
    std::string venuesCsvPath = "src/test/mock_venues.csv";

    // Call the readCSV function
    readCSV(venues, venuesCsvPath);

    // Compare the result with expected values
    REQUIRE(venues.size() == 4);
    REQUIRE(venues[0].name == "Venue A");
    REQUIRE(venues[0].email == "venueA@example.com");
    // ... Add more assertions for other fields
}

TEST_CASE("Test Load Config Settings", "[config]") {
    std::string smtpServer;
    int smtpPort;
    std::string smtpUsername;
    std::string smtpPass;
    std::string venuesCsvPath;
    std::string emailPassword;
    std::string senderEmail;
    int senderSmtpPort;

    // Call the loadConfigSettings function with the mock config file path
    bool result = loadConfigSettings("src/test/mock_config.json", smtpServer, smtpPort, smtpUsername, smtpPass,
                                     venuesCsvPath, emailPassword, senderEmail, senderSmtpPort);

    // Compare the result with expected values
    REQUIRE(result == true);
    REQUIRE(smtpServer == "mock_smtp_server"); // Assuming this is the value in mock_config.json
    REQUIRE(smtpPort == 587); // Assuming this is the value in mock_config.json
    REQUIRE(emailPassword == "enter_email_password"); // Assuming this is the value in mock_config.json
    REQUIRE(senderEmail == "sender@example.com"); // Assuming this is the value in mock_config.json
    REQUIRE(senderSmtpPort == 25); // Assuming this is the value in mock_config.json
}

TEST_CASE("Test Email Validation", "[validation]") {
    // Set up mock data for email validation
    std::string validEmail = "test@example.com";
    std::string invalidEmail = "invalid.email";

    // Call the isValidEmail function
    bool isValid1 = isValidEmail(validEmail);
    bool isValid2 = isValidEmail(invalidEmail);

    // Compare the result with expected values
    REQUIRE(isValid1 == true);
    REQUIRE(isValid2 == false);
}

TEST_CASE("Test Trim Function", "[trim]") {
    std::string stringWithSpaces = "   Hello, World!   ";
    std::string stringWithoutSpaces = "Hello, World!";

    std::string trimmedString = Catch::trim(stringWithSpaces);

    REQUIRE(trimmedString == stringWithoutSpaces);
}

TEST_CASE("Test Send Individual Email", "[email]") {
    // Set up mock data for sendIndividualEmail function
    CurlHandleWrapper curlWrapper; // Using the CurlHandleWrapper class
    CURL* curl = curlWrapper.get(); // Get the CURL handle from the wrapper

    Venue testVenue("Venue", "venue@example.com", "City", "Genre", "State", 100);
    SelectedVenue selectedVenue = testVenue;
    std::string senderEmail = "sender@example.com";
    std::string subject = "Mock Subject";
    std::string message = "Mock Message";
    std::string smtpServer = "mock_smtp_server";
    int smtpPort = 12345;
    std::string smtpUsername = "mock_smtp_username";
    std::string smtpPass = "mock_smtp_password";

    // Set up a mock response for the email sending
    curl_easy_setopt(curl, CURLOPT_URL, ("smtp://" + smtpServer + ":" + std::to_string(smtpPort)).c_str());
    std::string smtpUserPass = smtpUsername + ":" + smtpPass;
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUserPass.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, senderEmail.c_str());
    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, selectedVenue.email.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    struct curl_slist* headers = nullptr;
    std::string subjectHeader = "Subject: " + subject;
    headers = curl_slist_append(headers, subjectHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, static_cast<long>(message.length()));
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    // Call the sendIndividualEmail function
    double progress = 0.0; // Progress value for testing
    bool result = sendIndividualEmail(curl, selectedVenue, senderEmail, subject, message,
                                      smtpServer, smtpPort, smtpUsername, smtpPass, progress);

    // Compare the result with expected values
    REQUIRE(result == true);

    // Clean up
    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);
}

TEST_CASE("Test View Email Sending Progress", "[email]") {
    // Set up mock data for viewEmailSendingProgress function
    CurlHandleWrapper curlWrapper;
    CURL* curl = curlWrapper.get();
    std::vector<SelectedVenue> selectedVenuesForEmail;
    std::string senderEmail = "sender@example.com";
    std::string subject = "Mock Subject";
    std::string message = "Mock Message";
    std::string smtpServer = "mock_smtp_server";
    int smtpPort = 12345;
    std::string smtpUsername = "mock_smtp_username";
    std::string smtpPass = "mock_smtp_password";

    // Simulate adding some selected venues
    Venue testVenue1("Venue 1", "venue1@example.com", "Genre A", "State X", "City A", 100);
    Venue testVenue2("Venue 2", "venue2@example.com", "Genre B", "State Y", "City B", 150);

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
    REQUIRE(output.find("Sending email 1 of 2 to: venue1@example.com") != std::string::npos);
    REQUIRE(output.find("Sending email 2 of 2 to: venue2@example.com") != std::string::npos);
    REQUIRE(output.find("Email sending progress completed.") != std::string::npos);
}

TEST_CASE("Test Convert Venue to SelectedVenue", "[convertToSelectedVenue]") {
    // Create a mock Venue
    Venue mockVenue;
    mockVenue.name = "Mock Venue";
    mockVenue.email = "mock@example.com";
    mockVenue.city = "Mock City";
    mockVenue.genre = "Mock Genre";
    mockVenue.state = "Mock State";
    mockVenue.capacity = 200;

    // Convert Venue to SelectedVenue using the function
    SelectedVenue selectedVenue = convertToSelectedVenue(mockVenue);

    // Compare the converted SelectedVenue with expected values
    REQUIRE(selectedVenue.name == "Mock Venue");
    REQUIRE(selectedVenue.email == "mock@example.com");
    REQUIRE(selectedVenue.city == "Mock City");
    REQUIRE(selectedVenue.genre == "Mock Genre");
    REQUIRE(selectedVenue.state == "Mock State");
    REQUIRE(selectedVenue.capacity == 200);
}

bool operator==(const SelectedVenue& lhs, const SelectedVenue& rhs) {
    return lhs.name == rhs.name && lhs.email == rhs.email && lhs.city == rhs.city &&
           lhs.genre == rhs.genre && lhs.state == rhs.state && lhs.capacity == rhs.capacity;
}

TEST_CASE("Test Process Venue Selection", "[processVenueSelection]") {
    // Create a mock vector of temporary filtered venues
    Venue testVenue1("Venue 1", "venue1@example.com", "City A", "Genre A", "State X", 100);
    Venue testVenue2("Venue 2", "venue2@example.com", "City B", "Genre B", "State Y", 150);
    Venue testVenue3("Venue 3", "venue3@example.com", "City C", "Genre C", "State Z", 200);
    std::vector<Venue> temporaryFilteredVenues = {testVenue1, testVenue2, testVenue3};

    // Convert Venue objects to SelectedVenue objects
    std::vector<SelectedVenue> selectedVenuesForEmail;
    for (const Venue& venue : temporaryFilteredVenues) {
        selectedVenuesForEmail.push_back(convertToSelectedVenue(venue));
    }

    // Simulate user input of selecting venues 1 and 3
    std::stringstream input;
    input << "1,3\n";
    std::cin.rdbuf(input.rdbuf());

    // Call the processVenueSelection function
    processVenueSelection(selectedVenuesForEmail, selectedVenuesForEmail);

    // Convert the selected venues using convertToSelectedVenue
    SelectedVenue selectedVenue1 = convertToSelectedVenue(testVenue1);
    SelectedVenue selectedVenue2 = convertToSelectedVenue(testVenue2);
    SelectedVenue selectedVenue3 = convertToSelectedVenue(testVenue3);

    // Compare the selectedVenuesForEmail vector with expected values
    REQUIRE(selectedVenuesForEmail.size() == 2);
    REQUIRE(selectedVenuesForEmail[0] == selectedVenue1);
    REQUIRE(selectedVenuesForEmail[1] == selectedVenue3);
}

TEST_CASE("Read venues data from CSV file", "[readCSV]") {
    // Create a temporary CSV file with mock data
    std::ofstream csvFile("test_venues.csv");
    csvFile << "Name,Email,Genre,State,City,Capacity\n"
            << "Venue A,venueA@example.com,Rock,California,Los Angeles,500\n"
            << "Venue B,venueB@example.com,Jazz,New York,New York,300\n"
            << "Venue C,venueC@example.com,Pop,Texas,Austin,200\n"
            << "Venue D,venueD@example.com,Electronic,Florida,Miami,1000\n";
    csvFile.close();

    // Vector to store the read venues
    std::vector<Venue> venues;

    // Call the readCSV function to populate the venues vector
    readCSV(venues, "test_venues.csv");

    // Remove the temporary CSV file
    std::remove("test_venues.csv");

    // Write your assertions here to test the readCSV function
    REQUIRE(venues.size() == 4);
    REQUIRE(venues[0].name == "Venue A");
    REQUIRE(venues[0].email == "venueA@example.com");
    // ... Add more assertions for other fields
}

TEST_CASE("Encrypt and decrypt SMTP password", "[encryption][decryption]") {
    std::array<unsigned char, crypto_secretbox_KEYBYTES> encryptionKey;
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> encryptionNonce;
    initializeEncryptionParams(encryptionKey, encryptionNonce);

    std::string smtpPassword = "smtpPassword123";

    std::string encryptedSmtpPass;
    REQUIRE(encryptPassword(smtpPassword, encryptedSmtpPass, encryptionKey) == true);

    std::string decryptedSmtpPass;
    REQUIRE(decryptPassword(encryptedSmtpPass, decryptedSmtpPass, encryptionKey) == true);

    REQUIRE(smtpPassword == decryptedSmtpPass);
}

TEST_CASE("Encrypt and decrypt email password", "[encryption][decryption]") {
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Rest of your test case code
    std::array<unsigned char, crypto_secretbox_KEYBYTES> encryptionKey;
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> encryptionNonce;
    initializeEncryptionParams(encryptionKey, encryptionNonce);

    std::string emailPassword = "emailPassword456";

    std::string encryptedEmailPass;
    REQUIRE(encryptPassword(emailPassword, encryptedEmailPass, encryptionKey) == true);

    std::string decryptedEmailPass;
    REQUIRE(decryptPassword(encryptedEmailPass, decryptedEmailPass, encryptionKey) == true);

    REQUIRE(emailPassword == decryptedEmailPass);
}
CATCH_CONFIG_MAIN // This line will define Catch2's main function
