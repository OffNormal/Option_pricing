// 输入验证器测试
#include <gtest/gtest.h>
#include "input_validator.h"
#include "option_params.h"

using namespace option_pricer;

// 测试有效输入通过验证
TEST(InputValidatorTest, ValidInputs) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
}

// 测试负数标的资产价格被拒绝
TEST(InputValidatorTest, NegativeSpotPrice) {
    OptionParams params;
    params.spot_price = -100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
    EXPECT_EQ(result.errors[0], "Spot price must be positive");
}

// 测试零标的资产价格被拒绝
TEST(InputValidatorTest, ZeroSpotPrice) {
    OptionParams params;
    params.spot_price = 0.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
}

// 测试负数执行价格被拒绝
TEST(InputValidatorTest, NegativeStrikePrice) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = -105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
    EXPECT_EQ(result.errors[0], "Strike price must be positive");
}

// 测试负数到期时间被拒绝
TEST(InputValidatorTest, NegativeTimeToMaturity) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = -1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
    EXPECT_EQ(result.errors[0], "Time to maturity must be positive");
}

// 测试零到期时间被拒绝
TEST(InputValidatorTest, ZeroTimeToMaturity) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 0.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
}

// 测试负数波动率被拒绝
TEST(InputValidatorTest, NegativeVolatility) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = -0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
    EXPECT_EQ(result.errors[0], "Volatility must be non-negative");
}

// 测试零波动率被接受（边界情况）
TEST(InputValidatorTest, ZeroVolatility) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.0;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
}

// 测试多个错误同时报告
TEST(InputValidatorTest, MultipleErrors) {
    OptionParams params;
    params.spot_price = -100.0;
    params.strike_price = -105.0;
    params.time_to_maturity = -1.0;
    params.risk_free_rate = 0.05;
    params.volatility = -0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_EQ(result.errors.size(), 4);  // 四个错误
}

// 测试有效的二叉树步数
TEST(InputValidatorTest, ValidBinomialSteps) {
    auto result = InputValidator::validate_binomial_steps(100);
    
    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
}

// 测试最小边界步数
TEST(InputValidatorTest, MinimumBinomialSteps) {
    auto result = InputValidator::validate_binomial_steps(10);
    
    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
}

// 测试最大边界步数
TEST(InputValidatorTest, MaximumBinomialSteps) {
    auto result = InputValidator::validate_binomial_steps(1000);
    
    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
}

// 测试步数过小被拒绝
TEST(InputValidatorTest, TooFewBinomialSteps) {
    auto result = InputValidator::validate_binomial_steps(9);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
    EXPECT_EQ(result.errors[0], "Binomial tree steps must be at least 10");
}

// 测试步数过大被拒绝
TEST(InputValidatorTest, TooManyBinomialSteps) {
    auto result = InputValidator::validate_binomial_steps(1001);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
    EXPECT_EQ(result.errors[0], "Binomial tree steps must not exceed 1000");
}

// 测试无穷大值被拒绝
TEST(InputValidatorTest, InfiniteValues) {
    OptionParams params;
    params.spot_price = std::numeric_limits<double>::infinity();
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
}

// 测试NaN值被拒绝
TEST(InputValidatorTest, NaNValues) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = std::numeric_limits<double>::quiet_NaN();
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = InputValidator::validate(params);
    
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
}
