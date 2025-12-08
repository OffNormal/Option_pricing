#include <gtest/gtest.h>
#include "black_scholes_engine.h"
#include "binomial_tree_engine.h"
#include "option_params.h"
#include "math_utils.h"
#include <chrono>
#include <iostream>

using namespace option_pricer;

// 性能测试：Black-Scholes计算时间应小于100ms
TEST(PerformanceTest, BlackScholesSpeed) {
    BlackScholesEngine engine;
    
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    // 预热：运行一次以初始化CDF查找表
    engine.calculate(params);
    
    // 测试多次计算的平均时间
    const int num_iterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        auto result = engine.calculate(params);
        ASSERT_TRUE(result.success);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ms = duration.count() / 1000.0 / num_iterations;
    
    std::cout << "Black-Scholes average calculation time: " 
              << avg_time_ms << " ms (over " << num_iterations << " iterations)" << std::endl;
    
    // 需求：单次计算应小于100ms
    EXPECT_LT(avg_time_ms, 100.0) << "Black-Scholes calculation too slow";
    
    // 实际上应该远快于100ms，我们期望小于1ms
    EXPECT_LT(avg_time_ms, 1.0) << "Black-Scholes calculation slower than expected";
}

// 性能测试：二叉树（100步）计算时间应小于500ms
TEST(PerformanceTest, BinomialTree100StepsSpeed) {
    BinomialTreeEngine engine(100);
    
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    // 测试多次计算的平均时间
    const int num_iterations = 100;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        auto result = engine.calculate(params);
        ASSERT_TRUE(result.success);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ms = duration.count() / 1000.0 / num_iterations;
    
    std::cout << "Binomial Tree (100 steps) average calculation time: " 
              << avg_time_ms << " ms (over " << num_iterations << " iterations)" << std::endl;
    
    // 需求：单次计算应小于500ms
    EXPECT_LT(avg_time_ms, 500.0) << "Binomial Tree (100 steps) calculation too slow";
    
    // 实际上应该远快于500ms，我们期望小于50ms
    EXPECT_LT(avg_time_ms, 50.0) << "Binomial Tree (100 steps) calculation slower than expected";
}

// 性能测试：二叉树（500步）计算时间应小于1秒
TEST(PerformanceTest, BinomialTree500StepsSpeed) {
    BinomialTreeEngine engine(500);
    
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    // 测试多次计算的平均时间
    const int num_iterations = 10;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        auto result = engine.calculate(params);
        ASSERT_TRUE(result.success);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_ms = duration.count() / 1000.0 / num_iterations;
    
    std::cout << "Binomial Tree (500 steps) average calculation time: " 
              << avg_time_ms << " ms (over " << num_iterations << " iterations)" << std::endl;
    
    // 需求：单次计算应小于1000ms
    EXPECT_LT(avg_time_ms, 1000.0) << "Binomial Tree (500 steps) calculation too slow";
    
    // 实际上应该远快于1000ms，我们期望小于200ms
    EXPECT_LT(avg_time_ms, 200.0) << "Binomial Tree (500 steps) calculation slower than expected";
}

// 性能对比测试：验证优化后的性能提升
TEST(PerformanceTest, OptimizationComparison) {
    BlackScholesEngine bs_engine;
    BinomialTreeEngine bt_engine_100(100);
    BinomialTreeEngine bt_engine_500(500);
    
    OptionParams params;
    params.spot_price = 100.0;
    params.strike_price = 105.0;
    params.time_to_maturity = 1.0;
    params.risk_free_rate = 0.05;
    params.volatility = 0.2;
    params.option_type = OptionType::Call;
    
    // Black-Scholes性能
    auto start_bs = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        bs_engine.calculate(params);
    }
    auto end_bs = std::chrono::high_resolution_clock::now();
    auto duration_bs = std::chrono::duration_cast<std::chrono::microseconds>(end_bs - start_bs);
    
    // 二叉树100步性能
    auto start_bt100 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        bt_engine_100.calculate(params);
    }
    auto end_bt100 = std::chrono::high_resolution_clock::now();
    auto duration_bt100 = std::chrono::duration_cast<std::chrono::microseconds>(end_bt100 - start_bt100);
    
    // 二叉树500步性能
    auto start_bt500 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; ++i) {
        bt_engine_500.calculate(params);
    }
    auto end_bt500 = std::chrono::high_resolution_clock::now();
    auto duration_bt500 = std::chrono::duration_cast<std::chrono::microseconds>(end_bt500 - start_bt500);
    
    std::cout << "\n=== Performance Comparison ===" << std::endl;
    std::cout << "Black-Scholes (1000 iterations): " << duration_bs.count() / 1000.0 << " ms" << std::endl;
    std::cout << "  Average per iteration: " << duration_bs.count() / 1000.0 / 1000.0 << " ms" << std::endl;
    std::cout << "Binomial Tree 100 steps (100 iterations): " << duration_bt100.count() / 1000.0 << " ms" << std::endl;
    std::cout << "  Average per iteration: " << duration_bt100.count() / 1000.0 / 100.0 << " ms" << std::endl;
    std::cout << "Binomial Tree 500 steps (10 iterations): " << duration_bt500.count() / 1000.0 << " ms" << std::endl;
    std::cout << "  Average per iteration: " << duration_bt500.count() / 1000.0 / 10.0 << " ms" << std::endl;
    std::cout << "==============================\n" << std::endl;
    
    // 验证Black-Scholes比二叉树快
    EXPECT_LT(duration_bs.count() / 1000.0, duration_bt100.count() / 100.0) 
        << "Black-Scholes should be faster than Binomial Tree";
}

// 测试CDF查找表的准确性
TEST(PerformanceTest, CDFLookupTableAccuracy) {
    // 测试查找表在常见范围内的准确性
    for (double x = -5.0; x <= 5.0; x += 0.1) {
        double exact = option_pricer::math::normal_cdf(x);
        double fast = option_pricer::math::normal_cdf_fast(x);
        
        // 查找表应该在0.0001的误差范围内
        EXPECT_NEAR(exact, fast, 0.0001) 
            << "CDF lookup table inaccurate at x=" << x;
    }
}
