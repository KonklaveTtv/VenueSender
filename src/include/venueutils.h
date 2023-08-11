#ifndef VENUEUTILS_H
#define VENUEUTILS_H

#include "venue.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

#include <sodium.h>

class Venue;


//*Password Conversion Function*//
//---------------------------------//

// Utility function to convert binary data to hex
std::string binToHex(const std::string& input);

// Utility function to convert hex data to string
std::string hexToString(const std::string& hex);

// Convert plain-text password to hex
std::string stringToHex(const std::string& input);

// Convert the Email Password from Plain Text to Hex(stringToHex)
std::string EmailPassToHex(const std::string& emailPassword);

// Convert the Smtp Password from Plain Text to Hex (stringToHex)
std::string SmtpPassToHex(const std::string& smtpPass);

// Convert the Email Password from Hex to Plain Text (hexToString)
std::string EmailPassHexToString(const std::string& emailPassHex);

// Convert the Smtp Password from Hex to Plain Text (hexToString)
std::string SmtpPassHexToString(const std::string& smtpPassHex);

/*End of Password Conversion Function Declarations*/

// Function to initialize encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce);

// Declaration for getUniqueValues with a member pointer to a string
std::vector<std::string> getUniqueValues(const std::vector<Venue>& venues, std::string Venue::* memberPtr);

// Declaration for getUniqueValues with a member pointer to an integer
std::vector<int> getUniqueValues(const std::vector<Venue>& venues, int Venue::* memberPtr);

// Definition of getSelectedIndices for vector of any type
template<class T>
std::vector<int> getSelectedIndices(const std::vector<T>& options, std::istream& input) {
    // Get selected indices from user input and return as vector
    std::cout << "Enter the indices of options (comma-separated): ";
    std::string inputStr;
    std::getline(input, inputStr);
    std::istringstream iss(inputStr);
    std::string indexStr;
    std::vector<int> selectedIndices;
    while (std::getline(iss, indexStr, ',')) {
        int index = std::stoi(indexStr);
        if (index >= 1 && index <= static_cast<int>(options.size())) {
            selectedIndices.push_back(index - 1);
        }
    }
    return selectedIndices;
}

// Definition of getSelectedIndices
template<class T>
std::vector<int> getSelectedIndices(const std::vector<T>& options, std::istream& input);

#endif // VENUEUTILS_H
