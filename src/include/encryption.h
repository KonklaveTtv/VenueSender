#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include "errorhandler.h"

#include <array>
#include <iostream>

// Include libsodium for encryption/decryption functionalities
#include <sodium.h>

// Declare global encryption key and nonce arrays
extern std::array<unsigned char, crypto_secretbox_KEYBYTES> globalEncryptionKey;
extern std::array<unsigned char, crypto_secretbox_NONCEBYTES> globalEncryptionNonce;

// EncryptionManager class to handle encryption-related functionalities
class EncryptionManager {
public:
    // Constructor will initialize encryption key and nonce
    EncryptionManager();

    // Utility function to encrypt a password
    // Parameters: 
    // decryptedPassword: The original password to encrypt
    // encryptedPassword: The encrypted password will be stored here
    // Return: true if encryption is successful, false otherwise
    static bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword);

    // Utility function to decrypt a password
    // Parameter: encryptedPassword: The encrypted password to decrypt
    // Return: The decrypted password as a string
    static std::string decryptPassword(const std::string& encryptedPassword);
};

#endif // ENCRYPTION_H
