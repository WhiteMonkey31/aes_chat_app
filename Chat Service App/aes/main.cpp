#include "s_box.hpp"
#include <iostream>

int main() {
    std::string key = "secretkey123456";
    std::string msg = "Hello AES Chat App";

    std::cout << "Enter message: ";
    std::getline(std::cin, msg);

    std::string cipher = aesEncryptString(msg, key);
    std::cout << "Encrypted: " << cipher << "\n";

    std::string plain = aesDecryptString(cipher, key);
    std::cout << "Decrypted: " << plain << "\n";
}
