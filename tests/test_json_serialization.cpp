#include <gtest/gtest.h>
#include "json_serialization.h"
#include <nlohmann/json.hpp>
#include <cmath>

using namespace option_pricer;
using json = nlohmann::json;

// 测试OptionParams反序列化
TEST(JsonSerializationTest, DeserializeOptionParams_ValidInput) {
    json j = {
        {"spot_price", 100.0},
        {"strike_price", 105.0},
        {"time_to_maturity", 1.0},
        {"risk_free_rate", 0.05},
        {"volatility", 0.2},
        {"option_type", "call"}
    };
    
    OptionParams params = JsonSerializer::deserialize_option_params(j);
    
    EXPECT_DOUBLE_EQ(params.spot_price, 100.0);
    EXPECT_DOUBLE_EQ(params.strike_price, 105.0);
    EXPECT_DOUBLE_EQ(params.time_to_maturity, 1.0);
    EXPECT_DOUBLE_EQ(params.risk_free_rate, 0.05);
    EXPECT_DOUBLE_EQ(params.volatility, 0.2);
    EXPECT_EQ(params.option_type, OptionType::Call);
}

TEST(JsonSerializationTest, DeserializeOptionParams_PutOption) {
    json j = {
        {"spot_price", 50.0},
        {"strike_price", 55.0},
        {"time_to_maturity", 0.5},
        {"risk_free_rate", 0.03},
        {"volatility", 0.3},
        {"option_type", "put"}
    };
    
    OptionParams params = JsonSerializer::deserialize_option_params(j);
    
    EXPECT_EQ(params.option_type, OptionType::Put);
}

TEST(JsonSerializationTest, DeserializeOptionParams_CaseInsensitive) {
    json j1 = {
        {"spot_price", 100.0},
        {"strike_price", 105.0},
        {"time_to_maturity", 1.0},
        {"risk_free_rate", 0.05},
        {"volatility", 0.2},
        {"option_type", "Call"}
    };
    
    OptionParams params1 = JsonSerializer::deserialize_option_params(j1);
    EXPECT_EQ(params1.option_type, OptionType::Call);
    
    json j2 = {
        {"spot_price", 100.0},
        {"strike_price", 105.0},
        {"time_to_maturity", 1.0},
        {"risk_free_rate", 0.05},
        {"volatility", 0.2},
        {"option_type", "PUT"}
    };
    
    OptionParams params2 = JsonSerializer::deserialize_option_params(j2);
    EXPECT_EQ(params2.option_type, OptionType::Put);
}

TEST(JsonSerializationTest, DeserializeOptionParams_MissingField) {
    json j = {
        {"spot_price", 100.0},
        {"strike_price", 105.0},
        // Missing time_to_maturity
        {"risk_free_rate", 0.05},
        {"volatility", 0.2},
        {"option_type", "call"}
    };
    
    EXPECT_THROW(JsonSerializer::deserialize_option_params(j), std::invalid_argument);
}

TEST(JsonSerializationTest, DeserializeOptionParams_InvalidOptionType) {
    json j = {
        {"spot_price", 100.0},
        {"strike_price", 105.0},
        {"time_to_maturity", 1.0},
        {"risk_free_rate", 0.05},
        {"volatility", 0.2},
        {"option_type", "invalid"}
    };
    
    EXPECT_THROW(JsonSerializer::deserialize_option_params(j), std::invalid_argument);
}

// 测试Greeks序列化
TEST(JsonSerializationTest, SerializeGreeks) {
    Greeks greeks;
    greeks.delta = 0.55963;
    greeks.gamma = 0.01842;
    greeks.theta = -6.41354;
    greeks.vega = 36.73112;
    greeks.rho = 51.86443;
    
    json j = JsonSerializer::serialize_greeks(greeks);
    
    EXPECT_DOUBLE_EQ(j["delta"].get<double>(), 0.5596);
    EXPECT_DOUBLE_EQ(j["gamma"].get<double>(), 0.0184);
    EXPECT_DOUBLE_EQ(j["theta"].get<double>(), -6.4135);
    EXPECT_DOUBLE_EQ(j["vega"].get<double>(), 36.7311);
    EXPECT_DOUBLE_EQ(j["rho"].get<double>(), 51.8644);
}

