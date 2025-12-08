#ifndef PRICING_RESULT_H
#define PRICING_RESULT_H

#include <string>
#include "greeks.h"

namespace option_pricer {

// 定价结果结构
struct PricingResult {
    double option_price;           // 期权价格
    Greeks greeks;                 // 希腊值
    bool success;                  // 计算是否成功
    std::string error_message;     // 错误消息（如果失败）
};

} // namespace option_pricer

#endif // PRICING_RESULT_H
