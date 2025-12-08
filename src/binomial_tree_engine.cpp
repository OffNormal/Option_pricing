#include "binomial_tree_engine.h"
#include "math_utils.h"
#include "error_logger.h"
#include "parameter_validator.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace option_pricer {

BinomialTreeEngine::BinomialTreeEngine(int steps) : steps_(steps) {
    if (steps < 10 || steps > 1000) {
        throw std::invalid_argument("Steps must be between 10 and 1000");
    }
}

PricingResult BinomialTreeEngine::calculate(const OptionParams& params) {
    PricingResult result;
    result.success = false;
    result.option_price = 0.0;
    result.greeks = {0.0, 0.0, 0.0, 0.0, 0.0};
    
    try {
        // 验证输入参数
        if (params.spot_price <= 0 || params.strike_price <= 0 || 
            params.time_to_maturity <= 0 || params.volatility < 0) {
            std::stringstream ss;
            ss << "Invalid input parameters: spot=" << params.spot_price 
               << ", strike=" << params.strike_price 
               << ", time=" << params.time_to_maturity 
               << ", vol=" << params.volatility;
            result.error_message = "Invalid input parameters: prices and time must be positive, volatility must be non-negative";
            ErrorLogger::error(result.error_message, "BinomialTreeEngine");
            ErrorLogger::error(ss.str(), "BinomialTreeEngine");
            return result;
        }
        
        // 检查是否为有限值
        if (!std::isfinite(params.spot_price) || !std::isfinite(params.strike_price) ||
            !std::isfinite(params.time_to_maturity) || !std::isfinite(params.risk_free_rate) ||
            !std::isfinite(params.volatility)) {
            result.error_message = "Invalid input parameters: all values must be finite";
            ErrorLogger::error(result.error_message, "BinomialTreeEngine");
            return result;
        }
        
        // 检查极端参数值并发出警告
        auto warnings = ParameterValidator::check_for_extreme_values(params);
        for (const auto& warning : warnings) {
            result.warnings.push_back(warning.warning_message);
            ErrorLogger::warning(warning.warning_message, "BinomialTreeEngine");
        }
        
        // 计算期权价格
        result.option_price = calculate_option_price(params);
        
        // 检查价格是否有效
        if (!std::isfinite(result.option_price)) {
            std::stringstream ss;
            ss << "Non-finite option price calculated: " << result.option_price;
            result.error_message = "Invalid option price calculated (non-finite)";
            ErrorLogger::error(ss.str(), "BinomialTreeEngine");
            result.success = false;
            return result;
        }
        
        if (result.option_price < 0.0) {
            std::stringstream ss;
            ss << "Negative option price calculated: " << result.option_price;
            result.error_message = "Invalid option price calculated (negative)";
            ErrorLogger::error(ss.str(), "BinomialTreeEngine");
            result.success = false;
            return result;
        }
        
        // 计算希腊值
        result.greeks = calculate_greeks_numerical(params);
        
        // 验证希腊值的有效性
        if (!std::isfinite(result.greeks.delta) || !std::isfinite(result.greeks.gamma) ||
            !std::isfinite(result.greeks.theta) || !std::isfinite(result.greeks.vega) ||
            !std::isfinite(result.greeks.rho)) {
            std::stringstream ss;
            ss << "Non-finite Greeks calculated: delta=" << result.greeks.delta 
               << ", gamma=" << result.greeks.gamma << ", theta=" << result.greeks.theta 
               << ", vega=" << result.greeks.vega << ", rho=" << result.greeks.rho;
            result.warnings.push_back("Some Greeks values are non-finite");
            ErrorLogger::warning(ss.str(), "BinomialTreeEngine");
        }
        
        result.success = true;
        ErrorLogger::info("Option pricing calculation completed successfully", "BinomialTreeEngine");
        
    } catch (const std::overflow_error& e) {
        std::stringstream ss;
        ss << "Numerical overflow during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BinomialTreeEngine");
    } catch (const std::underflow_error& e) {
        std::stringstream ss;
        ss << "Numerical underflow during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BinomialTreeEngine");
    } catch (const std::domain_error& e) {
        std::stringstream ss;
        ss << "Domain error (invalid mathematical operation): " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BinomialTreeEngine");
    } catch (const std::runtime_error& e) {
        std::stringstream ss;
        ss << "Runtime error during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BinomialTreeEngine");
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Exception during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BinomialTreeEngine");
    } catch (...) {
        result.error_message = "Unknown exception during calculation";
        result.success = false;
        ErrorLogger::error(result.error_message, "BinomialTreeEngine");
    }
    
    return result;
}

std::string BinomialTreeEngine::get_model_name() const {
    return "Binomial Tree";
}

void BinomialTreeEngine::set_steps(int steps) {
    if (steps < 10 || steps > 1000) {
        throw std::invalid_argument("Steps must be between 10 and 1000");
    }
    steps_ = steps;
}