TEST(JsonSerializationTest, SerializeGreeks_FourDecimalPrecision) {
    Greeks greeks;
    greeks.delta = 0.123456789;
    greeks.gamma = 1.987654321;
    greeks.theta = -0.555555555;
    greeks.vega = 99.999999;
    greeks.rho = 0.000012345;
    
    json j = JsonSerializer::serialize_greeks(greeks);
    
    // 验证精确到四位小数
    EXPECT_DOUBLE_EQ(j["delta"].get<double>(), 0.1235);
    EXPECT_DOUBLE_EQ(j["gamma"].get<double>(), 1.9877);
    EXPECT_DOUBLE_EQ(j["theta"].get<double>(), -0.5556);
    EXPECT_DOUBLE_EQ(j["vega"].get<double>(), 100.0);
    EXPECT_DOUBLE_EQ(j["rho"].get<double>(), 0.0);
}

// 测试PricingResult序列化
TEST(JsonSerializationTest, SerializePricingResult_Success) {
    PricingResult result;
    result.success = true;
    result.option_price = 8.91623;
    result.greeks.delta = 0.5596;
    result.greeks.gamma = 0.0184;
    result.greeks.theta = -6.4135;
    result.greeks.vega = 36.7311;
    result.greeks.rho = 51.8644;
    
    json j = JsonSerializer::serialize_pricing_result(result, "Black-Scholes", 12.5);
    
    EXPECT_TRUE(j["success"].get<bool>());
    EXPECT_DOUBLE_EQ(j["option_price"].get<double>(), 8.9162);
    EXPECT_EQ(j["model"].get<std::string>(), "Black-Scholes");
    EXPECT_DOUBLE_EQ(j["computation_time_ms"].get<double>(), 12.5);
    
    EXPECT_TRUE(j.contains("greeks"));
    EXPECT_DOUBLE_EQ(j["greeks"]["delta"].get<double>(), 0.5596);
}

TEST(JsonSerializationTest, SerializePricingResult_WithWarnings) {
    PricingResult result;
    result.success = true;
    result.option_price = 10.0;
    result.greeks.delta = 0.5;
    result.greeks.gamma = 0.02;
    result.greeks.theta = -5.0;
    result.greeks.vega = 30.0;
    result.greeks.rho = 40.0;
    result.warnings = {"High volatility detected", "Long time to maturity"};
    
    json j = JsonSerializer::serialize_pricing_result(result);
    
    EXPECT_TRUE(j["success"].get<bool>());
    EXPECT_TRUE(j.contains("warnings"));
    EXPECT_EQ(j["warnings"].size(), 2);
    EXPECT_EQ(j["warnings"][0].get<std::string>(), "High volatility detected");
}

TEST(JsonSerializationTest, SerializePricingResult_Failure) {
    PricingResult result;
    result.success = false;
    result.error_message = "Invalid input parameters";
    
    json j = JsonSerializer::serialize_pricing_result(result);
    
    EXPECT_FALSE(j["success"].get<bool>());
    EXPECT_EQ(j["error"].get<std::string>(), "Invalid input parameters");
    EXPECT_FALSE(j.contains("option_price"));
    EXPECT_FALSE(j.contains("greeks"));
}

