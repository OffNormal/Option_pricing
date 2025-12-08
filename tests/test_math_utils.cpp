#include <gtest/gtest.h>
#include "math_utils.h"
#include <cmath>
#include <limits>

using namespace option_pricer::math;

// 测试标准正态分布CDF函数
TEST(MathUtilsTest, NormalCDF_KnownValues) {
    // 测试已知值
    EXPECT_NEAR(normal_cdf(0.0), 0.5, 1e-6);
    EXPECT_NEAR(normal_cdf(1.0), 0.8413447, 1e-6);
    EXPECT_NEAR(normal_cdf(-1.0), 0.1586553, 1e-6);
    EXPECT_NEAR(normal_cdf(2.0), 0.9772499, 1e-6);
    EXPECT_NEAR(normal_cdf(-2.0), 0.0227501, 1e-6);
}

// 测试CDF边界情况
TEST(MathUtilsTest, NormalCDF_BoundaryValues) {
    // 极端正值应该接近1
    EXPECT_NEAR(normal_cdf(10.0), 1.0, 1e-10);
    EXPECT_NEAR(normal_cdf(20.0), 1.0, 1e-10);
    
    // 极端负值应该接近0
    EXPECT_NEAR(normal_cdf(-10.0), 0.0, 1e-10);
    EXPECT_NEAR(normal_cdf(-20.0), 0.0, 1e-10);
}

// 测试CDF对称性
TEST(MathUtilsTest, NormalCDF_Symmetry) {
    // CDF(-x) + CDF(x) = 1
    for (double x = 0.0; x <= 5.0; x += 0.5) {
        EXPECT_NEAR(normal_cdf(-x) + normal_cdf(x), 1.0, 1e-10);
    }
}

// 测试CDF单调性
TEST(MathUtilsTest, NormalCDF_Monotonicity) {
    // CDF应该是单调递增的
    double prev = normal_cdf(-5.0);
    for (double x = -4.9; x <= 5.0; x += 0.1) {
        double curr = normal_cdf(x);
        EXPECT_GE(curr, prev);
        prev = curr;
    }
}

// 测试CDF处理特殊值
TEST(MathUtilsTest, NormalCDF_SpecialValues) {
    // 测试NaN
    EXPECT_TRUE(std::isnan(normal_cdf(std::numeric_limits<double>::quiet_NaN())));
    
    // 测试无穷大
    EXPECT_EQ(normal_cdf(std::numeric_limits<double>::infinity()), 1.0);
    EXPECT_EQ(normal_cdf(-std::numeric_limits<double>::infinity()), 0.0);
}

// 测试标准正态分布PDF函数
TEST(MathUtilsTest, NormalPDF_KnownValues) {
    // 测试已知值
    EXPECT_NEAR(normal_pdf(0.0), 0.3989423, 1e-6);  // 1/sqrt(2*pi)
    EXPECT_NEAR(normal_pdf(1.0), 0.2419707, 1e-6);
    EXPECT_NEAR(normal_pdf(-1.0), 0.2419707, 1e-6);
    EXPECT_NEAR(normal_pdf(2.0), 0.0539910, 1e-6);
}

// 测试PDF对称性
TEST(MathUtilsTest, NormalPDF_Symmetry) {
    // PDF(-x) = PDF(x)
    for (double x = 0.0; x <= 5.0; x += 0.5) {
        EXPECT_NEAR(normal_pdf(-x), normal_pdf(x), 1e-10);
    }
}

// 测试PDF非负性
TEST(MathUtilsTest, NormalPDF_NonNegativity) {
    // PDF应该始终非负
    for (double x = -10.0; x <= 10.0; x += 0.5) {
        EXPECT_GE(normal_pdf(x), 0.0);
    }
}

// 测试PDF在极端值处接近0
TEST(MathUtilsTest, NormalPDF_ExtremeValues) {
    EXPECT_NEAR(normal_pdf(10.0), 0.0, 1e-10);
    EXPECT_NEAR(normal_pdf(-10.0), 0.0, 1e-10);
    EXPECT_NEAR(normal_pdf(40.0), 0.0, 1e-10);
}

// 测试PDF处理特殊值
TEST(MathUtilsTest, NormalPDF_SpecialValues) {
    // 测试NaN
    EXPECT_TRUE(std::isnan(normal_pdf(std::numeric_limits<double>::quiet_NaN())));
    
    // 测试无穷大
    EXPECT_EQ(normal_pdf(std::numeric_limits<double>::infinity()), 0.0);
    EXPECT_EQ(normal_pdf(-std::numeric_limits<double>::infinity()), 0.0);
}

// 测试PDF最大值在x=0处
TEST(MathUtilsTest, NormalPDF_MaximumAtZero) {
    double max_value = normal_pdf(0.0);
    for (double x = -5.0; x <= 5.0; x += 0.1) {
        if (x != 0.0) {
            EXPECT_LE(normal_pdf(x), max_value);
        }
    }
}

// 测试CDF和PDF的关系（数值积分验证）
TEST(MathUtilsTest, CDF_PDF_Relationship) {
    // 使用简单的梯形法则验证 CDF'(x) ≈ PDF(x)
    double h = 0.001;  // 步长
    for (double x = -3.0; x <= 3.0; x += 0.5) {
        double numerical_derivative = (normal_cdf(x + h) - normal_cdf(x - h)) / (2 * h);
        EXPECT_NEAR(numerical_derivative, normal_pdf(x), 1e-4);
    }
}
