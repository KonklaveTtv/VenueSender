#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <array>
#include <iostream>
#include <sodium.h>

class EncryptionManager {
private:
    std::array<unsigned char, crypto_secretbox_KEYBYTES> encryptionKey;
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> encryptionNonce;

public:
    EncryptionManager(); // Constructor will initialize encryption key and nonce

    // Utility function to encrypt the email and SMTP passwords
    bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword);

    // Utility function to decrypt the email and SMTP passwords
    std::string decryptPassword(const std::string& encryptedPassword);
};

#endif // ENCRYPTION_H
