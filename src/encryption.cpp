#include "encryption.h"

// Initialize the encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce) {

    // Initialize libsodium
    if (sodium_init() < 0) {
        std::cerr << "Error initializing libsodium." << std::endl;
        // Handle the error appropriately
    }

    // Generate a random encryption key
    randombytes_buf(encryptionKey.data(), crypto_secretbox_KEYBYTES);

    // Generate a random nonce
    randombytes_buf(encryptionNonce.data(), crypto_secretbox_NONCEBYTES);
}
bool decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword,
                      const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                      const std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce) {

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
                                   encryptionNonce.data(), encryptionKey.data()) != 0) {
        std::cerr << "Error decrypting password." << std::endl;
        return false;
    }

    // Store the decrypted password data
    decryptedPassword.assign(reinterpret_cast<char*>(decryptedBuffer), ciphertext.size());

    return true;
}

bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword,
                     const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                     const std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce) {

    // Initialize a buffer for the ciphertext
    unsigned char encryptedBuffer[decryptedPassword.size()];

    // Encrypt the password
    if (crypto_secretbox_easy(encryptedBuffer, reinterpret_cast<const unsigned char*>(decryptedPassword.c_str()), decryptedPassword.size(),
                              encryptionNonce.data(), encryptionKey.data()) != 0) {
        std::cerr << "Error encrypting password." << std::endl;
        return false;
    }

    // Combine the nonce and ciphertext into the encrypted password
    encryptedPassword = std::string(reinterpret_cast<const char*>(encryptionNonce.data()), encryptionNonce.size()) +
                        std::string(reinterpret_cast<const char*>(encryptedBuffer), decryptedPassword.size());

    return true;
}