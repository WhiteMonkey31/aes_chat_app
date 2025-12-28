#include "Crypto.h"
#include "../aes/aes_api.hpp"
#include <random>

bool Crypto::init() {
    // AES implementation in aes/ does not require a library init step
    return true;
}

std::string Crypto::encrypt(const std::string& plaintext, const std::string& key) {
    // aesEncryptString returns hex-encoded ciphertext
    return aesEncryptString(plaintext, key);
}

std::string Crypto::decrypt(const std::string& ciphertext, const std::string& key) {
    return aesDecryptString(ciphertext, key);
}

std::string Crypto::generateKey() {
    // Generate a 16-byte random key (AES-128) as printable characters
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    std::string key;
    key.reserve(16);
    for (int i = 0; i < 16; ++i) key.push_back(static_cast<char>(dist(gen)));
    return key;
}
