# VenueSender Project

VenueSender is a command-line tool designed to facilitate the process of sending emails and attachments to different venues based on specific criteria. Users can filter venues by genre, city, state, and capacity to send custom email messages to the selected venues.

## Getting Started

### Prerequisites

VenueSender has multiple Makefiles to build statically with GCC, Clang, ICX or the MinGW-w64 compiler. There is also a Makefile.dynamic to build a dynamically linked version using GCC.

If you choose to go the static building route, then build using Makefile.dynamic will no longer be possible.

After installing dependencies and you are ready to build invoke:

      make -f Makefile.<compiler/dynamic>


### Installing Dependencies To Build Statically

To build this project statically, you will need the following installed on your system:

- GCC (GNU Compiler Collection)
- C++20 compatible compiler
- Curl v8.2.1 (Install from source)
- libjsoncpp v1.9.5-4 (Install from source)
- libx11 v1.8.6 (Install from source)
- libpsl 0.21.2 (Install from source)
- libssl v3.0.8-1ubuntu1.2 (Install with apt)
- libsqlite3 v3.40.1-1 (Install with apt)
- libboost v1.74.0.3ubuntu7 (Install with apt)
- Catch2 v2.13.10-1 (Install with apt)

The versions mentioned are the ones that VenueSender was built and tested with, it may build with newer versions but this is not guaranteed.


Run the following commands to install the first lot of dependencies:

      sudo apt-get update
      sudo apt-get install make cmake autogen libtool shtool pkg-config xutils-dev g++ libx11-dev libssl-dev libsqlite3-dev libboost-all-dev

Curl v8.2.1 when installing from source must be ran like this:

      wget https://github.com/curl/curl/releases/download/curl-8_2_1/curl-8.2.1.tar.gz
      tar -xvf curl-8.2.1.tar.gz
      cd curl-8.2.1
      ./configure --with-openssl --enable-static --disable-shared
      make
      sudo make install 

libjoncpp 1.9.5-4 can be installed using:

      wget https://launchpad.net/ubuntu/+archive/primary/+sourcefiles/libjsoncpp/1.9.5-4/libjsoncpp_1.9.5.orig.tar.gz
      tar -xvf libjsoncpp_1.9.5.orig.tar.gz
      cd jsoncpp-1.9.5
      mkdir -p build/debug
      cd build/debug
      cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ../../
      make
      sudo make install

libx11 v1.8.6 can be installed using:

      wget https://launchpad.net/ubuntu/+archive/primary/+sourcefiles/libx11/2:1.8.4-2/libx11_1.8.4.orig.tar.gz
      tar -xvf libx11-libX11-1.8.6.tar.gz
      cd libx11-libX11-1.8.6
      ./autogen.sh
      ./configure --enable-static --disable-shared
      make
      sudo make install

libpsl v0.21.2 can be installed using:

      wget https://github.com/rockdaboot/libpsl/releases/download/0.21.2/libpsl-0.21.2.tar.gz
      tar -xvf libpsl-0.21.2.tar.gz
      cd libpsl-0.21.2
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

To make clean:

      make -f Makefile.<compiler/dynamic> clean

### Installing Dependencies To Build Dynamically

To build this project dynamically, you will need the following installed on your system:

- GCC (GNU Compiler Collection)
- C++20 compatible compiler
- libjsoncpp
- Curl
- libx11
- libssl
- libsqlite3
- libboost
- Catch2

The versions mentioned are the ones that VenueSender was built and tested with, it may build with newer versions but this is not guaranteed.


Run the following commands to install the above dependencies:

      sudo apt-get update
      sudo apt-get install make g++ libjsoncpp-dev libcurl4-openssl-dev zlib1g-dev libx11-dev libxcb1-dev libxau-dev libxdmcp-dev libssl-dev libsqlite3-dev libboost-all-dev

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

To make clean:

      make -f Makefile.dynamic clean


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