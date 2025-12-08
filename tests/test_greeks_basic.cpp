#include <gtest/gtest.h>
#include "black_scholes_engine.h"
#include <cmath>

using namespace option_pricer;

// 测试希腊值计算基本功能
class GreeksBasicTest : public ::testing::Test {
protected:
    BlackScholesEngine engine;
};

// 测试看涨期权希腊值计算
TEST_F(GreeksBasicTest, CallOptionGreeks) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    
    // 验证所有希腊值都是有限数值
    EXPECT_TRUE(std::isfinite(result.greeks.delta));
    EXPECT_TRUE(std::isfinite(result.greeks.gamma));
    EXPECT_TRUE(std::isfinite(result.greeks.theta));
    EXPECT_TRUE(std::isfinite(result.greeks.vega));
    EXPECT_TRUE(std::isfinite(result.greeks.rho));
    
    // 验证Delta在[0, 1]范围内（看涨期权）
    EXPECT_GE(result.greeks.delta, 0.0);
    EXPECT_LE(result.greeks.delta, 1.0);
    
    // 验证Gamma非负
    EXPECT_GE(result.greeks.gamma, 0.0);
    
    // 验证Vega为正（期权价格随波动率增加而增加）
    EXPECT_GT(result.greeks.vega, 0.0);
}

// 测试看跌期权希腊值计算
TEST_F(GreeksBasicTest, PutOptionGreeks) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Put;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    
    // 验证所有希腊值都是有限数值
    EXPECT_TRUE(std::isfinite(result.greeks.delta));
    EXPECT_TRUE(std::isfinite(result.greeks.gamma));
    EXPECT_TRUE(std::isfinite(result.greeks.theta));
    EXPECT_TRUE(std::isfinite(result.greeks.vega));
    EXPECT_TRUE(std::isfinite(result.greeks.rho));
    
    // 验证Delta在[-1, 0]范围内（看跌期权）
    EXPECT_GE(result.greeks.delta, -1.0);
    EXPECT_LE(result.greeks.delta, 0.0);
    
    // 验证Gamma非负
    EXPECT_GE(result.greeks.gamma, 0.0);
    
    // 验证Vega为正
    EXPECT_GT(result.greeks.vega, 0.0);
}

// 测试平值期权的Delta约为0.5（看涨）
TEST_F(GreeksBasicTest, AtTheMoneyCallDelta) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    
    // 平值看涨期权的Delta应该接近0.5（略大于0.5因为有利率）
    EXPECT_NEAR(result.greeks.delta, 0.5, 0.15);
}

// 测试深度实值看涨期权的Delta接近1
TEST_F(GreeksBasicTest, DeepInTheMoneyCallDelta) {
    OptionParams params;
    params.spot_price = 150.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    
    // 深度实值看涨期权的Delta应该接近1
    EXPECT_GT(result.greeks.delta, 0.9);
}

// 测试深度虚值看涨期权的Delta接近0
TEST_F(GreeksBasicTest, DeepOutOfTheMoneyCallDelta) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 150.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    
    // 深度虚值看涨期权的Delta应该接近0
    EXPECT_LT(result.greeks.delta, 0.2);
}
