#include <gtest/gtest.h>
#include "black_scholes_engine.h"
#include <cmath>

using namespace option_pricer;

// 测试Black-Scholes引擎基本功能
class BlackScholesEngineTest : public ::testing::Test {
protected:
    BlackScholesEngine engine;
};

// 测试模型名称
TEST_F(BlackScholesEngineTest, ModelName) {
    EXPECT_EQ(engine.get_model_name(), "Black-Scholes");
}

// 测试看涨期权定价 - 使用标准教科书案例
TEST_F(BlackScholesEngineTest, CallOptionStandardCase) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(std::isfinite(result.option_price));
    EXPECT_GT(result.option_price, 0.0);
    
    // 期望价格约为10.45（根据Black-Scholes公式）
    EXPECT_NEAR(result.option_price, 10.45, 0.5);
}

// 测试看跌期权定价 - 使用标准教科书案例
TEST_F(BlackScholesEngineTest, PutOptionStandardCase) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Put;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(std::isfinite(result.option_price));
    EXPECT_GT(result.option_price, 0.0);
    
    // 期望价格约为5.57（根据Black-Scholes公式）
    EXPECT_NEAR(result.option_price, 5.57, 0.5);
}

// 测试看涨-看跌平价关系: C - P = S - K*e^(-rT)
TEST_F(BlackScholesEngineTest, PutCallParity) {
    OptionParams call_params;
    call_params.spot_price = 100.0;
    call_params.strike_price = 105.0;
    call_params.time_to_maturity = 1.0;
    call_params.risk_free_rate = 0.05;
    call_params.volatility = 0.2;
    call_params.option_type = OptionType::Call;
    
    OptionParams put_params = call_params;
    put_params.option_type = OptionType::Put;
    
    PricingResult call_result = engine.calculate(call_params);
    PricingResult put_result = engine.calculate(put_params);
    
    EXPECT_TRUE(call_result.success);
    EXPECT_TRUE(put_result.success);
    
    // 计算 C - P
    double left_side = call_result.option_price - put_result.option_price;
    
    // 计算 S - K*e^(-rT)
    double discount_factor = std::exp(-call_params.risk_free_rate * call_params.time_to_maturity);
    double right_side = call_params.spot_price - call_params.strike_price * discount_factor;
    
    // 验证平价关系，允许小的数值误差
    EXPECT_NEAR(left_side, right_side, 0.01);
}

// 测试深度实值看涨期权
TEST_F(BlackScholesEngineTest, DeepInTheMoneyCall) {
    OptionParams params;
    params.spot_price = 150.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(std::isfinite(result.option_price));
    
    // 深度实值期权价格应该接近内在价值 S - K*e^(-rT)
    double intrinsic_value = params.spot_price - params.strike_price * std::exp(-params.risk_free_rate * params.time_to_maturity);
    EXPECT_GT(result.option_price, intrinsic_value);
    EXPECT_LT(result.option_price, params.spot_price);
}

// 测试深度虚值看涨期权
TEST_F(BlackScholesEngineTest, DeepOutOfTheMoneyCall) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 150.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(std::isfinite(result.option_price));
    
    // 深度虚值期权价格应该很小但为正
    EXPECT_GT(result.option_price, 0.0);
    EXPECT_LT(result.option_price, 5.0);
}

// 测试无效输入：负数价格
TEST_F(BlackScholesEngineTest, InvalidNegativePrice) {
    OptionParams params;
    params.spot_price = -100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// 测试无效输入：负数到期时间
TEST_F(BlackScholesEngineTest, InvalidNegativeTime) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = -1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// 测试无效输入：负数波动率
TEST_F(BlackScholesEngineTest, InvalidNegativeVolatility) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = -0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// 测试零波动率的特殊情况
TEST_F(BlackScholesEngineTest, ZeroVolatility) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.0;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    // 零波动率时，期权价格应该等于内在价值的折现
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(std::isfinite(result.option_price));
}
