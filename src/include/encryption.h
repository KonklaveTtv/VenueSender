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
    static EncryptionManager& getInstance();
    EncryptionManager(EncryptionManager const&) = delete;
    void operator=(EncryptionManager const&) = delete;

    // Declare constructor
    EncryptionManager();

    // Your existing methods here
    static bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword);
    static std::string decryptPassword(const std::string& encryptedPassword);
};

#endif // ENCRYPTION_H
