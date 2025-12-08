#include "input_validator.h"
#include <cmath>
#include <limits>

namespace option_pricer {

// 验证期权参数
InputValidator::ValidationResult InputValidator::validate(const OptionParams& params) {
    ValidationResult result;
    result.is_valid = true;
    
    // 验证标的资产价格为正数
    if (!is_positive(params.spot_price)) {
        result.is_valid = false;
        result.errors.push_back("Spot price must be positive");
    }
    
    // 验证执行价格为正数
    if (!is_positive(params.strike_price)) {
        result.is_valid = false;
        result.errors.push_back("Strike price must be positive");
    }
    
    // 验证到期时间为正数
    if (!is_positive(params.time_to_maturity)) {
        result.is_valid = false;
        result.errors.push_back("Time to maturity must be positive");
    }
    
    // 验证波动率非负
    if (!is_non_negative(params.volatility)) {
        result.is_valid = false;
        result.errors.push_back("Volatility must be non-negative");
    }
    
    // 验证无风险利率为有限数
    if (!is_finite(params.risk_free_rate)) {
        result.is_valid = false;
        result.errors.push_back("Risk-free rate must be a finite number");
    }
    
    // 验证期权类型有效性（枚举类型自动保证有效性，但检查以防万一）
    if (params.option_type != OptionType::Call && params.option_type != OptionType::Put) {
        result.is_valid = false;
        result.errors.push_back("Option type must be Call or Put");
    }
    
    return result;
}

// 验证二叉树步数
InputValidator::ValidationResult InputValidator::validate_binomial_steps(int steps) {
    ValidationResult result;
    result.is_valid = true;
    
    // 验证步数在[10, 1000]范围内
    if (steps < 10) {
        result.is_valid = false;
        result.errors.push_back("Binomial tree steps must be at least 10");
    }
    
    if (steps > 1000) {
        result.is_valid = false;
        result.errors.push_back("Binomial tree steps must not exceed 1000");
    }
    
    return result;
}

// 检查是否为正数
bool InputValidator::is_positive(double value) {
    return std::isfinite(value) && value > 0.0;
}

// 检查是否为非负数
bool InputValidator::is_non_negative(double value) {
    return std::isfinite(value) && value >= 0.0;
}

// 检查是否为有限数
bool InputValidator::is_finite(double value) {
    return std::isfinite(value);
}

} // namespace option_pricer
