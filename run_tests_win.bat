@echo off
setlocal

:: Compile and link the tests using the Makefile for MinGW
make -f Makefile.mingw-win test

:: Run the tests with detailed output and save it to a text file
bin\VenueSenderTest.exe -s > test_results.txt

:: Clean up
make -f Makefile.mingw-win clean

:: Pause to keep the window open
pause

endlocal
