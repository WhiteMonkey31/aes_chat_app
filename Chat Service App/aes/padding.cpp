#include "s_box.hpp"
#include "logger.hpp"

static int g_message_counter = 0;

std::vector<uint8_t> pkcs7Pad(const std::vector<uint8_t>& data) {
    size_t padLen = 16 - (data.size() % 16);
    std::vector<uint8_t> padded = data;
    for (size_t i = 0; i < padLen; i++) {
        padded.push_back(static_cast<uint8_t>(padLen));
    }
    return padded;
}

void pkcs7Unpad(std::vector<uint8_t>& data) {
    if (data.empty()) throw std::runtime_error("Invalid padding");

    uint8_t pad = data.back();
    if (pad == 0 || pad > 16) throw std::runtime_error("Invalid padding");

    for (uint8_t i = 0; i < pad; i++) {
        if (data[data.size() - 1 - static_cast<size_t>(i)] != pad)
            throw std::runtime_error("Invalid padding");
    }

    data.resize(data.size() - pad);
}

std::vector<uint8_t> stringToBytes(const std::string& s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}

std::string bytesToString(const std::vector<uint8_t>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

std::string bytesToHex(const std::vector<uint8_t>& data) {
    static const char* hex = "0123456789abcdef";
    std::string out;
    for (uint8_t b : data) {
        out.push_back(hex[b >> 4]);
        out.push_back(hex[b & 0x0F]);
    }
    return out;
}

uint8_t hexVal(char c) {
    if ('0' <= c && c <= '9') return static_cast<uint8_t>(c - '0');
    if ('a' <= c && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
    if ('A' <= c && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
    throw std::runtime_error("Invalid hex");
}

std::vector<uint8_t> hexToBytes(const std::string& hex) {
    if (hex.size() % 2 != 0)
        throw std::runtime_error("Invalid hex length");

    std::vector<uint8_t> data;
    for (size_t i = 0; i < hex.size(); i += 2) {
        data.push_back((hexVal(hex[i]) << 4) | hexVal(hex[i + 1]));
    }
    return data;
}

std::vector<uint8_t> normalizeKey(const std::string& key) {
    std::vector<uint8_t> k(16, 0x00);
    for (size_t i = 0; i < key.size() && i < 16; i++) {
        k[i] = static_cast<uint8_t>(key[i]);
    }
    return k;
}

std::string aesEncryptString(const std::string& plaintext,
                            const std::string& keyStr) {
    auto key = normalizeKey(keyStr);
    uint8_t roundKeys[176];
    // prepare logging
    Logger logger;
    ++g_message_counter;
    logger.start(keyStr, plaintext, g_message_counter, "encrypt");
    g_logger = &logger;

    keyExpansion(key.data(), roundKeys);

    // log round keys
    if (g_logger) {
        for (int r = 0; r < 11; ++r) {
            g_logger->logRoundKey(r, roundKeys + r * 16);
        }
    }

    auto data = pkcs7Pad(stringToBytes(plaintext));

    for (size_t i = 0; i < data.size(); i += 16) {
        if (g_logger) g_logger->logBytesHex("Block input", &data[i], 16);
        aesEncryptBlock(&data[i], roundKeys);
        if (g_logger) g_logger->logBytesHex("Block output", &data[i], 16);
    }

    // log final cipher hex
    if (g_logger) g_logger->log(std::string("Cipher (hex): ") + bytesToHex(data));
    // finish logging
    g_logger = nullptr;

    return bytesToHex(data);
}

std::string aesDecryptString(const std::string& hexCipher,
                            const std::string& keyStr) {
    auto key = normalizeKey(keyStr);
    uint8_t roundKeys[176];
    // prepare logging for decryption
    Logger logger;
    ++g_message_counter;
    logger.start(keyStr, hexCipher, g_message_counter, "decrypt");
    g_logger = &logger;

    keyExpansion(key.data(), roundKeys);

    // log round keys
    if (g_logger) {
        for (int r = 0; r < 11; ++r) {
            g_logger->logRoundKey(r, roundKeys + r * 16);
        }
    }

    auto data = hexToBytes(hexCipher);

    for (size_t i = 0; i < data.size(); i += 16) {
        if (g_logger) g_logger->logBytesHex("Block input", &data[i], 16);
        aesDecryptBlock(&data[i], roundKeys);
        if (g_logger) g_logger->logBytesHex("Block output", &data[i], 16);
    }

    // log cipher (hex) as provided
    if (g_logger) g_logger->log(std::string("Cipher (hex): ") + hexCipher);

    pkcs7Unpad(data);
    // log final plaintext
    if (g_logger) g_logger->log(std::string("Plaintext: ") + bytesToString(data));
    // finish logging
    g_logger = nullptr;

    return bytesToString(data);
}
