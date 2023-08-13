#include <array>
#include <iostream>
#include <sodium.h>

// Function to initialize encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce);

// Utility function to decrypt the email and smtp passwords
bool decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword,
                      const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey);

// Utility function to encrypt the email and smtp passwords
bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword,
                     const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey);

//Utility function to generate a unique identifer
void generateIdentifier(const unsigned char* encryptionKey, const char* nonce,
                        std::array<unsigned char, crypto_secretbox_MACBYTES>& identifier);
