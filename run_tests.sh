#!/bin/bash

# Compile and link the tests
make -f Makefile.static test

# Run the tests with detailed output and save it to a text file
./bin/venuesender_test -s > test_results.txt
