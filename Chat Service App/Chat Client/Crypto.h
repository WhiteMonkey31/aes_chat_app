#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>

class Crypto {
public:
    // Initialize crypto subsystem if needed (noop for AES wrapper)
    static bool init();
    
    // Encrypt plaintext and return ciphertext (implementation uses aes submodule)
    static std::string encrypt(const std::string& plaintext, const std::string& key);
    
    // Decrypt ciphertext and return plaintext
    static std::string decrypt(const std::string& ciphertext, const std::string& key);
    
    // Generate a symmetric key (16-byte string for AES-128)
    static std::string generateKey();
};

#endif // CRYPTO_H
