# VenueSender Project

VenueSender is a command-line tool designed to facilitate the process of sending emails and attachments to different venues based on specific criteria. Users can filter venues by genre, city, state, and capacity to send custom email messages to the selected venues.


## Getting Started

### Prerequisites

VenueSender by default builds a static binary. If you wish to not build statically and instead want to do a dynamic build, simply rename 'Makefile' to 'Makefile static' and 'Makefile dynamic' to 'Makefile'.

To run this project, you need the following installed on your system:

- GCC (GNU Compiler Collection)
- C++20 compatible compiler
- libjsoncpp v1.9.5-4 (Install with apt-get install)
- Curl v7.88.1 (Install from source)
- libx11 v1.8.4 (Install from source)
- libssl v3.0.8-1ubuntu1.2 (Install with apt-get install)
- libsqlite3 v3.40.1-1 (Install with apt-get install)
- libboost v1.74.0.3ubuntu7 (Install with apt-get install)
- Catch2 v2.13.10-1 (Install with apt-get install)

### Installing Dependencies To Build Statically

Run the following commands to install the first lot of dependencies:

      sudo apt-get update
      sudo apt-get install make g++ libx11-dev libssl-dev libsqlite3-dev libboost-all-dev

Curl v7.88.1 when installing from source must be ran like this:

      ./configure --with-openssl --enable-static --disable-shared
      make
      sudo make install 

The other dependencies can be installed using:

      ./configure --enable-static --disable-shared
      make
      sudo make install

If you wish to run unit tests, install Catch2:

      sudo apt-get install catch2

You may have to edit the paths in the Makefile to the actual locations of the libraries, this is how they are currently pathed:

      STATIC_LIBS = /usr/local/lib/libcurl.a \
                    /usr/local/lib/libpsl.a \
                    /usr/lib/x86_64-linux-gnu/libz.a \
                    /usr/local/lib/libX11.a \
                    /usr/lib/x86_64-linux-gnu/libxcb.a \
                    /usr/lib/x86_64-linux-gnu/libXau.a \
                    /usr/lib/x86_64-linux-gnu/libXdmcp.a \
                    /usr/local/lib/libjsoncpp.a \
                    /usr/lib/x86_64-linux-gnu/libssl.a \
                    /usr/lib/x86_64-linux-gnu/libcrypto.a \
                    /usr/lib/x86_64-linux-gnu/libsqlite3.a \
                    /usr/lib/x86_64-linux-gnu/libboost_system.a \
                    /usr/lib/x86_64-linux-gnu/libboost_filesystem.a \
                    /usr/lib/x86_64-linux-gnu/libboost_iostreams.a \
                    /usr/lib/x86_64-linux-gnu/libboost_regex.a              


### Installing Dependencies To Build Dynamically

Run the following commands to install the dependencies:

      sudo apt-get update
      sudo apt-get install make g++ libjsoncpp-dev libcurl4-openssl-dev libx11-dev libssl-dev libsqlite3-dev libboost-all-dev

To run unit tests, install Catch2:

      catch2


### Installation (without tests)

Clone the repository:

      git clone https://github.com/konklavettv/venuesender.git

Navigate to the project directory:

      cd venuesender

Compile the project:

      make

Installation (with tests)

Follow the same steps as above but compile using:

      make test


### Usage

Prepare your config.json file with the necessary SMTP and email settings:

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

Prepare a custom venues.csv file with the list of your personalised venues in the following format:

      Venue1,venue1@mock.com,USA,AL,Daphne,100,Mixed
      Venue2,venue2@mock.com,France,Paris Region,Paris,300,Rock

Run VenueSender:

      ./venuesender

The program will prompt you to select filter options and compose the email subject and message. You can send emails to filtered venues by confirming your choices.


### Running Tests

Run the test binary:

      ./bin/venuesender 

Or execute:

      ./bin/venuesender_test -s

Alternatively, run the bash script, which will output the full test results to test_results.txt:

      ./run_tests.sh


### Contributing

Contributions are welcome. Please open an issue to discuss any proposed changes or submit a pull request.


### License

This project is licensed under the MIT License.


### Acknowledgments

- libjsoncpp: A C++ library for working with JSON data.
- libcurl4: A client-side URL transfer library.
- libx11: The X11 client-side library.
- libssl: A library for implementing Secure Sockets Layer (SSL) and Transport Layer Security (TLS) protocols.
- libsqlite3: A C library that provides a lightweight disk-based database.
- libboost: A set of libraries that extend the functionality of C++.
- Catch2: A modern, C++-native, header-only, test framework for unit tests.