#include "include/encryption.h"

using namespace std;

// Define global variables to store the encryption key and nonce
array<unsigned char, crypto_secretbox_KEYBYTES> globalEncryptionKey;
array<unsigned char, crypto_secretbox_NONCEBYTES> globalEncryptionNonce;

EncryptionManager::EncryptionManager() {
    // Initialize libsodium
    if (sodium_init() < 0) {
        throw runtime_error("Failed to initialize libsodium.");
    }

    // Generate a random encryption key
    randombytes_buf(globalEncryptionKey.data(), crypto_secretbox_KEYBYTES);

    // Generate a random nonce
    randombytes_buf(globalEncryptionNonce.data(), crypto_secretbox_NONCEBYTES);
}

bool EncryptionManager::encryptPassword(const string& decryptedPassword, string& encryptedPassword) {
    // Initialize a buffer for the ciphertext
    unsigned char encryptedBuffer[crypto_secretbox_MACBYTES + decryptedPassword.size()];

    // Encrypt the password using the global encryption key and nonce
    if (crypto_secretbox_easy(encryptedBuffer, reinterpret_cast<const unsigned char*>(decryptedPassword.c_str()), decryptedPassword.size(),
                              globalEncryptionNonce.data(), globalEncryptionKey.data()) != 0) {
        cerr << "Failed to encrypt password." << endl;
        return false;
    }

    // Convert the nonce to a string
    string nonceStr(reinterpret_cast<const char*>(globalEncryptionNonce.data()), globalEncryptionNonce.size());

    // Combine the nonce and ciphertext into the encrypted password
    encryptedPassword = nonceStr + string(reinterpret_cast<const char*>(encryptedBuffer), sizeof(encryptedBuffer));
    
    return true;
}

string EncryptionManager::decryptPassword(const string& encryptedPassword) {
    // Define the encryption nonce length
    const size_t NONCE_LENGTH = crypto_secretbox_NONCEBYTES;

    // Check if the encrypted password is long enough to contain the nonce and MAC
    if (encryptedPassword.size() < crypto_secretbox_MACBYTES + NONCE_LENGTH) {
        throw invalid_argument("Invalid encrypted password format.");
    }

    // Extract the encryption nonce from the encrypted password
    string nonce = encryptedPassword.substr(0, NONCE_LENGTH);

    // Extract the ciphertext from the encrypted password
    string ciphertext = encryptedPassword.substr(NONCE_LENGTH);

    // Initialize a buffer for the decrypted password
    unsigned char decryptedBuffer[ciphertext.size() - crypto_secretbox_MACBYTES];

    // Decrypt the ciphertext using the global encryption key and extracted nonce
    if (crypto_secretbox_open_easy(decryptedBuffer, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size(),
                                   reinterpret_cast<const unsigned char*>(nonce.c_str()), globalEncryptionKey.data()) != 0) {
        throw runtime_error("Failed to decrypt password or corrupted password.");
    }

    // Return the decrypted password
    return string(reinterpret_cast<char*>(decryptedBuffer), ciphertext.size() - crypto_secretbox_MACBYTES);
}
