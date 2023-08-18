# VenueSender Project

VenueSender is a command-line tool that helps you send emails to different venues based on specific criteria. It allows you to filter venues by genre, city, state, and capacity and send custom email messages to the selected venues.

## Getting Started

### Prerequisites

To run this project, you need the following installed on your system:

- GCC (GNU Compiler Collection)
- C++17 compatible compiler
- libjsoncpp
- libcurl
- libsodium
- Catch2

### Dependencies

      sudo apt-get update
      sudo apt-get install make g++ libjsoncpp-dev libcurl4-openssl-dev libsodium-dev

Install this dependency if you wish to run unit tests:

      catch2

### Installation (without tests)

1. Clone the repository:

      git clone https://github.com/konklavettv/venuesender.git


Navigate to the project directory:

      cd venuesender

Compile the project:

      make


### Installation (with tests)

2. Clone the repository:

      git clone https://github.com/konklavettv/venuesender.git


Navigate to the project directory:

      cd venuesender

Compile the project:

      make test


### Usage

3. Prepare your config.json file with the necessary SMTP and email settings:
            
            ```
            {
            "email_pass_encrypted" : false, // don't change this
            "email_password" : "enter_email_password", // Replace with your email password
            "sender_email" : "enter_your_sender_email", // Replace with your sender email address
            "smtp_port" : 587, // Replace with your SMTP port number (i.e. 587, 443)
            "smtp_server" : "enter_your_smtp_server", // Replace with your SMTP server address
            "smtp_username" : "enter_your_smtp_username", // Replace with your SMTP username
            "useSSL" : false, // Set to true or false
            "venues_csv_path" : "venues.csv", // Provide the path to your venues CSV file
            "verifyHost" : false, // Set to true or false
            "verifyPeer" : false, // Set to true or false
            "verbose" : false // Set to true or false
            }
            ```

4. Prepare a venues.csv file with the list of venues in the following format:

      ```
      Venue1,venue1@mock.com,all,AL,Daphne,100
      Venue2,venue2@mock.com,rock,UT,Provo,300
      ```


5. Run VenueSender:

      ./venuesender

The program will prompt you to select filter options and compose the email subject and message. You can send emails to filtered venues by confirming your choices.


### Running Tests

To run the test binary

      ./bin/venuesender 

To run the unit tests, either execute:

      ./run_tests.sh

or execute:

      ./bin/venuesender_test -s

### Contributing

Contributions to this project are welcome. Please open an issue to discuss proposed changes or create a pull request with your contribution.


### License

This project is licensed under the MIT License.


### Acknowledgments

JSONCpp - C++ library for working with JSON data.
