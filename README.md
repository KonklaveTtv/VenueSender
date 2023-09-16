# VenueSender Project v0.1.0 Beta

VenueSender is a command-line tool designed to facilitate the process of sending emails and attachments to different venues based on specific criteria. Users can filter venues by genre, city, state, and capacity to send custom email messages to the selected venues.

## Getting Started

### Prerequisites

VenueSender has multiple Makefiles to build statically with GCC, Clang, ICX or the MinGW-w64 compiler. There is also a Makefile.dynamic to build a dynamically linked version using GCC.

After installing dependencies and you are ready to build invoke:

      make -f Makefile.<compiler/dynamic>


### Installing Dependencies To Build Statically (Ubuntu)

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

The versions mentioned throughout this readme are the ones that VenueSender was built and tested with, it may build with newer versions but this is not guaranteed.


Run the following commands to install the first lot of dependencies:

      sudo apt-get update
      sudo apt-get install make cmake autogen libtool shtool pkg-config xutils-dev g++ libssl-dev libsqlite3-dev libboost-all-dev

Curl v8.2.1 when installing from source must be run like this:

      wget https://github.com/curl/curl/releases/download/curl-8_2_1/curl-8.2.1.tar.gz
      tar -xvf curl-8.2.1.tar.gz
      cd curl-8.2.1
      ./configure --with-openssl --enable-static --disable-shared --without-libidn2 --disable-ldap
      make
      sudo make install 

libjsoncpp 1.9.5-4 can be installed using:

      wget https://launchpad.net/ubuntu/+archive/primary/+sourcefiles/libjsoncpp/1.9.5-4/libjsoncpp_1.9.5.orig.tar.gz
      tar -xvf libjsoncpp_1.9.5.orig.tar.gz
      cd jsoncpp-1.9.5
      mkdir -p build/debug
      cd build/debug
      cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ../../
      make
      sudo make install

libx11 v1.8.6 can be installed using:

      wget https://launchpad.net/ubuntu/+archive/primary/+sourcefiles/libx11/2:1.8.6-1/libx11_1.8.6.orig.tar.gz
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

Once the build has completed, the VenueSender binary is located in the VenueSender/bin directory. To amend the permissions and make the binary executable for other users run:

      chmod +x VenueSender

To make clean:

      make -f Makefile.<compiler/dynamic> clean

### Installing Dependencies & Building Dynamically (Ubuntu)

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

      git clone https://github.com/KonklaveTtv/VenueSender.git

Navigate to the project directory:

      cd VenueSender

Compile the project:

      make -f Makefile.dynamic

Once the build has completed, the VenueSender binary is located in the VenueSender/bin directory. To amend the permissions and make the binary executable for other users run:

      chmod +x VenueSender

Installation (with tests)

Follow the same steps as above but compile using:

      make -f Makefile.dynamic test

To make clean:

      make -f Makefile.dynamic clean

### Installing Dependencies & Building Statically (Windows)

First download, install and run MSYS2:

      https://github.com/msys2/msys2-installer/releases/download/2023-07-18/msys2-x86_64-20230718.exe

Inside the MSYS2 terminal which opens after install enter this line:

      pacman -Syu

MSYS2 should of shut down, if it didn't type exit and then open MSYS2 again and enter:

      pacman -Su

Next enter the below line:

      pacman -S --needed base-devel mingw-w64_64-toolchain

Now open Environment Variables for Windows and add the paths:

      C:\msys64\mingw64\bin;
      C:\msys64\usr\bin;

If everything has gone to plan open a CMD and this line should return the version of GCC:

      gcc --version

Now to install dependencies, open MSYS2 MINGW64 from the start menu and run:

      pacman -S mingw-w64-x86_64-curl-winssl

These are the rest of the deps, do each one-by-one:

      pacman -S mingw-w64-x86_64-jsoncpp
      pacman -S mingw-w64-clang-x86_64-libpsl
      pacman -S mingw-w64-x86_64-openssl
      pacman -S mingw-w64-x86_64-sqlite3
      pacman -S mingw-w64-x86_64-boost

Open CMD and navigate to the root of the VenueSender source directory and run:

      make -f Makefile.mingw

This will output a static Windows binary (VenueSender.exe) to the /VenueSender/bin directory.
Make sure to amend its permissions so the executable can be ran by other people:

      icacls VenueSender.exe /grant Everyone:RX

You can also strip the executable/binary:

      strip VenueSender.exe

