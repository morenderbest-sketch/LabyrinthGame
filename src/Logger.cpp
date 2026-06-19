#include "Logger.h"

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::init(const std::string& filename) {
    logFile_.open(filename, std::ios::app);
    log("=== Game session started ===");
}

void Logger::log(const std::string& message) {
    if (logFile_.is_open()) {
        logFile_ << "[" << getTimestamp() << "] " << message << "\n";
        logFile_.flush();
    }
}

void Logger::close() {
    if (logFile_.is_open()) {
        log("=== Game session ended ===");
        logFile_.close();
    }
}

Logger::~Logger() {
    close();
}

std::string Logger::getTimestamp() const {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
