#include "json_serialization.h"
#include <stdexcept>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace option_pricer {

// 辅助函数：格式化数值到四位小数
double JsonSerializer::format_to_four_decimals(double value) {
    if (std::isnan(value) || std::isinf(value)) {
        return value;
    }
    return std::round(value * 10000.0) / 10000.0;
}

// 辅助函数：将OptionType枚举转换为字符串
std::string JsonSerializer::option_type_to_string(OptionType type) {
    switch (type) {
        case OptionType::Call:
            return "call";
        case OptionType::Put:
            return "put";
        default:
            throw std::invalid_argument("Unknown option type");
    }
}

// 辅助函数：将字符串转换为OptionType枚举
OptionType JsonSerializer::string_to_option_type(const std::string& type_str) {
    if (type_str == "call" || type_str == "Call" || type_str == "CALL") {
        return OptionType::Call;
    } else if (type_str == "put" || type_str == "Put" || type_str == "PUT") {
        return OptionType::Put;
    } else {
        throw std::invalid_argument("Invalid option type: " + type_str);
    }
}

// OptionParams反序列化
OptionParams JsonSerializer::deserialize_option_params(const nlohmann::json& j) {
    OptionParams params;
    
    try {
        // 必需字段
        params.spot_price = j.at("spot_price").get<double>();
        params.strike_price = j.at("strike_price").get<double>();
        params.time_to_maturity = j.at("time_to_maturity").get<double>();
        params.risk_free_rate = j.at("risk_free_rate").get<double>();
        params.volatility = j.at("volatility").get<double>();
        
        // 期权类型
        std::string type_str = j.at("option_type").get<std::string>();
        params.option_type = string_to_option_type(type_str);
        
    } catch (const nlohmann::json::exception& e) {
        throw std::invalid_argument("JSON parsing error: " + std::string(e.what()));
    }
    
    return params;
}

// Greeks序列化
nlohmann::json JsonSerializer::serialize_greeks(const Greeks& greeks) {
    nlohmann::json j;
    
    j["delta"] = format_to_four_decimals(greeks.delta);
    j["gamma"] = format_to_four_decimals(greeks.gamma);
    j["theta"] = format_to_four_decimals(greeks.theta);
    j["vega"] = format_to_four_decimals(greeks.vega);
    j["rho"] = format_to_four_decimals(greeks.rho);
    
    return j;
}

// PricingResult序列化
nlohmann::json JsonSerializer::serialize_pricing_result(const PricingResult& result,
                                                         const std::string& model_name,
                                                         double computation_time_ms) {
    nlohmann::json j;
    
    j["success"] = result.success;
    
    if (result.success) {
        // 成功响应
        j["option_price"] = format_to_four_decimals(result.option_price);
        j["greeks"] = serialize_greeks(result.greeks);
        
        if (!model_name.empty()) {
            j["model"] = model_name;
        }
        
        if (computation_time_ms > 0.0) {
            j["computation_time_ms"] = format_to_four_decimals(computation_time_ms);
        }
        
        // 添加警告（如果有）
        if (!result.warnings.empty()) {
            j["warnings"] = result.warnings;
        }
    } else {
        // 失败响应
        j["error"] = result.error_message;
    }
    
    return j;
}

// ChartData序列化
nlohmann::json JsonSerializer::serialize_chart_data(const VisualizationGenerator::ChartData& data) {
    nlohmann::json j;
    
    j["label"] = data.label;
    
    // 格式化x值到四位小数
    nlohmann::json x_array = nlohmann::json::array();
    for (double x : data.x_values) {
        x_array.push_back(format_to_four_decimals(x));
    }
    j["x"] = x_array;
    
    // 格式化y值到四位小数
    nlohmann::json y_array = nlohmann::json::array();
    for (double y : data.y_values) {
        y_array.push_back(format_to_four_decimals(y));
    }
    j["y"] = y_array;
    
    return j;
}

// 可视化数据序列化
nlohmann::json JsonSerializer::serialize_visualization_data(
    const VisualizationGenerator::ChartData& price_curve,
    const std::vector<VisualizationGenerator::ChartData>& greeks_curves) {
    
    nlohmann::json j;
    
    // 价格曲线
    j["price_curve"] = serialize_chart_data(price_curve);
    
    // 希腊值曲线
    nlohmann::json greeks_obj;
    for (const auto& curve : greeks_curves) {
        // 使用小写标签作为键
        std::string key = curve.label;
        // 转换为小写
        for (char& c : key) {
            c = std::tolower(c);
        }
        greeks_obj[key] = serialize_chart_data(curve);
    }
    j["greeks_curves"] = greeks_obj;
    
    return j;
}

// 错误响应序列化
nlohmann::json JsonSerializer::serialize_error_response(const std::string& error,
                                                         const std::vector<std::string>& details) {
    nlohmann::json j;
    
    j["success"] = false;
    j["error"] = error;
    
    if (!details.empty()) {
        j["details"] = details;
    }
    
    return j;
}

} // namespace option_pricer
