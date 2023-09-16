#!/bin/bash

# Compile and link the tests using the macOS Makefile
make -f Makefile.macos test

# Run the tests with detailed output and save it to a text file
./bin/VenueSender_test -s > test_results.txt

# Clean up
make -f Makefile.macos clean
