#pragma once
#include <string>

// Demo pre-shared key (32 bytes). Replace with secure key management for production.
static inline std::string GetPreSharedKey() {
    // 32 ASCII characters = 32 bytes
    static const char key[33] = "0123456789abcdef0123456789abcdef"; // 32 bytes
    return std::string(key, 32);
}
