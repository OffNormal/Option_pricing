#include "error_logger.h"

namespace option_pricer {

void ErrorLogger::log(Level level, const std::string& message, 
                     const std::string& context) {
    std::cerr << "[" << get_timestamp() << "] "
              << "[" << level_to_string(level) << "] ";
    
    if (!context.empty()) {
        std::cerr << "[" << context << "] ";
    }
    
    std::cerr << message << std::endl;
}

void ErrorLogger::info(const std::string& message, const std::string& context) {
    log(Level::INFO, message, context);
}

void ErrorLogger::warning(const std::string& message, const std::string& context) {
    log(Level::WARNING, message, context);
}

void ErrorLogger::error(const std::string& message, const std::string& context) {
    log(Level::ERROR, message, context);
}

std::string ErrorLogger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string ErrorLogger::level_to_string(Level level) {
    switch (level) {
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR:   return "ERROR";
        default:             return "UNKNOWN";
    }
}

} // namespace option_pricer
