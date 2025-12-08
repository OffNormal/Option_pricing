#ifndef BINOMIAL_TREE_ENGINE_H
#define BINOMIAL_TREE_ENGINE_H

#include "pricing_engine.h"
#include <vector>

namespace option_pricer {

/**
 * 二叉树定价引擎
 * 
 * 实现二叉树模型用于期权定价
 * 使用离散时间方法和向后递推算法
 */
class BinomialTreeEngine : public IPricingEngine {
public:
    /**
     * 构造函数
     * @param steps 二叉树的时间步数（默认100步）
     */
    explicit BinomialTreeEngine(int steps = 100);
    
    ~BinomialTreeEngine() override = default;
    
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
    
    /**
     * 设置二叉树步数
     * @param steps 时间步数（10-1000）
     */
    void set_steps(int steps);
    
    /**
     * 获取当前步数
     * @return 当前时间步数
     */
    int get_steps() const { return steps_; }
    
private:
    int steps_;  // 二叉树的时间步数
    
    /**
     * 计算上涨因子
     * u = e^(σ√Δt)
     * 
     * @param volatility 波动率
     * @param dt 时间步长
     * @return 上涨因子
     */
    double calculate_up_factor(double volatility, double dt) const;
    
    /**
     * 计算下跌因子
     * d = 1/u
     * 
     * @param up_factor 上涨因子
     * @return 下跌因子
     */
    double calculate_down_factor(double up_factor) const;
    
    /**
     * 计算风险中性概率
     * p = (e^(rΔt) - d) / (u - d)
     * 
     * @param risk_free_rate 无风险利率
     * @param dt 时间步长
     * @param up_factor 上涨因子
     * @param down_factor 下跌因子
     * @return 风险中性概率
     */
    double calculate_risk_neutral_probability(double risk_free_rate, double dt,
                                              double up_factor, double down_factor) const;
    
    /**
     * 构建二叉树并计算期权价格
     * 使用向后递推算法
     * 
     * @param params 期权参数
     * @return 期权价格
     */
    double calculate_option_price(const OptionParams& params) const;
    
    /**
     * 计算到期时的期权价值
     * 
     * @param spot_price 标的资产价格
     * @param strike_price 执行价格
     * @param option_type 期权类型
     * @return 期权内在价值
     */
    double calculate_payoff(double spot_price, double strike_price, OptionType option_type) const;
    
    /**
     * 计算希腊值（使用数值方法）
     * 使用有限差分法计算Delta、Gamma、Theta、Vega和Rho
     * 
     * @param params 期权参数
     * @return Greeks结构
     */
    Greeks calculate_greeks_numerical(const OptionParams& params) const;
};

} // namespace option_pricer

#endif // BINOMIAL_TREE_ENGINE_H
