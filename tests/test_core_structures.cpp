#include <gtest/gtest.h>
#include "../include/option_params.h"
#include "../include/greeks.h"
#include "../include/pricing_result.h"
#include "../include/pricing_engine.h"

using namespace option_pricer;

// 测试OptionParams结构
TEST(CoreStructuresTest, OptionParamsCreation) {
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    EXPECT_EQ(params.spot_price, 100.0);
    EXPECT_EQ(params.strike_price, 105.0);
    EXPECT_EQ(params.time_to_maturity, 1.0);
    EXPECT_EQ(params.risk_free_rate, 0.05);
    EXPECT_EQ(params.volatility, 0.2);
    EXPECT_EQ(params.option_type, OptionType::Call);
}

// 测试Greeks结构
TEST(CoreStructuresTest, GreeksCreation) {
    Greeks greeks;
    greeks.delta = 0.5596;
    greeks.gamma = 0.0184;
    greeks.theta = -6.4135;
    greeks.vega = 36.7311;
    greeks.rho = 51.8644;
    
    EXPECT_DOUBLE_EQ(greeks.delta, 0.5596);
    EXPECT_DOUBLE_EQ(greeks.gamma, 0.0184);
    EXPECT_DOUBLE_EQ(greeks.theta, -6.4135);
    EXPECT_DOUBLE_EQ(greeks.vega, 36.7311);
    EXPECT_DOUBLE_EQ(greeks.rho, 51.8644);
}

// 测试PricingResult结构
TEST(CoreStructuresTest, PricingResultCreation) {
    PricingResult result;
    result.option_price = 8.9162;
    result.success = true;
    result.error_message = "";
    result.greeks.delta = 0.5596;
    
    EXPECT_DOUBLE_EQ(result.option_price, 8.9162);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.error_message, "");
    EXPECT_DOUBLE_EQ(result.greeks.delta, 0.5596);
}

// 测试OptionType枚举
TEST(CoreStructuresTest, OptionTypeEnum) {
    OptionType call = OptionType::Call;
    OptionType put = OptionType::Put;
    
    EXPECT_NE(call, put);
    EXPECT_EQ(call, OptionType::Call);
    EXPECT_EQ(put, OptionType::Put);
}

// 测试IPricingEngine接口存在性（通过创建mock实现）
class MockPricingEngine : public IPricingEngine {
public:
    PricingResult calculate(const OptionParams& params) override {
        PricingResult result;
        result.option_price = 10.0;
        result.success = true;
        result.error_message = "";
        return result;
    }
    
    std::string get_model_name() const override {
        return "Mock";
    }
};

TEST(CoreStructuresTest, IPricingEngineInterface) {
    MockPricingEngine engine;
    
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    PricingResult result = engine.calculate(params);
    
    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.option_price, 10.0);
    EXPECT_EQ(engine.get_model_name(), "Mock");
}
