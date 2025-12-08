#ifndef VISUALIZATION_GENERATOR_H
#define VISUALIZATION_GENERATOR_H

#include <vector>
#include <string>
#include "pricing_engine.h"
#include "option_params.h"

namespace option_pricer {

/**
 * 可视化数据生成器
 * 
 * 生成用于前端图表展示的数据
 * 包括价格曲线和希腊值曲线
 */
class VisualizationGenerator {
public:
    /**
     * 图表数据结构
     * 包含x值、y值和数据系列标签
     */
    struct ChartData {
        std::vector<double> x_values;  // x轴数据点
        std::vector<double> y_values;  // y轴数据点
        std::string label;             // 数据系列标签
    };
    
    /**
     * 生成期权价格曲线数据
     * 扫描标的资产价格范围，计算对应的期权价格
     * 
     * @param base_params 基础期权参数
     * @param engine 定价引擎
     * @param spot_min 标的资产价格最小值
     * @param spot_max 标的资产价格最大值
     * @param points 数据点数量（默认50）
     * @return 包含价格曲线数据的ChartData对象
     */
    static ChartData generate_price_curve(
        const OptionParams& base_params,
        IPricingEngine& engine,
        double spot_min,
        double spot_max,
        int points = 50
    );
    
    /**
     * 生成希腊值曲线数据
     * 扫描标的资产价格范围，计算对应的所有希腊值
     * 
     * @param base_params 基础期权参数
     * @param engine 定价引擎
     * @param spot_min 标的资产价格最小值
     * @param spot_max 标的资产价格最大值
     * @param points 数据点数量（默认50）
     * @return 包含五个希腊值曲线数据的向量（Delta、Gamma、Theta、Vega、Rho）
     */
    static std::vector<ChartData> generate_greeks_curves(
        const OptionParams& base_params,
        IPricingEngine& engine,
        double spot_min,
        double spot_max,
        int points = 50
    );
};

} // namespace option_pricer

#endif // VISUALIZATION_GENERATOR_H
