#!/bin/bash

# Compile and link the tests
make -f Makefile.static-linux test

# Run the tests with detailed output and save it to a text file
./bin/VenueSender_test -s > test_results.txt && make -f Makefile.static-linux clean