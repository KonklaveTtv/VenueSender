#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <array>
#include <iostream>
#include <sodium.h>

// Extern global variables to store the encryption key and nonce
extern std::array<unsigned char, crypto_secretbox_KEYBYTES> globalEncryptionKey;
extern std::array<unsigned char, crypto_secretbox_NONCEBYTES> globalEncryptionNonce;

// Function to initialize encryption key and nonce
void initializeEncryptionParams();

// Utility function to encrypt the email and smtp passwords
bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword);

// Utility function to decrypt the email and smtp passwords
std::string decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword);

#endif // ENCRYPTION_H
