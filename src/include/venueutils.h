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


//*Password Encryption Function*//
//---------------------------------//

// Utility function to decrypt the email and smtp passwords
bool decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword,
                      const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey);

// Utility function to encrypt the email and smtp passwords
bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword,
                     const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey);

// Function to initialize encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce);

/*End of Password Encryption Function Declarations*/

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
