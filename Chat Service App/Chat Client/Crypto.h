#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <vector>
#include <sodium.h>

class Crypto {
public:
    // Initialize crypto library (call once at startup)
    static bool init();
    
    // Encrypt data using ChaCha20-Poly1305 AEAD
    // Returns: ciphertext + nonce + tag (concatenated)
    static std::string encrypt(const std::string& plaintext, const std::string& key);
    
    // Decrypt data using ChaCha20-Poly1305 AEAD
    // Input: ciphertext + nonce + tag (as returned by encrypt)
    static std::string decrypt(const std::string& ciphertext_with_nonce_tag, const std::string& key);
    
    // Generate a shared key (for PSK mode)
    static std::string generateKey();

private:
    // Nonce size for ChaCha20-Poly1305 (IETF variant)
    static constexpr size_t NONCE_SIZE = crypto_aead_chacha20poly1305_ietf_NPUBBYTES;
    // Key size
    static constexpr size_t KEY_SIZE = crypto_aead_chacha20poly1305_KEYBYTES;
    // AEAD tag size (IETF variant)
    static constexpr size_t TAG_SIZE = crypto_aead_chacha20poly1305_ietf_ABYTES;
};

#endif // CRYPTO_H
