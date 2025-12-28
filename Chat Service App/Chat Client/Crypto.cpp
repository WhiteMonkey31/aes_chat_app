#include "Crypto.h"
#include <stdexcept>
#include <cstring>

bool Crypto::init() {
    return sodium_init() == 0;
}

std::string Crypto::encrypt(const std::string& plaintext, const std::string& key) {
    if (key.size() != KEY_SIZE) {
        throw std::runtime_error("Invalid key size for encryption");
    }

    // Generate random nonce for each encryption
    std::vector<unsigned char> nonce(NONCE_SIZE);
    randombytes_buf(nonce.data(), NONCE_SIZE);

    // Allocate ciphertext buffer (plaintext + tag)
    std::vector<unsigned char> ciphertext(plaintext.size() + TAG_SIZE);
    unsigned long long ciphertext_len = 0;

    // Encrypt using IETF API: npub = nonce, nsec = NULL
    if (crypto_aead_chacha20poly1305_ietf_encrypt(
            ciphertext.data(), &ciphertext_len,
            reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size(),
            nullptr, 0, // additional data
            nullptr,     // nsec (not used)
            nonce.data(),
            reinterpret_cast<const unsigned char*>(key.data())) != 0) {
        throw std::runtime_error("Encryption failed");
    }

    // Concatenate: nonce + ciphertext
    std::string result;
    result.append(reinterpret_cast<const char*>(nonce.data()), NONCE_SIZE);
    result.append(reinterpret_cast<const char*>(ciphertext.data()), static_cast<size_t>(ciphertext_len));
    return result;
}

std::string Crypto::decrypt(const std::string& ciphertext_with_nonce_tag, const std::string& key) {
    if (key.size() != KEY_SIZE) {
        throw std::runtime_error("Invalid key size for decryption");
    }

    if (ciphertext_with_nonce_tag.size() < NONCE_SIZE + TAG_SIZE) {
        throw std::runtime_error("Ciphertext too short");
    }

    const unsigned char* nonce = reinterpret_cast<const unsigned char*>(ciphertext_with_nonce_tag.data());
    const unsigned char* ciphertext = nonce + NONCE_SIZE;
    size_t ciphertext_len = ciphertext_with_nonce_tag.size() - NONCE_SIZE;

    std::vector<unsigned char> plaintext(ciphertext_len);
    unsigned long long plaintext_len = 0;

    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            plaintext.data(), &plaintext_len,
            nullptr,
            ciphertext, ciphertext_len,
            nullptr, 0,
            nonce,
            reinterpret_cast<const unsigned char*>(key.data())) != 0) {
        throw std::runtime_error("Decryption failed - authentication tag invalid");
    }

    return std::string(reinterpret_cast<const char*>(plaintext.data()), static_cast<size_t>(plaintext_len));
}

std::string Crypto::generateKey() {
    std::vector<unsigned char> key(KEY_SIZE);
    randombytes_buf(key.data(), KEY_SIZE);
    return std::string(reinterpret_cast<const char*>(key.data()), KEY_SIZE);
}
