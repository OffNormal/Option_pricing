#include "math_utils.h"
#include <cmath>
#include <limits>

namespace option_pricer {
namespace math {

// 常量定义
constexpr double INV_SQRT_2PI = 0.3989422804014327;  // 1/sqrt(2*pi)
constexpr double SQRT_2 = 1.4142135623730951;        // sqrt(2)

/**
 * 标准正态分布累积分布函数 (CDF)
 * 使用 Abramowitz and Stegun 近似公式 (误差 < 7.5e-8)
 */
double normal_cdf(double x) {
    // 处理极端值以确保数值稳定性
    if (x < -10.0) {
        return 0.0;
    }
    if (x > 10.0) {
        return 1.0;
    }
    
    // 处理 NaN 和无穷大
    if (std::isnan(x)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (std::isinf(x)) {
        return x > 0 ? 1.0 : 0.0;
    }
    
    // 使用误差函数 erf 计算 CDF
    // CDF(x) = 0.5 * (1 + erf(x/sqrt(2)))
    return 0.5 * (1.0 + std::erf(x / SQRT_2));
}

/**
 * 标准正态分布概率密度函数 (PDF)
 * f(x) = (1/sqrt(2π)) * exp(-x²/2)
 */
double normal_pdf(double x) {
    // 处理 NaN
    if (std::isnan(x)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    
    // 处理无穷大
    if (std::isinf(x)) {
        return 0.0;
    }
    
    // 对于极端值，PDF 接近 0
    if (std::abs(x) > 40.0) {
        return 0.0;
    }
    
    // 计算 PDF，使用数值稳定的方式
    // 避免直接计算 exp(-x²/2) 可能导致的溢出
    double x_squared = x * x;
    
    // 如果 x² 太大，直接返回 0
    if (x_squared > 700.0) {  // exp(-350) 已经非常接近 0
        return 0.0;
    }
    
    return INV_SQRT_2PI * std::exp(-0.5 * x_squared);
}

} // namespace math
} // namespace option_pricer
