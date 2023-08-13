#define CATCH_CONFIG_MAIN

#include "test_paths.h"

#include "fileutils.h"
#include "filtercriteria.h"
#include "venuesender.h"

#include "catch.hpp"

#include <cstdlib>
#include <ctime>

TEST_CASE("Test Read CSV", "[csv]") {
    // Set up mock data for readCSV function
    std::vector<Venue> venues;
    std::string venuesCsvPath = TestPaths::mockVenuesCsvPath;

    // Call the readCSV function
    readCSV(venues, venuesCsvPath);

    // Compare the result with expected values
    REQUIRE(venues.size() == 2);

    REQUIRE(venues[0].name == "Top of the Bay");
    REQUIRE(venues[0].email == "topofthebayllc@gmail.com");
    REQUIRE(venues[0].genre == "all");
    REQUIRE(venues[0].state == "AL");
    REQUIRE(venues[0].city == "Daphne");
    REQUIRE(venues[0].capacity == 100);

    REQUIRE(venues[1].name == "Quarters");
    REQUIRE(venues[1].email == "atonuv10@gmail.com");
    REQUIRE(venues[1].genre == "rock");
    REQUIRE(venues[1].state == "UT");
    REQUIRE(venues[1].city == "Provo");
    REQUIRE(venues[1].capacity == 300);
}

TEST_CASE("Test Load Config Settings", "[config]") {
    std::string smtpServer;
    int smtpPort;
    std::string smtpUsername;
    std::string smtpPass;
    std::string venuesCsvPath;
    std::string emailPass;
    std::string senderEmail;
    int senderSmtpPort;

    // Call the loadConfigSettings function with the mock config file path
    bool result = loadConfigSettings(TestPaths::mockConfigJsonPath, smtpServer, smtpPort, smtpUsername, smtpPass,
                                     venuesCsvPath, emailPass, senderEmail, senderSmtpPort);

    // Compare the result with expected values
    REQUIRE(result == true);
    REQUIRE(smtpServer == "mock_smtp_server");
    REQUIRE(smtpPort == 587);
    REQUIRE(smtpUsername == "mock_smtp_username");
    REQUIRE(smtpPass == "mock_smtp_password");  // Check the expected decrypted value
    REQUIRE(venuesCsvPath == "mock_venues_csv_path");
    REQUIRE(emailPass == "enter_email_password");  // Check the expected decrypted value
    REQUIRE(senderEmail == "your_sender_email@example.com");
    REQUIRE(senderSmtpPort == 587);
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

    Venue testVenue("Top of the Bay", "topofthebayllc@gmail.com", "Daphne", "all", "AL", 100);
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
    Venue testVenue1("Top of the Bay", "topofthebayllc@gmail.com", "all", "AL", "Daphne", 100);
    Venue testVenue2("Quarters", "atonuv10@gmail.com", "rock", "UT", "Provo", 300);

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
    REQUIRE(output.find("Sending email 1 of 2 to: topofthebayllc@gmail.com") != std::string::npos);
    REQUIRE(output.find("Sending email 2 of 2 to: atonuv10@gmail.com") != std::string::npos);
    REQUIRE(output.find("Email sending progress completed.") != std::string::npos);
}

TEST_CASE("Test Convert Venue to SelectedVenue", "[convertToSelectedVenue]") {
    // Create a mock Venue
    Venue mockVenue;
    mockVenue.name = "Top of the Bay";
    mockVenue.email = "topofthebayllc@gmail.com";
    mockVenue.city = "Daphne";
    mockVenue.genre = "all";
    mockVenue.state = "AL";
    mockVenue.capacity = 100;

    // Convert Venue to SelectedVenue using the function
    SelectedVenue selectedVenue = convertToSelectedVenue(mockVenue);

    // Compare the converted SelectedVenue with expected values
    REQUIRE(selectedVenue.name == "Top of the Bay");
    REQUIRE(selectedVenue.email == "topofthebayllc@gmail.com");
    REQUIRE(selectedVenue.city == "Daphne");
    REQUIRE(selectedVenue.genre == "all");
    REQUIRE(selectedVenue.state == "AL");
    REQUIRE(selectedVenue.capacity == 100);
}

bool operator==(const SelectedVenue& lhs, const SelectedVenue& rhs) {
    return lhs.name == rhs.name && lhs.email == rhs.email && lhs.city == rhs.city &&
           lhs.genre == rhs.genre && lhs.state == rhs.state && lhs.capacity == rhs.capacity;
}

TEST_CASE("Test Process Venue Selection", "[processVenueSelection]") {
    // Create a mock vector of temporary filtered venues
    Venue testVenue1("Top of the Bay", "topofthebayllc@gmail.com", "Daphne", "all", "AL", 100);
    Venue testVenue2("Quarters", "atonuv10@gmail.com", "Provo", "rock", "UT", 300);
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

    std::string emailPassword = "enter_email_password";

    std::string encryptedEmailPass;
    REQUIRE(encryptPassword(emailPassword, encryptedEmailPass) == true);

    std::string smtpPassDecrypted;
    std::string decryptedEmailPass = decryptPassword(encryptedEmailPass);
    REQUIRE(decryptedEmailPass == emailPassword);

    REQUIRE(emailPassword == decryptedEmailPass);
}

TEST_CASE("Test Reset Config File", "[resetConfigFile]") {
    // Define a temporary config file path for testing
    std::string tempConfigFilePath = "test_config.json";

    // Create an initial config.json content for testing
    std::string initialConfigContent = R"(
        {
            "smtp_pass_encrypted": true,
            "email_pass_encrypted": true,
            "smtp_password": "encrypted_smtp_password",
            "email_password": "encrypted_email_password"
        }
    )";

    // Write the initial config content to the temporary config file
    std::ofstream tempConfigFile(tempConfigFilePath);
    tempConfigFile << initialConfigContent;
    tempConfigFile.close();

    // Call the resetConfigFile function to reset the config.json
    resetConfigFile(tempConfigFilePath);

    // Read the modified config.json content
    Json::Value modifiedConfig;
    std::ifstream configFile(tempConfigFilePath);
    configFile >> modifiedConfig;
    configFile.close();

    // Check if the values in the modified config.json match the expected values
    REQUIRE(modifiedConfig["smtp_pass_encrypted"].asBool() == false);
    REQUIRE(modifiedConfig["email_pass_encrypted"].asBool() == false);
    REQUIRE(modifiedConfig["smtp_password"].asString() == "enter_smtp_password");
    REQUIRE(modifiedConfig["email_password"].asString() == "enter_email_password");

    // Clean up by removing the temporary config file
    std::remove(tempConfigFilePath.c_str());
}
CATCH_CONFIG_MAIN // This line will define Catch2's main function

