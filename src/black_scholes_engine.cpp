#include "black_scholes_engine.h"
#include "math_utils.h"
#include <cmath>
#include <limits>

namespace option_pricer {

PricingResult BlackScholesEngine::calculate(const OptionParams& params) {
    PricingResult result;
    result.success = false;
    result.option_price = 0.0;
    result.greeks = {0.0, 0.0, 0.0, 0.0, 0.0};
    
    try {
        // 验证输入参数的有效性
        if (params.spot_price <= 0.0 || params.strike_price <= 0.0 ||
            params.time_to_maturity <= 0.0 || params.volatility < 0.0) {
            result.error_message = "Invalid input parameters: prices and time must be positive, volatility must be non-negative";
            return result;
        }
        
        // 检查是否为有限值
        if (!std::isfinite(params.spot_price) || !std::isfinite(params.strike_price) ||
            !std::isfinite(params.time_to_maturity) || !std::isfinite(params.risk_free_rate) ||
            !std::isfinite(params.volatility)) {
            result.error_message = "Invalid input parameters: all values must be finite";
            return result;
        }
        
        // 计算d1和d2
        double d1 = calculate_d1(params);
        double d2 = calculate_d2(params, d1);
        
        // 检查d1和d2是否有效
        if (!std::isfinite(d1) || !std::isfinite(d2)) {
            result.error_message = "Numerical error in d1/d2 calculation";
            return result;
        }
        
        // 根据期权类型计算价格
        double price = 0.0;
        if (params.option_type == OptionType::Call) {
            price = calculate_call_price(params, d1, d2);
        } else {
            price = calculate_put_price(params, d1, d2);
        }
        
        // 检查价格是否有效
        if (!std::isfinite(price) || price < 0.0) {
            result.error_message = "Invalid option price calculated";
            return result;
        }
        
        // 设置结果
        result.option_price = price;
        result.success = true;
        result.error_message = "";
        
        // 注意：希腊值计算将在任务5中实现
        // 目前保持为零值
        
    } catch (const std::exception& e) {
        result.error_message = std::string("Exception during calculation: ") + e.what();
        result.success = false;
    } catch (...) {
        result.error_message = "Unknown exception during calculation";
        result.success = false;
    }
    
    return result;
}

std::string BlackScholesEngine::get_model_name() const {
    return "Black-Scholes";
}

double BlackScholesEngine::calculate_d1(const OptionParams& params) const {
    // d1 = [ln(S/K) + (r + σ²/2)T] / (σ√T)
    
    double S = params.spot_price;
    double K = params.strike_price;
    double T = params.time_to_maturity;
    double r = params.risk_free_rate;
    double sigma = params.volatility;
    
    // 处理波动率为零的特殊情况
    if (sigma == 0.0) {
        // 当波动率为0时，期权价格退化为内在价值
        // d1趋向于正无穷或负无穷，取决于S与K*e^(-rT)的关系
        double discounted_strike = K * std::exp(-r * T);
        if (S > discounted_strike) {
            return 10.0;  // 使用大的正数代替无穷大
        } else if (S < discounted_strike) {
            return -10.0;  // 使用大的负数代替无穷大
        } else {
            return 0.0;
        }
    }
    
    double sqrt_T = std::sqrt(T);
    double sigma_sqrt_T = sigma * sqrt_T;
    
    // 计算 ln(S/K)
    double log_S_K = std::log(S / K);
    
    // 计算分子: ln(S/K) + (r + σ²/2)T
    double numerator = log_S_K + (r + 0.5 * sigma * sigma) * T;
    
    // 计算d1
    double d1 = numerator / sigma_sqrt_T;
    
    return d1;
}

double BlackScholesEngine::calculate_d2(const OptionParams& params, double d1) const {
    // d2 = d1 - σ√T
    
    double sigma = params.volatility;
    double T = params.time_to_maturity;
    
    double sigma_sqrt_T = sigma * std::sqrt(T);
    double d2 = d1 - sigma_sqrt_T;
    
    return d2;
}

double BlackScholesEngine::calculate_call_price(const OptionParams& params, 
                                                 double d1, double d2) const {
    // C = S*N(d1) - K*e^(-rT)*N(d2)
    
    double S = params.spot_price;
    double K = params.strike_price;
    double T = params.time_to_maturity;
    double r = params.risk_free_rate;
    
    // 计算N(d1)和N(d2)
    double N_d1 = math::normal_cdf(d1);
    double N_d2 = math::normal_cdf(d2);
    
    // 计算折现因子
    double discount_factor = std::exp(-r * T);
    
    // 计算看涨期权价格
    double call_price = S * N_d1 - K * discount_factor * N_d2;
    
    return call_price;
}

double BlackScholesEngine::calculate_put_price(const OptionParams& params, 
                                                double d1, double d2) const {
    // P = K*e^(-rT)*N(-d2) - S*N(-d1)
    
    double S = params.spot_price;
    double K = params.strike_price;
    double T = params.time_to_maturity;
    double r = params.risk_free_rate;
    
    // 计算N(-d1)和N(-d2)
    double N_minus_d1 = math::normal_cdf(-d1);
    double N_minus_d2 = math::normal_cdf(-d2);
    
    // 计算折现因子
    double discount_factor = std::exp(-r * T);
    
    // 计算看跌期权价格
    double put_price = K * discount_factor * N_minus_d2 - S * N_minus_d1;
    
    return put_price;
}

} // namespace option_pricer
