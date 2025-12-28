#ifndef AES_API_HPP
#define AES_API_HPP

#include <string>

// High-level AES string encrypt/decrypt provided by the aes submodule
// These functions are defined in encrypt.cpp / decrypt.cpp
std::string aesEncryptString(const std::string& plaintext, const std::string& keyStr);
std::string aesDecryptString(const std::string& hexCipher, const std::string& keyStr);

#endif // AES_API_HPP