Make sure to include the needed DLL files for it to be able to run on other systems:

      libboost_filesystem-mt.dll
      libboost_iostreams-mt.dll
      libboost_regex-mt.dll
      libboost_system-mt.dll
      libbrotlicommon.dll
      libbrotlidec.dll
      libbz2-1.dll
      libcrypto-3-x64.dll
      libcurl-4.dll
      libgcc_s_seh-1.dll
      libiconv-2.dll
      libidn2-0.dll
      libintl-8.dll
      libjsoncpp-25.dll
      libzlma-5.dll
      libpsl-5.dll
      libsqlite3-0.dll
      libssh2-1.dll
      libssl-3-x64.dll
      libstdc++-6.dll
      libunistring-5.dll
      libnwinpthread-1.dll
      libzstd.dll
      zlib.dll


### Installing Dependencies & Building Dynamically (MacOS)

Open a terminal and install the Homebrew package manager and Command Line Tools For Xcode:

      /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

We need a few build tools for the next steps:

      brew install wget cmake

Now to install the dependencies we need to build from source beginning with Curl v8.2.1:

      wget https://github.com/curl/curl/releases/download/curl-8_2_1/curl-8.2.1.tar.gz
      tar -xvf curl-8.2.1.tar.gz
      cd curl-8.2.1
      ./configure --with-openssl --enable-static --disable-shared --without-libidn2 --disable-ldap
      make
      sudo make install 

libjsoncpp 1.9.5 can be installed using:

      wget https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.5.tar.gz
      tar -xvf 1.9.5.tar.gz
      cd jsoncpp-1.9.5
      mkdir -p build/debug
      cd build/debug
      cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -G "Unix Makefiles" ../../
      make
      sudo make install

libpsl v0.21.2 can be installed using:

      wget https://github.com/rockdaboot/libpsl/releases/download/0.21.2/libpsl-0.21.2.tar.gz
      tar -xvf libpsl-0.21.2.tar.gz
      cd libpsl-0.21.2
      ./configure --enable-static --disable-shared
      make
      sudo make install

Now lets install the rest of the dependencies via Homebrew, allow each to complete:

      brew install zlib
      brew install openssl@3
      brew install sqlite
      brew install boost

If you wish to run unit tests, install Catch2:

      brew install catch2

Now navigate to the root of the VenueSender source directory and run:

      make -f Makefile.macos

You may have to edit the paths in the Makefile to the actual locations of the libraries, this is how they are currently pathed:

      STATIC_LIBS = /usr/local/lib/libcurl.a \
                    /usr/local/lib/libpsl.a \
                    /usr/local/lib/libjsoncpp.a \
                    /usr/local/opt/openssl@3.1/lib/libssl.a \
                    /usr/local/opt/openssl@3.1/lib/libcrypto.a \
                    /usr/local/opt/zlib/lib/libz.a \
                    /usr/local/opt/sqlite3/lib/libsqlite3.a \
                    /usr/local/opt/boost@1.82/lib/libboost_system.a \
                    /usr/local/opt/boost@1.82/lib/libboost_filesystem.a \
                    /usr/local/opt/boost@1.82/lib/libboost_iostreams.a \
                    /usr/local/opt/boost@1.82/lib/libboost_regex.a           

Once the build has completed, the VenueSender binary is located in the VenueSender/bin directory. To amend the permissions and make the binary executable for other users run:

      chmod +x VenueSender

To make clean:

      make -f Makefile.macos clean


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

      ./VenueSender


### Running Tests

Run the test binary:

Ubuntu:

      ./VenueSender_test

Windows:

      VenueSenderTest.exe

Mac:

      ./VenueSender_test
Alternatively, run the "run_tests" script, which will output the full test results to test_results.txt:

Ubuntu:

      ./run_tests_linux.sh

Windows:

      run_tests_win.bat

MacOS:

      ./run_tests_macos.sh

### Contributing

Contributions are welcome. Please open an issue to discuss any proposed changes or submit a pull request.


### License

This project is licensed under the MIT License.


### Acknowledgments

- MinGW and MSYS2: For providing a seamless development environment on Windows, facilitating the building and testing process.
- Ubuntu and Apt Package Manager: For the Linux development environment and easy management of some dependencies.
- libjsoncpp: A C++ library for working with JSON data.
- libcurl4: A client-side URL transfer library.
- libx11: The X11 client-side library.
- libssl: A library for implementing Secure Sockets Layer (SSL) and Transport Layer Security (TLS) protocols.
- libsqlite3: A C library that provides a lightweight disk-based database.
- libboost: A set of libraries that extend the functionality of C++.
- Catch2: A modern, C++-native, header-only, test framework for unit tests.
