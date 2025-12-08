#include <gtest/gtest.h>
#include "visualization_generator.h"
#include "black_scholes_engine.h"
#include "binomial_tree_engine.h"
#include <cmath>

using namespace option_pricer;

// 测试价格曲线生成
TEST(VisualizationGeneratorTest, GeneratePriceCurve) {
    // 创建基础参数
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    // 使用Black-Scholes引擎
    BlackScholesEngine engine;
    
    // 生成价格曲线
    auto chart_data = VisualizationGenerator::generate_price_curve(
        params, engine, 80.0, 120.0, 50
    );
    
    // 验证数据点数量
    EXPECT_EQ(chart_data.x_values.size(), 50);
    EXPECT_EQ(chart_data.y_values.size(), 50);
    EXPECT_EQ(chart_data.label, "Option Price");
    
    // 验证x值范围和均匀分布
    EXPECT_DOUBLE_EQ(chart_data.x_values.front(), 80.0);
    EXPECT_DOUBLE_EQ(chart_data.x_values.back(), 120.0);
    
    // 验证x值是递增的
    for (size_t i = 1; i < chart_data.x_values.size(); ++i) {
        EXPECT_GT(chart_data.x_values[i], chart_data.x_values[i-1]);
    }
    
    // 验证y值都是有限的（非NaN、非无穷）
    for (double y : chart_data.y_values) {
        EXPECT_TRUE(std::isfinite(y));
    }
    
    // 验证期权价格是非负的
    for (double y : chart_data.y_values) {
        EXPECT_GE(y, 0.0);
    }
}

// 测试希腊值曲线生成
TEST(VisualizationGeneratorTest, GenerateGreeksCurves) {
    // 创建基础参数
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    // 使用Black-Scholes引擎
    BlackScholesEngine engine;
    
    // 生成希腊值曲线
    auto greeks_data = VisualizationGenerator::generate_greeks_curves(
        params, engine, 80.0, 120.0, 50
    );
    
    // 验证返回了5个希腊值的数据系列
    EXPECT_EQ(greeks_data.size(), 5);
    
    // 验证标签
    EXPECT_EQ(greeks_data[0].label, "Delta");
    EXPECT_EQ(greeks_data[1].label, "Gamma");
    EXPECT_EQ(greeks_data[2].label, "Theta");
    EXPECT_EQ(greeks_data[3].label, "Vega");
    EXPECT_EQ(greeks_data[4].label, "Rho");
    
    // 验证每个希腊值的数据点数量
    for (const auto& data : greeks_data) {
        EXPECT_EQ(data.x_values.size(), 50);
        EXPECT_EQ(data.y_values.size(), 50);
        
        // 验证x值范围
        EXPECT_DOUBLE_EQ(data.x_values.front(), 80.0);
        EXPECT_DOUBLE_EQ(data.x_values.back(), 120.0);
        
        // 验证y值都是有限的
        for (double y : data.y_values) {
            EXPECT_TRUE(std::isfinite(y));
        }
    }
}

// 测试自定义数据点数量
TEST(VisualizationGeneratorTest, CustomPointCount) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    BlackScholesEngine engine;
    
    // 测试不同的数据点数量
    for (int points : {10, 25, 100}) {
        auto chart_data = VisualizationGenerator::generate_price_curve(
            params, engine, 80.0, 120.0, points
        );
        
        EXPECT_EQ(chart_data.x_values.size(), points);
        EXPECT_EQ(chart_data.y_values.size(), points);
    }
}

// 测试二叉树引擎
TEST(VisualizationGeneratorTest, BinomialTreeEngine) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    BinomialTreeEngine engine(100);
    
    // 生成价格曲线
    auto chart_data = VisualizationGenerator::generate_price_curve(
        params, engine, 80.0, 120.0, 30
    );
    
    EXPECT_EQ(chart_data.x_values.size(), 30);
    EXPECT_EQ(chart_data.y_values.size(), 30);
    
    // 验证所有价格都是有限的
    for (double y : chart_data.y_values) {
        EXPECT_TRUE(std::isfinite(y));
        EXPECT_GE(y, 0.0);
    }
}

// 测试无效输入
TEST(VisualizationGeneratorTest, InvalidInputs) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    BlackScholesEngine engine;
    
    // 测试负数范围
    EXPECT_THROW(
        VisualizationGenerator::generate_price_curve(params, engine, -10.0, 120.0, 50),
        std::invalid_argument
    );
    
    // 测试spot_min >= spot_max
    EXPECT_THROW(
        VisualizationGenerator::generate_price_curve(params, engine, 120.0, 80.0, 50),
        std::invalid_argument
    );
    
    // 测试负数数据点数量
    EXPECT_THROW(
        VisualizationGenerator::generate_price_curve(params, engine, 80.0, 120.0, -10),
        std::invalid_argument
    );
    
    // 测试零数据点数量
    EXPECT_THROW(
        VisualizationGenerator::generate_price_curve(params, engine, 80.0, 120.0, 0),
        std::invalid_argument
    );
}

// 测试看跌期权
TEST(VisualizationGeneratorTest, PutOption) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Put;
    
    BlackScholesEngine engine;
    
    auto chart_data = VisualizationGenerator::generate_price_curve(
        params, engine, 80.0, 120.0, 50
    );
    
    EXPECT_EQ(chart_data.x_values.size(), 50);
    EXPECT_EQ(chart_data.y_values.size(), 50);
    
    // 验证所有价格都是非负的
    for (double y : chart_data.y_values) {
        EXPECT_GE(y, 0.0);
    }
}
