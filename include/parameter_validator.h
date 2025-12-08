#ifndef PARAMETER_VALIDATOR_H
#define PARAMETER_VALIDATOR_H

#include <string>
#include <vector>
#include "option_params.h"

namespace option_pricer {

// 参数验证器 - 检测极端值和潜在问题
class ParameterValidator {
public:
    struct ValidationWarning {
        std::string parameter_name;
        std::string warning_message;
    };
    
    // 检查参数是否在合理范围内，返回警告列表
    static std::vector<ValidationWarning> check_for_extreme_values(
        const OptionParams& params);
    
private:
    // 极端值阈值
    static constexpr double MAX_VOLATILITY = 2.0;      // 200%
    static constexpr double MAX_TIME_TO_MATURITY = 10.0; // 10年
    static constexpr double MAX_PRICE_RATIO = 10.0;    // S/K 或 K/S 的最大比率
    static constexpr double MIN_PRICE_RATIO = 0.1;     // S/K 或 K/S 的最小比率
    static constexpr double MAX_RATE = 0.5;            // 50%
    static constexpr double MIN_RATE = -0.1;           // -10%
    static constexpr double MIN_VOLATILITY_WARNING = 0.001; // 0.1%
};

} // namespace option_pricer

#endif // PARAMETER_VALIDATOR_H
