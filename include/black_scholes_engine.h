#ifndef BLACK_SCHOLES_ENGINE_H
#define BLACK_SCHOLES_ENGINE_H

#include "pricing_engine.h"

namespace option_pricer {

/**
 * Black-Scholes定价引擎
 * 
 * 实现Black-Scholes模型用于欧式期权定价
 * 使用解析公式计算期权价格和希腊值
 */
class BlackScholesEngine : public IPricingEngine {
public:
    BlackScholesEngine() = default;
    ~BlackScholesEngine() override = default;
    
    /**
     * 计算期权价格和希腊值
     * @param params 期权参数
     * @return 定价结果，包含期权价格和希腊值
     */
    PricingResult calculate(const OptionParams& params) override;
    
    /**
     * 获取模型名称
     * @return 模型名称字符串
     */
    std::string get_model_name() const override;
    
private:
    /**
     * 计算Black-Scholes公式中的d1参数
     * d1 = [ln(S/K) + (r + σ²/2)T] / (σ√T)
     * 
     * @param params 期权参数
     * @return d1值
     */
    double calculate_d1(const OptionParams& params) const;
    
    /**
     * 计算Black-Scholes公式中的d2参数
     * d2 = d1 - σ√T
     * 
     * @param params 期权参数
     * @param d1 已计算的d1值
     * @return d2值
     */
    double calculate_d2(const OptionParams& params, double d1) const;
    
    /**
     * 计算看涨期权价格
     * C = S*N(d1) - K*e^(-rT)*N(d2)
     * 
     * @param params 期权参数
     * @param d1 已计算的d1值
     * @param d2 已计算的d2值
     * @return 看涨期权价格
     */
    double calculate_call_price(const OptionParams& params, double d1, double d2) const;
    
    /**
     * 计算看跌期权价格
     * P = K*e^(-rT)*N(-d2) - S*N(-d1)
     * 
     * @param params 期权参数
     * @param d1 已计算的d1值
     * @param d2 已计算的d2值
     * @return 看跌期权价格
     */
    double calculate_put_price(const OptionParams& params, double d1, double d2) const;
    
    /**
     * 计算希腊值（使用解析公式）
     * 
     * @param params 期权参数
     * @param d1 已计算的d1值
     * @param d2 已计算的d2值
     * @return Greeks结构，包含所有五个希腊值
     */
    Greeks calculate_greeks(const OptionParams& params, double d1, double d2) const;
};

} // namespace option_pricer

#endif // BLACK_SCHOLES_ENGINE_H
