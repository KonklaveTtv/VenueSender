#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <array>
#include <iostream>

#include <sodium.h>

extern std::array<unsigned char, crypto_secretbox_KEYBYTES> globalEncryptionKey;
extern std::array<unsigned char, crypto_secretbox_NONCEBYTES> globalEncryptionNonce;

class EncryptionManager {
public:
    EncryptionManager(); // Constructor will initialize encryption key and nonce

    // Utility function to encrypt the email and SMTP passwords
    static bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword);

    // Utility function to decrypt the email and SMTP passwords
    static std::string decryptPassword(const std::string& encryptedPassword);
};

#endif // ENCRYPTION_H
