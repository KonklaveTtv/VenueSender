#include "catch.hpp"

#include "filtercriteria.h"
#include "venuesender.h"
#include "venueutils.h"
#include "venue.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <json/json.h>

TEST_CASE("Test Load Config Settings", "[config]") {
    // Set up mock data for config settings
    std::string smtpServer = "mock_smtp_server";
    int smtpPort = 12345;
    std::string smtpUsername = "mock_smtp_username";
    std::string smtpPass = "mock_smtp_password";
    std::string venuesCsvPath = "mock_venues.csv";
    std::string emailPassword = "mock_email_password";
    std::string senderEmail = "mock_sender_email";
    int senderSmtpPort = 54321;

    // Call the loadConfigSettings function
    bool result = loadConfigSettings(smtpServer, smtpPort, smtpUsername, smtpPass,
                                     venuesCsvPath, emailPassword, senderEmail, senderSmtpPort);

    // Compare the result with expected values
    REQUIRE(result == true);
    REQUIRE(smtpServer == "mock_smtp_server");
    REQUIRE(smtpPort == 12345);
    REQUIRE(emailPassword == "mock_email_password");
    REQUIRE(senderEmail == "mock_sender_email");
    REQUIRE(senderSmtpPort == 54321);
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

TEST_CASE("Test Read CSV", "[csv]") {
    // Set up mock data for readCSV function
    std::vector<Venue> venues;
    std::string venuesCsvPath = "mock_venues.csv";

    // Call the readCSV function
    readCSV(venues, venuesCsvPath);

    // Compare the result with expected values
    REQUIRE(venues.size() == 4);
    REQUIRE(venues[0].name == "Venue A");
    REQUIRE(venues[0].email == "venueA@example.com");
    // ... Add more assertions for other fields
}

TEST_CASE("Test Send Individual Email", "[email]") {
    // Set up mock data for sendIndividualEmail function
    CURL* curl = curl_easy_init();  // Initialize a mock CURL handle
    SelectedVenue selectedVenue("Venue", "venue@example.com", "City", "Genre", "State", 100);
    std::string senderEmail = "sender@example.com";
    std::string subject = "Mock Subject";
    std::string message = "Mock Message";
    std::string smtpServer = "mock_smtp_server";
    int smtpPort = 12345;
    std::string smtpUsername = "mock_smtp_username";
    std::string smtpPass = "mock_smtp_password";

    // Set up a mock response for the email sending
    curl_easy_setopt(curl, CURLOPT_URL, "smtp://mock_smtp_server:12345");
    curl_easy_setopt(curl, CURLOPT_USERNAME, "mock_smtp_username:mock_smtp_password");
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
    CURLcode res = CURLE_OK;
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, nullptr);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    if (res != CURLE_OK) {
        std::cerr << "Failed to send email: " << curl_easy_strerror(res) << std::endl;
    }

    // Call the sendIndividualEmail function
    bool result = sendIndividualEmail(curl, selectedVenue, senderEmail, subject, message,
                                      smtpServer, smtpPort, smtpUsername, smtpPass);

    // Compare the result with expected values
    REQUIRE(result == true);

    // Clean up
    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

TEST_CASE("Test View Email Sending Progress", "[email]") {
    // Set up mock data for viewEmailSendingProgress function
    CURL* curl = curl_easy_init();  // Initialize a mock CURL handle
    std::vector<SelectedVenue> selectedVenuesForEmail;

    // Simulate adding some selected venues
    SelectedVenue venue1("Venue 1", "venue1@example.com", "City A", "Genre A", "State X", 100);
    SelectedVenue venue2("Venue 2", "venue2@example.com", "City B", "Genre B", "State Y", 150);
    selectedVenuesForEmail.push_back(venue1);
    selectedVenuesForEmail.push_back(venue2);

    // Redirect cout to capture console output
    std::stringstream outputCapture;
    std::streambuf* coutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(outputCapture.rdbuf());

    // Call the viewEmailSendingProgress function
    viewEmailSendingProgress(curl, selectedVenuesForEmail);

    // Restore cout
    std::cout.rdbuf(coutBuffer);

    // Compare the result with expected values
    std::string output = outputCapture.str();
    REQUIRE(output.find("Sending email to: venue1@example.com") != std::string::npos);
    REQUIRE(output.find("Sending email to: venue2@example.com") != std::string::npos);
    REQUIRE(output.find("Email sending progress completed.") != std::string::npos);

    // Clean up
    curl_easy_cleanup(curl);
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

TEST_CASE("Test Process Venue Selection", "[processVenueSelection]") {
    // Create a mock vector of temporary filtered venues
    std::vector<SelectedVenue> temporaryFilteredVenues;
    SelectedVenue venue1("Venue 1", "venue1@example.com", "City A", "Genre A", "State X", 100);
    SelectedVenue venue2("Venue 2", "venue2@example.com", "City B", "Genre B", "State Y", 150);
    SelectedVenue venue3("Venue 1", "venue1@example.com", "City A", "Genre A", "State X", 100);
    temporaryFilteredVenues.push_back(venue1);
    temporaryFilteredVenues.push_back(venue2);
    temporaryFilteredVenues.push_back(venue3);

    // Create a mock vector for selected venues
    std::vector<SelectedVenue> selectedVenuesForEmail;

    // Simulate user input of selecting venues 1 and 3
    std::stringstream input;
    input << "1,3\n";
    std::cin.rdbuf(input.rdbuf());

    // Call the processVenueSelection function
    processVenueSelection(temporaryFilteredVenues, selectedVenuesForEmail);

    // Compare the selectedVenuesForEmail vector with expected values
    REQUIRE(selectedVenuesForEmail.size() == 2);
    REQUIRE(selectedVenuesForEmail[0].name == "Venue 1");
    REQUIRE(selectedVenuesForEmail[1].name == "Venue 3");
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