// 测试ChartData序列化
TEST(JsonSerializationTest, SerializeChartData) {
    VisualizationGenerator::ChartData data;
    data.label = "Option Price";
    data.x_values = {80.0, 90.0, 100.0, 110.0, 120.0};
    data.y_values = {0.5234, 2.4567, 8.9162, 20.1234, 35.6789};
    
    json j = JsonSerializer::serialize_chart_data(data);
    
    EXPECT_EQ(j["label"].get<std::string>(), "Option Price");
    EXPECT_EQ(j["x"].size(), 5);
    EXPECT_EQ(j["y"].size(), 5);
    
    // 验证四位小数精度
    EXPECT_DOUBLE_EQ(j["x"][0].get<double>(), 80.0);
    EXPECT_DOUBLE_EQ(j["y"][0].get<double>(), 0.5234);
    EXPECT_DOUBLE_EQ(j["y"][2].get<double>(), 8.9162);
}

// 测试可视化数据序列化
TEST(JsonSerializationTest, SerializeVisualizationData) {
    VisualizationGenerator::ChartData price_curve;
    price_curve.label = "Price";
    price_curve.x_values = {80.0, 90.0, 100.0};
    price_curve.y_values = {0.5, 2.5, 8.9};
    
    std::vector<VisualizationGenerator::ChartData> greeks_curves;
    
    VisualizationGenerator::ChartData delta_curve;
    delta_curve.label = "Delta";
    delta_curve.x_values = {80.0, 90.0, 100.0};
    delta_curve.y_values = {0.2, 0.4, 0.6};
    greeks_curves.push_back(delta_curve);
    
    VisualizationGenerator::ChartData gamma_curve;
    gamma_curve.label = "Gamma";
    gamma_curve.x_values = {80.0, 90.0, 100.0};
    gamma_curve.y_values = {0.01, 0.02, 0.015};
    greeks_curves.push_back(gamma_curve);
    
    json j = JsonSerializer::serialize_visualization_data(price_curve, greeks_curves);
    
    EXPECT_TRUE(j.contains("price_curve"));
    EXPECT_TRUE(j.contains("greeks_curves"));
    
    EXPECT_EQ(j["price_curve"]["label"].get<std::string>(), "Price");
    EXPECT_EQ(j["price_curve"]["x"].size(), 3);
    
    EXPECT_TRUE(j["greeks_curves"].contains("delta"));
    EXPECT_TRUE(j["greeks_curves"].contains("gamma"));
    EXPECT_EQ(j["greeks_curves"]["delta"]["x"].size(), 3);
}

// 测试错误响应序列化
TEST(JsonSerializationTest, SerializeErrorResponse_Simple) {
    json j = JsonSerializer::serialize_error_response("Invalid input parameters");
    
    EXPECT_FALSE(j["success"].get<bool>());
    EXPECT_EQ(j["error"].get<std::string>(), "Invalid input parameters");
    EXPECT_FALSE(j.contains("details"));
}

TEST(JsonSerializationTest, SerializeErrorResponse_WithDetails) {
    std::vector<std::string> details = {
        "Spot price must be positive",
        "Volatility must be non-negative"
    };
    
    json j = JsonSerializer::serialize_error_response("Invalid input parameters", details);
    
    EXPECT_FALSE(j["success"].get<bool>());
    EXPECT_EQ(j["error"].get<std::string>(), "Invalid input parameters");
    EXPECT_TRUE(j.contains("details"));
    EXPECT_EQ(j["details"].size(), 2);
    EXPECT_EQ(j["details"][0].get<std::string>(), "Spot price must be positive");
}

// 测试特殊数值处理
TEST(JsonSerializationTest, SerializeGreeks_SpecialValues) {
    Greeks greeks;
    greeks.delta = std::nan("");
    greeks.gamma = std::numeric_limits<double>::infinity();
    greeks.theta = -std::numeric_limits<double>::infinity();
    greeks.vega = 0.0;
    greeks.rho = -0.0;
    
    json j = JsonSerializer::serialize_greeks(greeks);
    
    // NaN和Inf应该保持原样（JSON会将它们序列化为null或特殊值）
    EXPECT_TRUE(std::isnan(j["delta"].get<double>()) || j["delta"].is_null());
    EXPECT_DOUBLE_EQ(j["vega"].get<double>(), 0.0);
}
