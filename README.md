# VenueSender Project

VenueSender is a command-line tool that helps you send emails to different venues based on specific criteria. It allows you to filter venues by genre, city, state, and capacity and send custom email messages to the selected venues.

## Getting Started

### Prerequisites

To run this project, you need the following installed on your system:

- GCC (GNU Compiler Collection)
- C++17 compatible compiler
- JSONCpp library (https://github.com/open-source-parsers/jsoncpp)
- Catch2

### Dependencies

      sudo apt-get update
      sudo apt-get install g++ libjsoncpp-dev catch2


      git clone https://github.com/catchorg/Catch2.git
      cd Catch2
      mkdir build
      cd build
      cmake ..
      sudo make install

### Installation

1. Clone the repository:

      git clone https://github.com/konklavettv/venuesender.git


Navigate to the project directory:

      cd venue-sender

Compile the project:

      make


### Usage

1. Prepare your config.json file with the necessary SMTP and email settings.

2. Prepare a venues.csv file with the list of venues in the following format:

      ```
      Venue Name 1,email1@example.com,Rock,100,City 1,State 1
      Venue Name 2,email2@example.com,Pop,150,City 2,State 2
      ```


3. Run the Venue Sender:

      ./venuesender

The program will prompt you to select filter options and compose the email subject and message. You can send emails to filtered venues by confirming your choices.


### Running Tests

To run the unit tests, execute:

      ./run_tests.sh


### Contributing

Contributions to this project are welcome. Please open an issue to discuss proposed changes or create a pull request with your contribution.


### License

This project is licensed under the MIT License.


### Acknowledgments

JSONCpp - C++ library for working with JSON data.
