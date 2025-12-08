#include "visualization_generator.h"
#include <stdexcept>
#include <cmath>

namespace option_pricer {

VisualizationGenerator::ChartData VisualizationGenerator::generate_price_curve(
    const OptionParams& base_params,
    IPricingEngine& engine,
    double spot_min,
    double spot_max,
    int points
) {
    // 验证输入参数
    if (spot_min <= 0 || spot_max <= 0) {
        throw std::invalid_argument("Spot price range must be positive");
    }
    if (spot_min >= spot_max) {
        throw std::invalid_argument("spot_min must be less than spot_max");
    }
    if (points <= 0) {
        throw std::invalid_argument("Number of points must be positive");
    }
    
    ChartData chart_data;
    chart_data.label = "Option Price";
    
    // 预分配向量空间
    chart_data.x_values.reserve(points);
    chart_data.y_values.reserve(points);
    
    // 计算步长
    double step = (spot_max - spot_min) / (points - 1);
    
    // 生成数据点
    for (int i = 0; i < points; ++i) {
        double spot_price = spot_min + i * step;
        
        // 创建修改后的参数
        OptionParams params = base_params;
        params.spot_price = spot_price;
        
        // 计算期权价格
        PricingResult result = engine.calculate(params);
        
        if (result.success) {
            chart_data.x_values.push_back(spot_price);
            chart_data.y_values.push_back(result.option_price);
        } else {
            // 如果计算失败，使用NaN标记
            chart_data.x_values.push_back(spot_price);
            chart_data.y_values.push_back(std::nan(""));
        }
    }
    
    return chart_data;
}

std::vector<VisualizationGenerator::ChartData> VisualizationGenerator::generate_greeks_curves(
    const OptionParams& base_params,
    IPricingEngine& engine,
    double spot_min,
    double spot_max,
    int points
) {
    // 验证输入参数
    if (spot_min <= 0 || spot_max <= 0) {
        throw std::invalid_argument("Spot price range must be positive");
    }
    if (spot_min >= spot_max) {
        throw std::invalid_argument("spot_min must be less than spot_max");
    }
    if (points <= 0) {
        throw std::invalid_argument("Number of points must be positive");
    }
    
    // 创建五个希腊值的数据系列
    std::vector<ChartData> greeks_data(5);
    greeks_data[0].label = "Delta";
    greeks_data[1].label = "Gamma";
    greeks_data[2].label = "Theta";
    greeks_data[3].label = "Vega";
    greeks_data[4].label = "Rho";
    
    // 预分配向量空间
    for (auto& data : greeks_data) {
        data.x_values.reserve(points);
        data.y_values.reserve(points);
    }
    
    // 计算步长
    double step = (spot_max - spot_min) / (points - 1);
    
    // 生成数据点
    for (int i = 0; i < points; ++i) {
        double spot_price = spot_min + i * step;
        
        // 创建修改后的参数
        OptionParams params = base_params;
        params.spot_price = spot_price;
        
        // 计算期权价格和希腊值
        PricingResult result = engine.calculate(params);
        
        if (result.success) {
            // Delta
            greeks_data[0].x_values.push_back(spot_price);
            greeks_data[0].y_values.push_back(result.greeks.delta);
            
            // Gamma
            greeks_data[1].x_values.push_back(spot_price);
            greeks_data[1].y_values.push_back(result.greeks.gamma);
            
            // Theta
            greeks_data[2].x_values.push_back(spot_price);
            greeks_data[2].y_values.push_back(result.greeks.theta);
            
            // Vega
            greeks_data[3].x_values.push_back(spot_price);
            greeks_data[3].y_values.push_back(result.greeks.vega);
            
            // Rho
            greeks_data[4].x_values.push_back(spot_price);
            greeks_data[4].y_values.push_back(result.greeks.rho);
        } else {
            // 如果计算失败，使用NaN标记
            double nan_value = std::nan("");
            for (auto& data : greeks_data) {
                data.x_values.push_back(spot_price);
                data.y_values.push_back(nan_value);
            }
        }
    }
    
    return greeks_data;
}

} // namespace option_pricer
