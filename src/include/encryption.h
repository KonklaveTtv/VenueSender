#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <array>
#include <iostream>
#include <sodium.h>

// Function to initialize encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce);

// Utility function to decrypt the email and smtp passwords
bool decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword,
                      const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                      const std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce);

// Utility function to encrypt the email and smtp passwords
bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword,
                     const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                     const std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce);

#endif // ENCRYPTION_H
