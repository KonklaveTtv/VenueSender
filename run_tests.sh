#!/bin/bash

# Full path to the venuesender_test executable
TEST_EXECUTABLE="./bin/venuesender_test"

# Run the test executable
$TEST_EXECUTABLE

# Capture the exit code of the test executable
TEST_EXIT_CODE=$?

# Check if the tests passed or failed
if [ $TEST_EXIT_CODE -eq 0 ]; then
    echo "Tests passed."
else
    echo "Tests failed. Check 'test_output.txt' for details."
fi