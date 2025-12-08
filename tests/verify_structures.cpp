#include <iostream>
#include <cassert>
#include "../include/option_params.h"
#include "../include/greeks.h"
#include "../include/pricing_result.h"
#include "../include/pricing_engine.h"

using namespace option_pricer;

// Mock实现用于测试接口
class MockPricingEngine : public IPricingEngine {
public:
    PricingResult calculate(const OptionParams& params) override {
        PricingResult result;
        result.option_price = 10.0;
        result.success = true;
        result.error_message = "";
        result.greeks.delta = 0.5;
        result.greeks.gamma = 0.02;
        result.greeks.theta = -5.0;
        result.greeks.vega = 30.0;
        result.greeks.rho = 40.0;
        return result;
    }
    
    std::string get_model_name() const override {
        return "Mock";
    }
};

int main() {
    std::cout << "验证核心数据结构和接口..." << std::endl;
    
    // 测试OptionParams
    std::cout << "\n1. 测试OptionParams结构..." << std::endl;
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    assert(params.spot_price == 100.0);
    assert(params.strike_price == 105.0);
    assert(params.time_to_maturity == 1.0);
    assert(params.risk_free_rate == 0.05);
    assert(params.volatility == 0.2);
    assert(params.option_type == OptionType::Call);
    std::cout << "   ✓ OptionParams结构正常" << std::endl;
    
    // 测试Greeks
    std::cout << "\n2. 测试Greeks结构..." << std::endl;
    Greeks greeks;
    greeks.delta = 0.5596;
    greeks.gamma = 0.0184;
    greeks.theta = -6.4135;
    greeks.vega = 36.7311;
    greeks.rho = 51.8644;
    
    assert(greeks.delta == 0.5596);
    assert(greeks.gamma == 0.0184);
    assert(greeks.theta == -6.4135);
    assert(greeks.vega == 36.7311);
    assert(greeks.rho == 51.8644);
    std::cout << "   ✓ Greeks结构正常" << std::endl;
    
    // 测试PricingResult
    std::cout << "\n3. 测试PricingResult结构..." << std::endl;
    PricingResult result;
    result.option_price = 8.9162;
    result.success = true;
    result.error_message = "";
    result.greeks = greeks;
    
    assert(result.option_price == 8.9162);
    assert(result.success == true);
    assert(result.error_message == "");
    assert(result.greeks.delta == 0.5596);
    std::cout << "   ✓ PricingResult结构正常" << std::endl;
    
    // 测试OptionType枚举
    std::cout << "\n4. 测试OptionType枚举..." << std::endl;
    OptionType call = OptionType::Call;
    OptionType put = OptionType::Put;
    
    assert(call != put);
    assert(call == OptionType::Call);
    assert(put == OptionType::Put);
    std::cout << "   ✓ OptionType枚举正常" << std::endl;
    
    // 测试IPricingEngine接口
    std::cout << "\n5. 测试IPricingEngine接口..." << std::endl;
    MockPricingEngine engine;
    
    PricingResult engine_result = engine.calculate(params);
    
    assert(engine_result.success == true);
    assert(engine_result.option_price == 10.0);
    assert(engine.get_model_name() == "Mock");
    assert(engine_result.greeks.delta == 0.5);
    assert(engine_result.greeks.gamma == 0.02);
    assert(engine_result.greeks.theta == -5.0);
    assert(engine_result.greeks.vega == 30.0);
    assert(engine_result.greeks.rho == 40.0);
    std::cout << "   ✓ IPricingEngine接口正常" << std::endl;
    
    std::cout << "\n✅ 所有核心数据结构和接口验证通过！" << std::endl;
    
    return 0;
}
