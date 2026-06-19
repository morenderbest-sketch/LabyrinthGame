#pragma once
#include "Common.h"

class Logger {
public:
    static Logger& instance();
    void init(const std::string& filename);
    void log(const std::string& message);
    void close();

private:
    Logger() = default;
    ~Logger();
    std::ofstream logFile_;
    std::string getTimestamp() const;
};
