#include "include/encryption.h"

// Use the standard namespace
using namespace std;

// Define global variables to store the encryption key and nonce
array<unsigned char, crypto_secretbox_KEYBYTES> globalEncryptionKey;
array<unsigned char, crypto_secretbox_NONCEBYTES> globalEncryptionNonce;

// Default constructor for EncryptionManager
EncryptionManager::EncryptionManager() {
    // Initialize libsodium library for encryption
    if (sodium_init() < 0) {
        // Handle initialization error
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::LIBSODIUM_INIT_ERROR);
        exit(EXIT_FAILURE); // Terminate the program
    }

        // Generate a random encryption key
        randombytes_buf(globalEncryptionKey.data(), crypto_secretbox_KEYBYTES);

        // Generate a random nonce (Number Once used in cryptographic communication)
        randombytes_buf(globalEncryptionNonce.data(), crypto_secretbox_NONCEBYTES);
}

// Function to encrypt a password
bool EncryptionManager::encryptPassword(const string& decryptedPassword, string& encryptedPassword) {
        // Initialize a buffer to hold the encrypted password
        unsigned char encryptedBuffer[crypto_secretbox_MACBYTES + decryptedPassword.size()];

        // Perform the encryption using the global key and nonce
        if (crypto_secretbox_easy(encryptedBuffer, reinterpret_cast<const unsigned char*>(decryptedPassword.c_str()), decryptedPassword.size(),
                              globalEncryptionNonce.data(), globalEncryptionKey.data()) != 0) {
        // Handle encryption error    
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_ENCRYPTION_ERROR);
        return false;
    }

    // Convert the nonce to a string for storage
    string nonceStr(reinterpret_cast<const char*>(globalEncryptionNonce.data()), globalEncryptionNonce.size());

    // Combine the nonce and encrypted password into one string
    encryptedPassword = nonceStr + string(reinterpret_cast<const char*>(encryptedBuffer), sizeof(encryptedBuffer));
    
    return true;
}

// Function to decrypt a password
string EncryptionManager::decryptPassword(const string& encryptedPassword) {
    // Define the length of the nonce
    const size_t NONCE_LENGTH = crypto_secretbox_NONCEBYTES;

        // Check if the encrypted password is long enough to contain both the nonce and MAC
        if (encryptedPassword.size() < crypto_secretbox_MACBYTES + NONCE_LENGTH) {
        // Handle format error
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_ENCRYPTION_FORMAT_ERROR);
        exit(EXIT_FAILURE); // Terminate the program
    }

    // Extract the nonce from the encrypted password
    string nonce = encryptedPassword.substr(0, NONCE_LENGTH);

    // Extract the actual encrypted password data
    string ciphertext = encryptedPassword.substr(NONCE_LENGTH);

    // Initialize a buffer to hold the decrypted password
    unsigned char decryptedBuffer[ciphertext.size() - crypto_secretbox_MACBYTES];

    // Perform the decryption
    if (crypto_secretbox_open_easy(decryptedBuffer, 
                                   reinterpret_cast<const unsigned char*>(ciphertext.c_str()), 
                                   ciphertext.size(),
                                   reinterpret_cast<const unsigned char*>(nonce.c_str()), 
                                   globalEncryptionKey.data()) != 0) {
        // Handle decryption error
        ErrorHandler::handleErrorAndReturn(ErrorHandler::ErrorType::EMAIL_PASSWORD_DECRYPTION_ERROR);
        exit(EXIT_FAILURE); // Terminate the program if decryption fails
    }

    // Return the decrypted password as a string
    return string(reinterpret_cast<char*>(decryptedBuffer), ciphertext.size() - crypto_secretbox_MACBYTES);
}
