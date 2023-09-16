@echo off
setlocal

:: Compile and link the tests using the Makefile for MinGW
make -f Makefile.mingw test

:: Run the tests with detailed output and save it to a text file
bin\VenueSenderTest.exe -s > test_results.txt

:: Clean up
make -f Makefile.mingw clean

:: Pause to keep the window open
pause

endlocal
