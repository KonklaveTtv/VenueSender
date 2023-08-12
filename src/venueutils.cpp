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

bool decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword,
                      const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey) {
    // Check if libsodium has been properly initialized
    if (sodium_init() < 0) {
        std::cerr << "Error initializing libsodium." << std::endl;
        return false;
    }

    // Define the encryption nonce length
    const size_t NONCE_LENGTH = crypto_secretbox_NONCEBYTES;

    // Check if the encrypted password is long enough to contain the nonce
    if (encryptedPassword.size() < NONCE_LENGTH) {
        std::cerr << "Invalid encrypted password format." << std::endl;
        return false;
    }

    // Extract the encryption nonce from the encrypted password
    std::string nonce = encryptedPassword.substr(0, NONCE_LENGTH);

    // Extract the ciphertext from the encrypted password
    std::string ciphertext = encryptedPassword.substr(NONCE_LENGTH);

    // Initialize a buffer for the decrypted password
    unsigned char decryptedBuffer[ciphertext.size()];

    // Decrypt the ciphertext
    if (crypto_secretbox_open_easy(decryptedBuffer, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size(),
                                   reinterpret_cast<const unsigned char*>(nonce.c_str()), encryptionKey.data()) != 0) {
        std::cerr << "Error decrypting password." << std::endl;
        return false;
    }

    // Convert the decrypted buffer to a string
    decryptedPassword.assign(reinterpret_cast<char*>(decryptedBuffer), ciphertext.size());

    return true;
}

bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword,
                     const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey) {
    // Check if libsodium has been properly initialized
    if (sodium_init() < 0) {
        std::cerr << "Error initializing libsodium." << std::endl;
        return false;
    }

    // Generate a random nonce
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> nonce;
    randombytes_buf(nonce.data(), nonce.size());

    // Initialize a buffer for the ciphertext
    unsigned char encryptedBuffer[decryptedPassword.size()];

    // Encrypt the password
    if (crypto_secretbox_easy(encryptedBuffer, reinterpret_cast<const unsigned char*>(decryptedPassword.c_str()), decryptedPassword.size(),
                              nonce.data(), encryptionKey.data()) != 0) {
        std::cerr << "Error encrypting password." << std::endl;
        return false;
    }

    // Combine the nonce and ciphertext into the encrypted password
    encryptedPassword = std::string(reinterpret_cast<char*>(nonce.data()), nonce.size()) +
                        std::string(reinterpret_cast<char*>(encryptedBuffer), decryptedPassword.size());

    return true;
}

// Initialize the encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce) {
    // Generate a random encryption key
    randombytes(encryptionKey.data(), crypto_secretbox_KEYBYTES);

    // Generate a random nonce
    randombytes(encryptionNonce.data(), crypto_secretbox_NONCEBYTES);
}