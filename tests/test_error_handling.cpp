#include <gtest/gtest.h>
#include "black_scholes_engine.h"
#include "binomial_tree_engine.h"
#include "parameter_validator.h"
#include <cmath>
#include <limits>

using namespace option_pricer;

// 测试异常处理和错误管理
class ErrorHandlingTest : public ::testing::Test {
protected:
    BlackScholesEngine bs_engine;
    BinomialTreeEngine binomial_engine{100};
};

// 测试极端波动率警告
TEST_F(ErrorHandlingTest, ExtremeVolatilityWarning) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 2.5;  // 250% - 极端值
    params.option_type = OptionType::Call;
    
    auto result = bs_engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.warnings.empty());
    EXPECT_GT(result.warnings.size(), 0);
}

// 测试极端到期时间警告
TEST_F(ErrorHandlingTest, ExtremeTimeToMaturityWarning) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 15.0;  // 15年 - 极端值
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = bs_engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.warnings.empty());
}

// 测试极端价格比率警告
TEST_F(ErrorHandlingTest, ExtremePriceRatioWarning) {
    OptionParams params;
    params.spot_price = 1000.0;
    params.strike_price = 50.0;  // S/K = 20 - 极端值
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = bs_engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.warnings.empty());
}

// 测试非有限值处理
TEST_F(ErrorHandlingTest, NonFiniteInputs) {
    OptionParams params;
    params.spot_price = std::numeric_limits<double>::infinity();
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = bs_engine.calculate(params);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// 测试NaN输入处理
TEST_F(ErrorHandlingTest, NaNInputs) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = std::numeric_limits<double>::quiet_NaN();
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = bs_engine.calculate(params);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// 测试参数验证器
TEST_F(ErrorHandlingTest, ParameterValidatorExtremeVolatility) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 3.0;  // 300%
    params.option_type = OptionType::Call;
    
    auto warnings = ParameterValidator::check_for_extreme_values(params);
    
    EXPECT_FALSE(warnings.empty());
    EXPECT_EQ(warnings[0].parameter_name, "volatility");
}

// 测试参数验证器 - 极端利率
TEST_F(ErrorHandlingTest, ParameterValidatorExtremeRate) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.6;  // 60%
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto warnings = ParameterValidator::check_for_extreme_values(params);
    
    EXPECT_FALSE(warnings.empty());
    bool found_rate_warning = false;
    for (const auto& warning : warnings) {
        if (warning.parameter_name == "risk_free_rate") {
            found_rate_warning = true;
            break;
        }
    }
    EXPECT_TRUE(found_rate_warning);
}

// 测试二叉树引擎的异常处理
TEST_F(ErrorHandlingTest, BinomialTreeExtremeValues) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 2.5;  // 极端波动率
    params.option_type = OptionType::Call;
    
    auto result = binomial_engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.warnings.empty());
}

// 测试二叉树引擎的无效输入
TEST_F(ErrorHandlingTest, BinomialTreeInvalidInputs) {
    OptionParams params;
    params.spot_price = -100.0;  // 负数
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = binomial_engine.calculate(params);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// 测试正常情况下没有警告
TEST_F(ErrorHandlingTest, NoWarningsForNormalParameters) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 100.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    auto result = bs_engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.warnings.empty());
}
