#include <gtest/gtest.h>
#include "option_params.h"
#include "pricing_result.h"
#include "binomial_tree_engine.h"
#include <future>
#include <chrono>

using namespace option_pricer;

// Test timeout control functionality
class TimeoutControlTest : public ::testing::Test {
protected:
    OptionParams create_test_params() {
        OptionParams params;
        params.spot_price = 100.0;
        params.strike_price = 100.0;
        params.time_to_maturity = 1.0;
        params.risk_free_rate = 0.05;
        params.volatility = 0.2;
        params.option_type = OptionType::Call;
        return params;
    }
};

// Test that normal calculations complete within timeout
TEST_F(TimeoutControlTest, NormalCalculationCompletesWithinTimeout) {
    auto params = create_test_params();
    BinomialTreeEngine engine(100);  // 100 steps should complete quickly
    
    constexpr std::chrono::seconds TIMEOUT{5};
    
    // Execute calculation with timeout
    std::future<PricingResult> future = std::async(
        std::launch::async,
        [&engine, &params]() {
            return engine.calculate(params);
        }
    );
    
    // Wait for result with timeout
    auto status = future.wait_for(TIMEOUT);
    
    // Should complete within timeout
    ASSERT_EQ(status, std::future_status::ready);
    
    // Get result and verify success
    PricingResult result = future.get();
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.option_price, 0.0);
}

// Test that very large step counts might approach timeout
TEST_F(TimeoutControlTest, LargeStepCountTakesLonger) {
    auto params = create_test_params();
    
    // Measure time for small step count
    BinomialTreeEngine small_engine(50);
    auto start_small = std::chrono::high_resolution_clock::now();
    PricingResult result_small = small_engine.calculate(params);
    auto end_small = std::chrono::high_resolution_clock::now();
    auto duration_small = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_small - start_small
    ).count();
    
    // Measure time for large step count
    BinomialTreeEngine large_engine(1000);
    auto start_large = std::chrono::high_resolution_clock::now();
    PricingResult result_large = large_engine.calculate(params);
    auto end_large = std::chrono::high_resolution_clock::now();
    auto duration_large = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_large - start_large
    ).count();
    
    // Both should succeed
    EXPECT_TRUE(result_small.success);
    EXPECT_TRUE(result_large.success);
    
    // Large step count should take longer
    EXPECT_GT(duration_large, duration_small);
    
    // But both should complete well within 5 seconds
    EXPECT_LT(duration_small, 5000);
    EXPECT_LT(duration_large, 5000);
}

// Test timeout detection mechanism
TEST_F(TimeoutControlTest, TimeoutDetectionMechanism) {
    constexpr std::chrono::milliseconds SHORT_TIMEOUT{100};
    
    // Create a future that simulates a long-running calculation
    std::future<int> future = std::async(
        std::launch::async,
        []() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return 42;
        }
    );
    
    // Wait with short timeout
    auto status = future.wait_for(SHORT_TIMEOUT);
    
    // Should timeout
    EXPECT_EQ(status, std::future_status::timeout);
}

// Test that async execution works correctly
TEST_F(TimeoutControlTest, AsyncExecutionWorks) {
    auto params = create_test_params();
    BinomialTreeEngine engine(100);
    
    // Execute asynchronously
    std::future<PricingResult> future = std::async(
        std::launch::async,
        [&engine, &params]() {
            return engine.calculate(params);
        }
    );
    
    // Get result
    PricingResult result = future.get();
    
    // Verify result
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.option_price, 0.0);
    EXPECT_FALSE(std::isnan(result.option_price));
    EXPECT_FALSE(std::isinf(result.option_price));
}
