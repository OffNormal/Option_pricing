#ifndef OPTION_PARAMS_H
#define OPTION_PARAMS_H

namespace option_pricer {

// 期权类型枚举
enum class OptionType {
    Call,
    Put
};

// 期权参数结构
struct OptionParams {
    double spot_price;        // 标的资产价格
    double strike_price;      // 执行价格
    double time_to_maturity;  // 到期时间（年）
    double risk_free_rate;    // 无风险利率
    double volatility;        // 波动率
    OptionType option_type;   // 期权类型
};

} // namespace option_pricer

#endif // OPTION_PARAMS_H
