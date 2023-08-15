#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <array>
#include <iostream>
#include <sodium.h>

using namespace std;

// Extern global variables to store the encryption key and nonce
extern array<unsigned char, crypto_secretbox_KEYBYTES> globalEncryptionKey;
extern array<unsigned char, crypto_secretbox_NONCEBYTES> globalEncryptionNonce;

// Function to initialize encryption key and nonce
void initializeEncryptionParams();

// Utility function to encrypt the email and smtp passwords
bool encryptPassword(const string& decryptedPassword, string& encryptedPassword);

// Utility function to decrypt the email and smtp passwords
string decryptPassword(const string& encryptedPassword);

#endif // ENCRYPTION_H