double BinomialTreeEngine::calculate_up_factor(double volatility, double dt) const {
    // u = e^(σ√Δt)
    
    // 检查输入
    if (volatility < 0.0) {
        throw std::domain_error("Volatility must be non-negative");
    }
    
    if (dt <= 0.0) {
        throw std::domain_error("Time step must be positive");
    }
    
    double sqrt_dt = std::sqrt(dt);
    if (!std::isfinite(sqrt_dt)) {
        throw std::overflow_error("Non-finite sqrt(dt) calculated");
    }
    
    double exponent = volatility * sqrt_dt;
    if (!std::isfinite(exponent)) {
        throw std::overflow_error("Non-finite exponent for up factor");
    }
    
    // 检查指数是否过大（可能导致溢出）
    if (exponent > 100.0) {
        throw std::overflow_error("Exponent too large for up factor calculation");
    }
    
    double u = std::exp(exponent);
    
    if (!std::isfinite(u)) {
        throw std::overflow_error("Non-finite up factor calculated");
    }
    
    return u;
}

double BinomialTreeEngine::calculate_down_factor(double up_factor) const {
    // d = 1/u
    
    // 检查除零
    if (up_factor == 0.0) {
        throw std::domain_error("Up factor is zero, cannot calculate down factor");
    }
    
    if (!std::isfinite(up_factor)) {
        throw std::domain_error("Up factor must be finite");
    }
    
    double d = 1.0 / up_factor;
    
    if (!std::isfinite(d)) {
        throw std::overflow_error("Non-finite down factor calculated");
    }
    
    return d;
}

double BinomialTreeEngine::calculate_risk_neutral_probability(
    double risk_free_rate, double dt, double up_factor, double down_factor) const {
    // p = (e^(rΔt) - d) / (u - d)
    
    // 检查输入
    if (!std::isfinite(risk_free_rate) || !std::isfinite(dt)) {
        throw std::domain_error("Risk-free rate and dt must be finite");
    }
    
    if (dt <= 0.0) {
        throw std::domain_error("Time step must be positive");
    }
    
    // 检查指数是否过大
    double exponent = risk_free_rate * dt;
    if (!std::isfinite(exponent)) {
        throw std::overflow_error("Non-finite exponent for growth factor");
    }
    
    if (std::abs(exponent) > 100.0) {
        throw std::overflow_error("Exponent too large for growth factor calculation");
    }
    
    double growth_factor = std::exp(exponent);
    
    if (!std::isfinite(growth_factor)) {
        throw std::overflow_error("Non-finite growth factor calculated");
    }
    
    // 检查除零
    double denominator = up_factor - down_factor;
    if (denominator == 0.0) {
        throw std::domain_error("Up factor equals down factor, cannot calculate probability");
    }
    
    if (!std::isfinite(denominator)) {
        throw std::domain_error("Non-finite denominator in probability calculation");
    }
    
    double numerator = growth_factor - down_factor;
    if (!std::isfinite(numerator)) {
        throw std::overflow_error("Non-finite numerator in probability calculation");
    }
    
    double p = numerator / denominator;
    
    if (!std::isfinite(p)) {
        throw std::overflow_error("Non-finite probability calculated");
    }
    
    return p;
}

double BinomialTreeEngine::calculate_option_price(const OptionParams& params) const {
    // 检查除零错误
    if (steps_ <= 0) {
        throw std::domain_error("Number of steps must be positive");
    }
    
    // 计算时间步长
    double dt = params.time_to_maturity / steps_;
    
    // 检查时间步长
    if (!std::isfinite(dt) || dt <= 0.0) {
        throw std::domain_error("Invalid time step calculated");
    }
    
    // 计算上涨和下跌因子
    double u = calculate_up_factor(params.volatility, dt);
    double d = calculate_down_factor(u);
    
    // 检查因子的有效性
    if (!std::isfinite(u) || !std::isfinite(d)) {
        throw std::overflow_error("Non-finite up/down factors calculated");
    }
    
    if (u <= 0.0 || d <= 0.0) {
        throw std::domain_error("Up and down factors must be positive");
    }
    
    if (u <= d) {
        throw std::domain_error("Up factor must be greater than down factor");
    }
    
    // 计算风险中性概率
    double p = calculate_risk_neutral_probability(params.risk_free_rate, dt, u, d);
    
    // 验证概率在[0,1]范围内
    if (!std::isfinite(p)) {
        throw std::overflow_error("Non-finite risk-neutral probability calculated");
    }
    
    if (p < 0.0 || p > 1.0) {
        std::stringstream ss;
        ss << "Invalid risk-neutral probability: " << p 
           << " (must be in [0,1]). u=" << u << ", d=" << d 
           << ", r=" << params.risk_free_rate << ", dt=" << dt;
        throw std::runtime_error(ss.str());
    }
    
    // 计算折现因子
    double discount = std::exp(-params.risk_free_rate * dt);
    
    // 检查折现因子
    if (!std::isfinite(discount) || discount <= 0.0) {
        throw std::overflow_error("Invalid discount factor calculated");
    }
    
    // 初始化最后一层的期权价值
    // 使用单个向量来存储当前层的值，节省内存
    std::vector<double> option_values(steps_ + 1);
    
    // 计算到期时每个节点的标的资产价格和期权价值
    for (int i = 0; i <= steps_; ++i) {
        // 在第i个节点，经历了i次上涨和(steps_-i)次下跌
        double spot_at_node = params.spot_price * std::pow(u, i) * std::pow(d, steps_ - i);
        
        // 检查资产价格是否有效
        if (!std::isfinite(spot_at_node)) {
            std::stringstream ss;
            ss << "Non-finite spot price at node i=" << i 
               << ", u^i=" << std::pow(u, i) 
               << ", d^(n-i)=" << std::pow(d, steps_ - i);
            throw std::overflow_error(ss.str());
        }
        
        option_values[i] = calculate_payoff(spot_at_node, params.strike_price, params.option_type);
        
        // 检查期权价值
        if (!std::isfinite(option_values[i])) {
            throw std::overflow_error("Non-finite option value at terminal node");
        }
    }
    
    // 向后递推计算期权价格
    for (int step = steps_ - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            // 计算期望价值并折现
            double expected_value = p * option_values[i + 1] + (1.0 - p) * option_values[i];
            
            // 检查期望值
            if (!std::isfinite(expected_value)) {
                throw std::overflow_error("Non-finite expected value during backward induction");
            }
            
            option_values[i] = discount * expected_value;
            
            // 检查折现后的值
            if (!std::isfinite(option_values[i])) {
                throw std::overflow_error("Non-finite discounted value during backward induction");
            }
            
            // 对于美式期权，需要比较提前行权价值和持有价值
            // 但当前任务只实现欧式期权，所以不需要这一步
        }
    }
    
    return option_values[0];
}

