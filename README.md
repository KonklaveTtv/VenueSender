# VenueSender Project

VenueSender is a command-line tool that helps you send emails/attachments to different venues based on specific criteria. It allows you to filter venues by genre, city, state, and capacity and send custom email messages to the selected venues.

## Getting Started

### Prerequisites

To run this project, you need the following installed on your system:

- GCC (GNU Compiler Collection)
- C++20 compatible compiler
- libjsoncpp
- libcurl
- libX11
- Catch2

### Dependencies

      sudo apt-get update
      sudo apt-get install make g++ libjsoncpp-dev libcurl4-openssl-dev libx11-dev

Install this dependency if you wish to run unit tests:

      catch2

### Installation (without tests)

Clone the repository:

      git clone https://github.com/konklavettv/venuesender.git


Navigate to the project directory:

      cd venuesender

Compile the project:

      make


### Installation (with tests)

Clone the repository:

      git clone https://github.com/konklavettv/venuesender.git


Navigate to the project directory:

      cd venuesender

Compile the project:

      make test


### Usage

Prepare your config.json file with the necessary SMTP and email settings:

      ```
      {
      "sender_email" : "enter_your_sender_email",
      "smtp_port" : 587,
      "smtp_server" : "enter_your_smtp_server",
      "smtp_username" : "enter_your_smtp_username",
      "useSSL" : false,
      "venues_csv_path" : "venues.csv",
      "verifyHost" : false,
      "verifyPeer" : false,
      "verbose" : false
      }
            ```

Prepare a venues.csv file with the list of venues in the following format:

      ```
      Venue1,venue1@mock.com,USA,AL,Daphne,100,Mixed
      Venue2,venue2@mock.com,France,Paris Region,Paris,300,Rock
      ```

Run VenueSender:

      ./venuesender

The program will prompt you to select filter options and compose the email subject and message. You can send emails to filtered venues by confirming your choices.


### Running Tests

To run the test binary

      ./bin/venuesender 

or execute:

      ./bin/venuesender_test -s

or run the bash script which will output the full test results to test_results.txt:

      ./run_tests.sh

### Contributing

Contributions to this project are welcome. Please open an issue to discuss proposed changes or create a pull request with your contribution.


### License

This project is licensed under the MIT License.


### Acknowledgments

JSONCpp - C++ library for working with JSON data.
