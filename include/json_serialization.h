#ifndef JSON_SERIALIZATION_H
#define JSON_SERIALIZATION_H

#include <nlohmann/json.hpp>
#include "option_params.h"
#include "pricing_result.h"
#include "greeks.h"
#include "visualization_generator.h"

namespace option_pricer {

/**
 * JSON序列化和反序列化工具
 * 
 * 提供所有核心数据结构与JSON之间的转换功能
 */
class JsonSerializer {
public:
    // OptionParams反序列化
    static OptionParams deserialize_option_params(const nlohmann::json& j);
    
    // PricingResult序列化
    static nlohmann::json serialize_pricing_result(const PricingResult& result, 
                                                     const std::string& model_name = "",
                                                     double computation_time_ms = 0.0);
    
    // Greeks序列化
    static nlohmann::json serialize_greeks(const Greeks& greeks);
    
    // 可视化数据序列化
    static nlohmann::json serialize_chart_data(const VisualizationGenerator::ChartData& data);
    static nlohmann::json serialize_visualization_data(
        const VisualizationGenerator::ChartData& price_curve,
        const std::vector<VisualizationGenerator::ChartData>& greeks_curves
    );
    
    // 错误响应序列化
    static nlohmann::json serialize_error_response(const std::string& error,
                                                     const std::vector<std::string>& details = {});
    
private:
    // 辅助函数：将OptionType枚举转换为字符串
    static std::string option_type_to_string(OptionType type);
    
    // 辅助函数：将字符串转换为OptionType枚举
    static OptionType string_to_option_type(const std::string& type_str);
    
    // 辅助函数：格式化数值到四位小数
    static double format_to_four_decimals(double value);
};

} // namespace option_pricer

#endif // JSON_SERIALIZATION_H
