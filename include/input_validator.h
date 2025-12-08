#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <vector>
#include <string>
#include "option_params.h"

namespace option_pricer {

// 输入验证器类
class InputValidator {
public:
    // 验证结果结构
    struct ValidationResult {
        bool is_valid;                      // 是否有效
        std::vector<std::string> errors;    // 错误消息列表
    };
    
    // 验证期权参数
    static ValidationResult validate(const OptionParams& params);
    
    // 验证二叉树步数
    static ValidationResult validate_binomial_steps(int steps);
    
private:
    // 辅助函数：检查是否为正数
    static bool is_positive(double value);
    
    // 辅助函数：检查是否为非负数
    static bool is_non_negative(double value);
    
    // 辅助函数：检查是否为有限数
    static bool is_finite(double value);
};

} // namespace option_pricer

#endif // INPUT_VALIDATOR_H
