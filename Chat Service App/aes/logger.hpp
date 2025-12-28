#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <cstdint>

class Logger {
public:
    Logger() = default;
    ~Logger();

    void start(const std::string& key, const std::string& message, int msgNo, const std::string& operation);
    void log(const std::string& line);
    void logBytesHex(const std::string& label, const uint8_t* data, size_t len);
    void logState(const std::string& label, uint8_t state[4][4]);
    void logRoundKey(int round, const uint8_t* roundKey);

private:
    std::ofstream ofs;
    std::string makeFilename(int msgNo, const std::string& operation);
};

extern Logger* g_logger;

#endif // LOGGER_HPP
