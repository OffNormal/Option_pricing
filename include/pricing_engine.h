#ifndef PRICING_ENGINE_H
#define PRICING_ENGINE_H

#include <string>
#include "option_params.h"
#include "pricing_result.h"

namespace option_pricer {

// 定价引擎抽象接口
class IPricingEngine {
public:
    virtual ~IPricingEngine() = default;
    
    // 计算期权价格和希腊值
    virtual PricingResult calculate(const OptionParams& params) = 0;
    
    // 获取模型名称
    virtual std::string get_model_name() const = 0;
};

} // namespace option_pricer

#endif // PRICING_ENGINE_H
