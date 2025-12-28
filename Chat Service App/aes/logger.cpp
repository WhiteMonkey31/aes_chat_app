#include "logger.hpp"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

Logger* g_logger = nullptr;

Logger::~Logger() {
    if (ofs.is_open()) ofs.close();
}

std::string Logger::makeFilename(int msgNo, const std::string& operation) {
    std::filesystem::create_directories("logs");
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    ss << "_" << operation;
    ss << "_msg" << msgNo << ".txt";
    return std::string("logs/") + ss.str();
}

void Logger::start(const std::string& key, const std::string& message, int msgNo, const std::string& operation) {
    if (ofs.is_open()) ofs.close();
    std::string fname = makeFilename(msgNo, operation);
    ofs.open(fname, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) return;
    ofs << "Operation: " << operation << "\n";
    ofs << "Original Message: " << message << "\n";
    // print hex and string form of key
    ofs << "Key Used (hex): ";
    for (unsigned char c : key) ofs << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
    ofs << "\n";
    ofs << "Key Used (str): " << key << "\n\n";
}

void Logger::log(const std::string& line) {
    if (!ofs.is_open()) return;
    ofs << line << "\n";
}

void Logger::logBytesHex(const std::string& label, const uint8_t* data, size_t len) {
    if (!ofs.is_open()) return;
    ofs << label << ": ";
    for (size_t i = 0; i < len; ++i) {
        ofs << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
    }
    ofs << "\n";
}

void Logger::logState(const std::string& label, uint8_t state[4][4]) {
    if (!ofs.is_open()) return;
    ofs << label << "\n";
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            ofs << std::hex << std::setw(2) << std::setfill('0') << (int)state[row][col] << " ";
        }
        ofs << "\n";
    }
    ofs << "\n";
}

void Logger::logRoundKey(int round, const uint8_t* roundKey) {
    if (!ofs.is_open()) return;
    ofs << "RoundKey " << round << ": ";
    for (int i = 0; i < 16; ++i) {
        ofs << std::hex << std::setw(2) << std::setfill('0') << (int)roundKey[i] << " ";
    }
    ofs << "\n";
}
