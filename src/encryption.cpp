#include "encryption.h"

// Define global variables to store the encryption key and nonce
std::array<unsigned char, crypto_secretbox_KEYBYTES> globalEncryptionKey;
std::array<unsigned char, crypto_secretbox_NONCEBYTES> globalEncryptionNonce;

// Initialize the encryption key and nonce
void initializeEncryptionParams() {

    // Initialize libsodium
    if (sodium_init() < 0) {
        std::cerr << "Error initializing libsodium." << std::endl;
        // Handle the error appropriately
    }

    // Generate a random encryption key
    randombytes_buf(globalEncryptionKey.data(), crypto_secretbox_KEYBYTES);

    // Generate a random nonce
    randombytes_buf(globalEncryptionNonce.data(), crypto_secretbox_NONCEBYTES);
}

bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword) {
    // Initialize a buffer for the ciphertext
    unsigned char encryptedBuffer[decryptedPassword.size()];

    // Encrypt the password using the global encryption key and nonce
    if (crypto_secretbox_easy(encryptedBuffer, reinterpret_cast<const unsigned char*>(decryptedPassword.c_str()), decryptedPassword.size(),
                              globalEncryptionNonce.data(), globalEncryptionKey.data()) != 0) {
        std::cerr << "Error encrypting password." << std::endl;
        return false;
    }

    // Combine the nonce and ciphertext into the encrypted password
    encryptedPassword = std::string(reinterpret_cast<const char*>(globalEncryptionNonce.data()), globalEncryptionNonce.size()) +
                        std::string(reinterpret_cast<const char*>(encryptedBuffer), decryptedPassword.size());
    return true;
}

std::string decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword) {
    // Define the encryption nonce length
    const size_t NONCE_LENGTH = crypto_secretbox_NONCEBYTES;

    // Check if the encrypted password is long enough to contain the nonce
    if (encryptedPassword.size() < NONCE_LENGTH) {
        std::cerr << "Invalid encrypted password format." << std::endl;
        return "";
    }

    // Extract the encryption nonce from the encrypted password
    std::string nonce = encryptedPassword.substr(0, NONCE_LENGTH);

    // Extract the ciphertext from the encrypted password
    std::string ciphertext = encryptedPassword.substr(NONCE_LENGTH);

    // Initialize a buffer for the decrypted password
    unsigned char decryptedBuffer[ciphertext.size()];

    // Decrypt the ciphertext using the global encryption key and extracted nonce
    if (crypto_secretbox_open_easy(decryptedBuffer, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size(),
                                   reinterpret_cast<const unsigned char*>(nonce.c_str()), globalEncryptionKey.data()) != 0) {
        std::cerr << "Error decrypting password." << std::endl;
        return "";
    }

    // Return the decrypted password
    decryptedPassword.assign(reinterpret_cast<char*>(decryptedBuffer), ciphertext.size());

    // Return the decrypted password
    return decryptedPassword;
}
