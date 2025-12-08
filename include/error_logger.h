#ifndef ERROR_LOGGER_H
#define ERROR_LOGGER_H

#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace option_pricer {

// 简单的错误日志记录器
class ErrorLogger {
public:
    enum class Level {
        INFO,
        WARNING,
        ERROR
    };
    
    // 记录日志消息
    static void log(Level level, const std::string& message, 
                   const std::string& context = "");
    
    // 便捷方法
    static void info(const std::string& message, const std::string& context = "");
    static void warning(const std::string& message, const std::string& context = "");
    static void error(const std::string& message, const std::string& context = "");
    
private:
    static std::string get_timestamp();
    static std::string level_to_string(Level level);
};

} // namespace option_pricer

#endif // ERROR_LOGGER_H
