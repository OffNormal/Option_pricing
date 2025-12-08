#include "black_scholes_engine.h"
#include "math_utils.h"
#include "error_logger.h"
#include "parameter_validator.h"
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

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
            std::stringstream ss;
            ss << "Invalid input parameters: spot=" << params.spot_price 
               << ", strike=" << params.strike_price 
               << ", time=" << params.time_to_maturity 
               << ", vol=" << params.volatility;
            result.error_message = "Invalid input parameters: prices and time must be positive, volatility must be non-negative";
            ErrorLogger::error(result.error_message, "BlackScholesEngine");
            ErrorLogger::error(ss.str(), "BlackScholesEngine");
            return result;
        }
        
        // 检查是否为有限值
        if (!std::isfinite(params.spot_price) || !std::isfinite(params.strike_price) ||
            !std::isfinite(params.time_to_maturity) || !std::isfinite(params.risk_free_rate) ||
            !std::isfinite(params.volatility)) {
            result.error_message = "Invalid input parameters: all values must be finite";
            ErrorLogger::error(result.error_message, "BlackScholesEngine");
            return result;
        }
        
        // 检查极端参数值并发出警告
        auto warnings = ParameterValidator::check_for_extreme_values(params);
        for (const auto& warning : warnings) {
            result.warnings.push_back(warning.warning_message);
            ErrorLogger::warning(warning.warning_message, "BlackScholesEngine");
        }
        
        // 计算d1和d2
        double d1 = calculate_d1(params);
        double d2 = calculate_d2(params, d1);
        
        // 检查d1和d2是否有效
        if (!std::isfinite(d1) || !std::isfinite(d2)) {
            std::stringstream ss;
            ss << "Numerical error in d1/d2 calculation: d1=" << d1 << ", d2=" << d2;
            result.error_message = "Numerical error in d1/d2 calculation";
            ErrorLogger::error(ss.str(), "BlackScholesEngine");
            return result;
        }
        
        // 检查数值溢出风险
        if (std::abs(d1) > 10.0 || std::abs(d2) > 10.0) {
            std::stringstream ss;
            ss << "Extreme d1/d2 values detected: d1=" << d1 << ", d2=" << d2 
               << " - may indicate numerical instability";
            result.warnings.push_back(ss.str());
            ErrorLogger::warning(ss.str(), "BlackScholesEngine");
        }
        
        // 根据期权类型计算价格
        double price = 0.0;
        if (params.option_type == OptionType::Call) {
            price = calculate_call_price(params, d1, d2);
        } else {
            price = calculate_put_price(params, d1, d2);
        }
        
        // 检查价格是否有效
        if (!std::isfinite(price)) {
            std::stringstream ss;
            ss << "Non-finite option price calculated: " << price;
            result.error_message = "Invalid option price calculated (non-finite)";
            ErrorLogger::error(ss.str(), "BlackScholesEngine");
            return result;
        }
        
        if (price < 0.0) {
            std::stringstream ss;
            ss << "Negative option price calculated: " << price;
            result.error_message = "Invalid option price calculated (negative)";
            ErrorLogger::error(ss.str(), "BlackScholesEngine");
            return result;
        }
        
        // 计算希腊值
        Greeks greeks = calculate_greeks(params, d1, d2);
        
        // 验证希腊值的有效性
        if (!std::isfinite(greeks.delta) || !std::isfinite(greeks.gamma) ||
            !std::isfinite(greeks.theta) || !std::isfinite(greeks.vega) ||
            !std::isfinite(greeks.rho)) {
            std::stringstream ss;
            ss << "Non-finite Greeks calculated: delta=" << greeks.delta 
               << ", gamma=" << greeks.gamma << ", theta=" << greeks.theta 
               << ", vega=" << greeks.vega << ", rho=" << greeks.rho;
            result.warnings.push_back("Some Greeks values are non-finite");
            ErrorLogger::warning(ss.str(), "BlackScholesEngine");
        }
        
        // 设置结果
        result.option_price = price;
        result.greeks = greeks;
        result.success = true;
        result.error_message = "";
        
        // 记录成功计算
        ErrorLogger::info("Option pricing calculation completed successfully", "BlackScholesEngine");
        
    } catch (const std::overflow_error& e) {
        std::stringstream ss;
        ss << "Numerical overflow during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BlackScholesEngine");
    } catch (const std::underflow_error& e) {
        std::stringstream ss;
        ss << "Numerical underflow during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BlackScholesEngine");
    } catch (const std::domain_error& e) {
        std::stringstream ss;
        ss << "Domain error (invalid mathematical operation): " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BlackScholesEngine");
    } catch (const std::runtime_error& e) {
        std::stringstream ss;
        ss << "Runtime error during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BlackScholesEngine");
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Exception during calculation: " << e.what();
        result.error_message = ss.str();
        result.success = false;
        ErrorLogger::error(result.error_message, "BlackScholesEngine");
    } catch (...) {
        result.error_message = "Unknown exception during calculation";
        result.success = false;
        ErrorLogger::error(result.error_message, "BlackScholesEngine");
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
    
    // 处理除零错误：检查执行价格
    if (K <= 0.0) {
        throw std::domain_error("Strike price must be positive for d1 calculation");
    }
    
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
    
    // 处理时间为零的特殊情况
    if (T <= 0.0) {
        throw std::domain_error("Time to maturity must be positive for d1 calculation");
    }
    
    double sqrt_T = std::sqrt(T);
    double sigma_sqrt_T = sigma * sqrt_T;
    
    // 再次检查除零（虽然前面已经检查过，但为了安全）
    if (sigma_sqrt_T == 0.0) {
        throw std::domain_error("sigma * sqrt(T) is zero, cannot calculate d1");
    }
    
    // 计算 ln(S/K) - 检查数值溢出
    double ratio = S / K;
    if (ratio <= 0.0) {
        throw std::domain_error("S/K ratio must be positive for logarithm");
    }
    
    double log_S_K = std::log(ratio);
    
    // 检查对数结果
    if (!std::isfinite(log_S_K)) {
        throw std::overflow_error("Logarithm calculation resulted in non-finite value");
    }
    
    // 计算分子: ln(S/K) + (r + σ²/2)T
    double numerator = log_S_K + (r + 0.5 * sigma * sigma) * T;
    
    // 检查分子是否有限
    if (!std::isfinite(numerator)) {
        throw std::overflow_error("Numerator calculation resulted in non-finite value");
    }
    
    // 计算d1
    double d1 = numerator / sigma_sqrt_T;
    
    // 检查结果
    if (!std::isfinite(d1)) {
        throw std::overflow_error("d1 calculation resulted in non-finite value");
    }
    
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
    
    // 计算N(d1)和N(d2) - 使用快速查表法
    double N_d1 = math::normal_cdf_fast(d1);
    double N_d2 = math::normal_cdf_fast(d2);
    
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
    
    // 计算N(-d1)和N(-d2) - 使用快速查表法
    double N_minus_d1 = math::normal_cdf_fast(-d1);
    double N_minus_d2 = math::normal_cdf_fast(-d2);
    
    // 计算折现因子
    double discount_factor = std::exp(-r * T);
    
    // 计算看跌期权价格
    double put_price = K * discount_factor * N_minus_d2 - S * N_minus_d1;
    
    return put_price;
}

