#!/bin/bash

# Run the test executable
./bin/venuesender_test 2>&1 > test_output.txt

# Check the exit code of the executable
if [ $? -eq 0 ]; then
    echo "Tests ran successfully!"
else
    echo "Tests failed. Check 'test_output.txt' for details."
fi