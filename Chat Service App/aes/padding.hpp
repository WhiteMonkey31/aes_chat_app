#ifndef PADDING_HPP
#define PADDING_HPP

#include <string>
#include <vector>
#include <stdint.h>
#include <stdexcept>

// Forward declarations
void aesEncryptBlock(uint8_t* block, const uint8_t* roundKeys);
void aesDecryptBlock(uint8_t* block, const uint8_t* roundKeys);
void keyExpansion(const uint8_t* key, uint8_t* roundKeys);

// Function declarations
std::vector<uint8_t> pkcs7Pad(const std::vector<uint8_t>& data);
void pkcs7Unpad(std::vector<uint8_t>& data);
std::vector<uint8_t> stringToBytes(const std::string& s);
std::string bytesToString(const std::vector<uint8_t>& bytes);
std::string bytesToHex(const std::vector<uint8_t>& data);
uint8_t hexVal(char c);
std::vector<uint8_t> hexToBytes(const std::string& hex);
std::vector<uint8_t> normalizeKey(const std::string& key);
std::string aesEncryptString(const std::string& plaintext, const std::string& keyStr);
std::string aesDecryptString(const std::string& hexCipher, const std::string& keyStr);

#endif // PADDING_HPP
