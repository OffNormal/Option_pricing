#include <gtest/gtest.h>
#include "binomial_tree_engine.h"
#include "black_scholes_engine.h"
#include <cmath>

using namespace option_pricer;

// 测试二叉树引擎基本功能
TEST(BinomialTreeEngineTest, BasicCallOption) {
    BinomialTreeEngine engine(100);
    
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.option_price, 0.0);
    EXPECT_FALSE(std::isnan(result.option_price));
    EXPECT_FALSE(std::isinf(result.option_price));
}

// 测试二叉树引擎看跌期权
TEST(BinomialTreeEngineTest, BasicPutOption) {
    BinomialTreeEngine engine(100);
    
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Put;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.option_price, 0.0);
    EXPECT_FALSE(std::isnan(result.option_price));
    EXPECT_FALSE(std::isinf(result.option_price));
}

// 测试步数设置
TEST(BinomialTreeEngineTest, SetSteps) {
    BinomialTreeEngine engine(50);
    EXPECT_EQ(engine.get_steps(), 50);
    
    engine.set_steps(200);
    EXPECT_EQ(engine.get_steps(), 200);
}

// 测试无效步数
TEST(BinomialTreeEngineTest, InvalidSteps) {
    EXPECT_THROW(BinomialTreeEngine(5), std::invalid_argument);
    EXPECT_THROW(BinomialTreeEngine(1500), std::invalid_argument);
    
    BinomialTreeEngine engine(100);
    EXPECT_THROW(engine.set_steps(5), std::invalid_argument);
    EXPECT_THROW(engine.set_steps(1500), std::invalid_argument);
}

// 测试与Black-Scholes的收敛性（粗略测试）
TEST(BinomialTreeEngineTest, ConvergenceToBlackScholes) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    // Black-Scholes价格
    BlackScholesEngine bs_engine;
    PricingResult bs_result = bs_engine.calculate(params);
    
    // 二叉树价格（100步）
    BinomialTreeEngine bt_engine_100(100);
    PricingResult bt_result_100 = bt_engine_100.calculate(params);
    
    // 二叉树价格（500步）
    BinomialTreeEngine bt_engine_500(500);
    PricingResult bt_result_500 = bt_engine_500.calculate(params);
    
    EXPECT_TRUE(bs_result.success);
    EXPECT_TRUE(bt_result_100.success);
    EXPECT_TRUE(bt_result_500.success);
    
    // 500步应该比100步更接近Black-Scholes
    double error_100 = std::abs(bt_result_100.option_price - bs_result.option_price);
    double error_500 = std::abs(bt_result_500.option_price - bs_result.option_price);
    
    // 500步的误差应该小于100步
    EXPECT_LT(error_500, error_100);
    
    // 500步应该在Black-Scholes价格的5%以内
    double relative_error = error_500 / bs_result.option_price;
    EXPECT_LT(relative_error, 0.05);
}

// 测试深度实值看涨期权
TEST(BinomialTreeEngineTest, DeepInTheMoneyCall) {
    BinomialTreeEngine engine(100);
    
    OptionParams params;
    params.spot_price = 150.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    // 深度实值期权价格应该接近内在价值
    double intrinsic_value = params.spot_price - params.strike_price;
    EXPECT_GT(result.option_price, intrinsic_value * 0.9);
}

// 测试深度虚值看涨期权
TEST(BinomialTreeEngineTest, DeepOutOfTheMoneyCall) {
    BinomialTreeEngine engine(100);
    
    OptionParams params;
    params.spot_price = 50.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    // 深度虚值期权价格应该很小
    EXPECT_LT(result.option_price, 5.0);
}

// 测试无效输入参数
TEST(BinomialTreeEngineTest, InvalidInputs) {
    BinomialTreeEngine engine(100);
    
    OptionParams params;
    params.spot_price = -100.0;  // 负数价格
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// 测试模型名称
TEST(BinomialTreeEngineTest, ModelName) {
    BinomialTreeEngine engine(100);
    EXPECT_EQ(engine.get_model_name(), "Binomial Tree");
}
