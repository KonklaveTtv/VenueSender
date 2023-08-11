#include "venueutils.h"
#include "venue.h"

#include <array>

// Utility function to get unique genres from a vector of venues
std::set<std::string> getUniqueGenres(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueGenres;
    for (const auto& venue : venues) {
        if (venue.genre != "all") {  // Exclude the "all" genre
            uniqueGenres.insert(venue.genre);
        }
    }
    return uniqueGenres;
}

// Utility function to get unique states from a vector of venues
std::set<std::string> getUniqueStates(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueStates;
    for (const auto& venue : venues) {
        if (uniqueStates.find(venue.state) == uniqueStates.end()) {
            uniqueStates.insert(venue.state);
        }
    }
    return uniqueStates;
}

// Utility function to get unique cities from a vector of venues
std::set<std::string> getUniqueCities(const std::vector<Venue>& venues) {
    std::set<std::string> uniqueCities;
    for (const auto& venue : venues) {
        if (uniqueCities.find(venue.city) == uniqueCities.end()) {
            uniqueCities.insert(venue.city);
        }
    }
    return uniqueCities;
}

// Utility function to get unique capacities from a vector of venues
std::set<int> getUniqueCapacities(const std::vector<Venue>& venues) {
    std::set<int> uniqueCapacities;
    for (const Venue& venue : venues) {
        uniqueCapacities.insert(venue.capacity);
    }
    return uniqueCapacities;
}

// Utility function to get unique values of a member using a member pointer
std::vector<std::string> getUniqueValues(const std::vector<Venue>& venues, std::string Venue::* memberPtr) {
    std::vector<std::string> uniqueValues;
    for (const Venue& venue : venues) {
        std::string value = venue.*memberPtr;
        if (std::find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end()) {
            uniqueValues.push_back(value);
        }
    }
    return uniqueValues;
}

// Utility function to get unique values of a member using a member pointer
std::vector<int> getUniqueValues(const std::vector<Venue>& venues, int Venue::* memberPtr) {
    std::vector<int> uniqueValues;
    for (const Venue& venue : venues) {
        int value = venue.*memberPtr;
        if (std::find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end()) {
            uniqueValues.push_back(value);
        }
    }
    return uniqueValues;
}

// Convert plain-text password to hex
std::string stringToHex(const std::string& input) {
    std::string hex;
    for (char c : input) {
        hex += "0123456789ABCDEF"[((c >> 4) & 0xF)];
        hex += "0123456789ABCDEF"[((c >> 0) & 0xF)];
    }
    return hex; // Return hex, not stringToHex
}

// Utility function to convert hex data to string
std::string hexToString(const std::string& hex) {
    std::string str;
    str.reserve(hex.size() / 2);

    for (size_t i = 0; i < hex.size(); i += 2) {
        char byte = (hex[i] <= '9' ? hex[i] - '0' : hex[i] - 'A' + 10) << 4;
        byte |= hex[i + 1] <= '9' ? hex[i + 1] - '0' : hex[i + 1] - 'A' + 10;
        str.push_back(byte);
    }

    return str; // Return str, not hexToString
}

// Utility function to convert binary data to hex
std::string binToHex(const std::string& input) {
    static const char* hex_chars = "0123456789ABCDEF";
    std::string hex;
    hex.reserve(input.size() * 2);

    for (unsigned char c : input) {
        hex.push_back(hex_chars[c >> 4]);
        hex.push_back(hex_chars[c & 0xF]);
    }

    return hex;
}

// Convert the Email Password from Plain Text to Hex (stringToHex)
std::string EmailPassToHex(const std::string& emailPassword) {
    // Convert plain-text email password to hex
    std::string emailPassHex = stringToHex(emailPassword);

    return emailPassHex;
}

// Convert the Smtp Password from Plain Text to Hex (stringToHex)
std::string SmtpPassToHex(const std::string& smtpPass) {
    // Convert plain-text SMTP password to hex
    std::string smtpPassHex = stringToHex(smtpPass);

    return smtpPassHex;
}

// Convert the Email Password from Plain Text to Hex (stringToHex)
std::string EmailPassHexToString(const std::string& emailPassHex) {
    // Convert plain-text email password to hex
    std::string emailPass = hexToString(emailPassHex);

    return emailPassHex;
}

// Convert the Smtp Password from Plain Text to Hex (stringToHex)
std::string SmtpPassHexToString(const std::string& smtpPassHex) {
    // Convert plain-text SMTP password to hex
    std::string smtpPass = hexToString(smtpPassHex);

    return smtpPassHex;
}

// Initialize the encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce) {
    // Generate a random encryption key
    randombytes(encryptionKey.data(), crypto_secretbox_KEYBYTES);

    // Generate a random nonce
    randombytes(encryptionNonce.data(), crypto_secretbox_NONCEBYTES);
}