Greeks BlackScholesEngine::calculate_greeks(const OptionParams& params, 
                                            double d1, double d2) const {
    Greeks greeks;
    
    double S = params.spot_price;
    double K = params.strike_price;
    double T = params.time_to_maturity;
    double r = params.risk_free_rate;
    double sigma = params.volatility;
    
    // 计算常用的中间值
    double sqrt_T = std::sqrt(T);
    double discount_factor = std::exp(-r * T);
    double n_d1 = math::normal_pdf(d1);  // φ(d1) - 标准正态PDF
    double N_d1 = math::normal_cdf_fast(d1);  // N(d1) - 标准正态CDF（使用快速查表法）
    double N_d2 = math::normal_cdf_fast(d2);  // N(d2)（使用快速查表法）
    
    // Delta: ∂V/∂S
    // 看涨期权: Delta = N(d1)
    // 看跌期权: Delta = N(d1) - 1 = -N(-d1)
    if (params.option_type == OptionType::Call) {
        greeks.delta = N_d1;
    } else {
        greeks.delta = N_d1 - 1.0;  // 等价于 -N(-d1)
    }
    
    // Gamma: ∂²V/∂S² (看涨和看跌相同)
    // Gamma = φ(d1) / (S * σ * √T)
    if (sigma > 0.0 && T > 0.0) {
        greeks.gamma = n_d1 / (S * sigma * sqrt_T);
    } else {
        greeks.gamma = 0.0;
    }
    
    // Vega: ∂V/∂σ (看涨和看跌相同)
    // Vega = S * φ(d1) * √T
    greeks.vega = S * n_d1 * sqrt_T;
    
    // Theta: ∂V/∂t (注意：通常以年为单位，这里计算的是 -∂V/∂T)
    // 看涨期权: Theta = -(S * φ(d1) * σ) / (2 * √T) - r * K * e^(-rT) * N(d2)
    // 看跌期权: Theta = -(S * φ(d1) * σ) / (2 * √T) + r * K * e^(-rT) * N(-d2)
    double theta_common = 0.0;
    if (T > 0.0 && sigma > 0.0) {
        theta_common = -(S * n_d1 * sigma) / (2.0 * sqrt_T);
    }
    
    if (params.option_type == OptionType::Call) {
        greeks.theta = theta_common - r * K * discount_factor * N_d2;
    } else {
        double N_minus_d2 = math::normal_cdf_fast(-d2);
        greeks.theta = theta_common + r * K * discount_factor * N_minus_d2;
    }
    
    // Rho: ∂V/∂r
    // 看涨期权: Rho = K * T * e^(-rT) * N(d2)
    // 看跌期权: Rho = -K * T * e^(-rT) * N(-d2)
    if (params.option_type == OptionType::Call) {
        greeks.rho = K * T * discount_factor * N_d2;
    } else {
        double N_minus_d2 = math::normal_cdf_fast(-d2);
        greeks.rho = -K * T * discount_factor * N_minus_d2;
    }
    
    return greeks;
}

} // namespace option_pricer
