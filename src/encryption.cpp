#include "encryption.h"

// Initialize the encryption key and nonce
void initializeEncryptionParams(std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey,
                                 std::array<unsigned char, crypto_secretbox_NONCEBYTES>& encryptionNonce) {
    // Generate a random encryption key
    randombytes(encryptionKey.data(), crypto_secretbox_KEYBYTES);

    // Generate a random nonce
    randombytes(encryptionNonce.data(), crypto_secretbox_NONCEBYTES);
}

void generateIdentifier(const unsigned char* encryptionKey, const char* nonce,
                        std::array<unsigned char, crypto_secretbox_MACBYTES>& identifier) {
    // Combine the encryptionKey and nonce
    std::string keyNonceData(reinterpret_cast<const char*>(encryptionKey), crypto_secretbox_KEYBYTES);
    keyNonceData += nonce;

    crypto_generichash(identifier.data(), identifier.size(),
                        reinterpret_cast<const unsigned char*>(keyNonceData.c_str()), keyNonceData.size(),
                        nullptr, 0);
}

bool decryptPassword(const std::string& encryptedPassword, std::string& decryptedPassword,
                      const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey) {
    // Check if libsodium has been properly initialized
    if (sodium_init() < 0) {
        std::cerr << "Error initializing libsodium." << std::endl;
        return false;
    }

    // Define the encryption nonce length
    const size_t NONCE_LENGTH = crypto_secretbox_NONCEBYTES;
    const size_t IDENTIFIER_LENGTH = crypto_secretbox_MACBYTES;

    // Check if the encrypted password is long enough to contain the nonce
    if (encryptedPassword.size() < NONCE_LENGTH + IDENTIFIER_LENGTH) {
        std::cerr << "Invalid encrypted password format." << std::endl;
        return false;
    }

    // Extract the encryption nonce from the encrypted password
    std::string nonce = encryptedPassword.substr(0, NONCE_LENGTH);

    // Extract the ciphertext from the encrypted password
    std::string ciphertext = encryptedPassword.substr(NONCE_LENGTH);

    // Initialize a buffer for the decrypted password
    unsigned char decryptedBuffer[ciphertext.size()];

    // Decrypt the ciphertext
    if (crypto_secretbox_open_easy(decryptedBuffer, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size(),
                                   reinterpret_cast<const unsigned char*>(nonce.c_str()), encryptionKey.data()) != 0) {
        std::cerr << "Error decrypting password." << std::endl;
        return false;
    }

    // Extract the identifier from the decrypted data
    std::array<unsigned char, IDENTIFIER_LENGTH> extractedIdentifier;
    std::copy(decryptedBuffer, decryptedBuffer + IDENTIFIER_LENGTH, extractedIdentifier.data());

    // Create expected identifier using key and nonce.
    std::array<unsigned char, IDENTIFIER_LENGTH> expectedIdentifier;
    generateIdentifier(encryptionKey.data(), nonce.c_str(), expectedIdentifier);

    // Check if the identifiers match
    if (crypto_verify_16(extractedIdentifier.data(), expectedIdentifier.data()) != 0) {
        std::cerr << "Incorrect password identifier, set 'smtp_pass_encrypted' & 'email_pass_encrypted' to false and re-enter both passwords (config.json)." << std::endl;
        return false;
    }

    // Decrypt the actual password data
    decryptedPassword.assign(reinterpret_cast<char*>(decryptedBuffer) + IDENTIFIER_LENGTH, ciphertext.size() - IDENTIFIER_LENGTH);

    return true;
}


bool encryptPassword(const std::string& decryptedPassword, std::string& encryptedPassword,
                     const std::array<unsigned char, crypto_secretbox_KEYBYTES>& encryptionKey) {
    // Check if libsodium has been properly initialized
    if (sodium_init() < 0) {
        std::cerr << "Error initializing libsodium." << std::endl;
        return false;
    }

    // Generate a random nonce
    std::array<unsigned char, crypto_secretbox_NONCEBYTES> nonce;
    randombytes_buf(nonce.data(), nonce.size());

    // Generate a random identifier
    std::array<unsigned char, crypto_secretbox_MACBYTES> identifier;
    randombytes(identifier.data(), identifier.size());

    std::string dataToEncrypt(reinterpret_cast<char*>(identifier.data()), identifier.size());
    dataToEncrypt += decryptedPassword;

    // Initialize a buffer for the ciphertext
    unsigned char encryptedBuffer[dataToEncrypt.size()];

    // Encrypt the password
    if (crypto_secretbox_easy(encryptedBuffer, reinterpret_cast<const unsigned char*>(decryptedPassword.c_str()), decryptedPassword.size(),
                              nonce.data(), encryptionKey.data()) != 0) {
        std::cerr << "Error encrypting password." << std::endl;
        return false;
    }

    // Combine the nonce and ciphertext into the encrypted password
    encryptedPassword = std::string(reinterpret_cast<char*>(nonce.data()), nonce.size()) +
                        std::string(reinterpret_cast<char*>(encryptedBuffer), decryptedPassword.size());

    return true;
}