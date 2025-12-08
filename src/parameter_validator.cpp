#include "parameter_validator.h"
#include <cmath>
#include <sstream>

namespace option_pricer {

std::vector<ParameterValidator::ValidationWarning> 
ParameterValidator::check_for_extreme_values(const OptionParams& params) {
    std::vector<ValidationWarning> warnings;
    
    // 检查波动率
    if (params.volatility > MAX_VOLATILITY) {
        std::stringstream ss;
        ss << "Volatility (" << params.volatility << ") exceeds " 
           << (MAX_VOLATILITY * 100) << "%, which may indicate data error or extreme market conditions";
        warnings.push_back({"volatility", ss.str()});
    }
    
    if (params.volatility < MIN_VOLATILITY_WARNING && params.volatility > 0.0) {
        std::stringstream ss;
        ss << "Volatility (" << params.volatility << ") is very low (< " 
           << (MIN_VOLATILITY_WARNING * 100) << "%), which may cause numerical instability";
        warnings.push_back({"volatility", ss.str()});
    }
    
    // 检查到期时间
    if (params.time_to_maturity > MAX_TIME_TO_MATURITY) {
        std::stringstream ss;
        ss << "Time to maturity (" << params.time_to_maturity 
           << " years) exceeds " << MAX_TIME_TO_MATURITY 
           << " years, which is unusually long for standard options";
        warnings.push_back({"time_to_maturity", ss.str()});
    }
    
    // 检查价格比率
    double price_ratio = params.spot_price / params.strike_price;
    if (price_ratio > MAX_PRICE_RATIO) {
        std::stringstream ss;
        ss << "Spot/Strike ratio (" << price_ratio << ") is very high (> " 
           << MAX_PRICE_RATIO << "), option is deeply in-the-money";
        warnings.push_back({"price_ratio", ss.str()});
    }
    
    if (price_ratio < MIN_PRICE_RATIO) {
        std::stringstream ss;
        ss << "Spot/Strike ratio (" << price_ratio << ") is very low (< " 
           << MIN_PRICE_RATIO << "), option is deeply out-of-the-money";
        warnings.push_back({"price_ratio", ss.str()});
    }
    
    // 检查利率
    if (params.risk_free_rate > MAX_RATE) {
        std::stringstream ss;
        ss << "Risk-free rate (" << (params.risk_free_rate * 100) 
           << "%) exceeds " << (MAX_RATE * 100) 
           << "%, which is unusually high";
        warnings.push_back({"risk_free_rate", ss.str()});
    }
    
    if (params.risk_free_rate < MIN_RATE) {
        std::stringstream ss;
        ss << "Risk-free rate (" << (params.risk_free_rate * 100) 
           << "%) is below " << (MIN_RATE * 100) 
           << "%, which may indicate negative rates or data error";
        warnings.push_back({"risk_free_rate", ss.str()});
    }
    
    return warnings;
}

} // namespace option_pricer