double BinomialTreeEngine::calculate_payoff(double spot_price, double strike_price, 
                                           OptionType option_type) const {
    if (option_type == OptionType::Call) {
        // 看涨期权: max(S - K, 0)
        return std::max(spot_price - strike_price, 0.0);
    } else {
        // 看跌期权: max(K - S, 0)
        return std::max(strike_price - spot_price, 0.0);
    }
}

Greeks BinomialTreeEngine::calculate_greeks_numerical(const OptionParams& params) const {
    Greeks greeks;
    
    // 使用有限差分法计算希腊值
    // 基准价格
    double base_price = calculate_option_price(params);
    
    // Delta: ∂V/∂S
    // 使用中心差分: Delta ≈ (V(S+ΔS) - V(S-ΔS)) / (2*ΔS)
    double dS = params.spot_price * 0.01;  // 1%的价格变化
    
    OptionParams params_up = params;
    params_up.spot_price = params.spot_price + dS;
    double price_up = calculate_option_price(params_up);
    
    OptionParams params_down = params;
    params_down.spot_price = params.spot_price - dS;
    double price_down = calculate_option_price(params_down);
    
    greeks.delta = (price_up - price_down) / (2.0 * dS);
    
    // Gamma: ∂²V/∂S²
    // 使用中心差分: Gamma ≈ (V(S+ΔS) - 2*V(S) + V(S-ΔS)) / (ΔS²)
    greeks.gamma = (price_up - 2.0 * base_price + price_down) / (dS * dS);
    
    // Theta: ∂V/∂t (注意：这里计算的是 -∂V/∂T，即时间衰减)
    // 使用向前差分: Theta ≈ (V(T-ΔT) - V(T)) / ΔT
    double dT = 1.0 / 365.0;  // 1天的时间变化（以年为单位）
    
    if (params.time_to_maturity > dT) {
        OptionParams params_time = params;
        params_time.time_to_maturity = params.time_to_maturity - dT;
        double price_time = calculate_option_price(params_time);
        greeks.theta = (price_time - base_price) / dT;
    } else {
        // 如果到期时间太短，无法计算Theta
        greeks.theta = 0.0;
    }
    
    // Vega: ∂V/∂σ
    // 使用中心差分: Vega ≈ (V(σ+Δσ) - V(σ-Δσ)) / (2*Δσ)
    double dSigma = 0.01;  // 1%的波动率变化
    
    OptionParams params_vol_up = params;
    params_vol_up.volatility = params.volatility + dSigma;
    double price_vol_up = calculate_option_price(params_vol_up);
    
    OptionParams params_vol_down = params;
    params_vol_down.volatility = std::max(0.0, params.volatility - dSigma);
    double price_vol_down = calculate_option_price(params_vol_down);
    
    greeks.vega = (price_vol_up - price_vol_down) / (2.0 * dSigma);
    
    // Rho: ∂V/∂r
    // 使用中心差分: Rho ≈ (V(r+Δr) - V(r-Δr)) / (2*Δr)
    double dr = 0.01;  // 1%的利率变化
    
    OptionParams params_rate_up = params;
    params_rate_up.risk_free_rate = params.risk_free_rate + dr;
    double price_rate_up = calculate_option_price(params_rate_up);
    
    OptionParams params_rate_down = params;
    params_rate_down.risk_free_rate = params.risk_free_rate - dr;
    double price_rate_down = calculate_option_price(params_rate_down);
    
    greeks.rho = (price_rate_up - price_rate_down) / (2.0 * dr);
    
    return greeks;
}

} // namespace option_pricer
