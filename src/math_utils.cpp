#include "math_utils.h"
#include <cmath>
#include <limits>
#include <array>
#include <algorithm>

namespace option_pricer {
namespace math {

// 常量定义
constexpr double INV_SQRT_2PI = 0.3989422804014327;  // 1/sqrt(2*pi)
constexpr double SQRT_2 = 1.4142135623730951;        // sqrt(2)

// CDF查找表常量
constexpr int CDF_TABLE_SIZE = 10001;  // 表大小（覆盖-5到5，步长0.001）
constexpr double CDF_TABLE_MIN = -5.0;
constexpr double CDF_TABLE_MAX = 5.0;
constexpr double CDF_TABLE_STEP = (CDF_TABLE_MAX - CDF_TABLE_MIN) / (CDF_TABLE_SIZE - 1);

// CDF查找表（延迟初始化）
static std::array<double, CDF_TABLE_SIZE> cdf_lookup_table;
static bool cdf_table_initialized = false;

// 初始化CDF查找表
static void initialize_cdf_table() {
    if (cdf_table_initialized) {
        return;
    }
    
    for (int i = 0; i < CDF_TABLE_SIZE; ++i) {
        double x = CDF_TABLE_MIN + i * CDF_TABLE_STEP;
        cdf_lookup_table[i] = normal_cdf(x);
    }
    
    cdf_table_initialized = true;
}

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
 * 标准正态分布累积分布函数 (CDF) - 使用查表法优化
 * 对于常见范围内的值使用预计算的查找表，提高性能
 */
double normal_cdf_fast(double x) {
    // 初始化查找表（仅第一次调用时）
    if (!cdf_table_initialized) {
        initialize_cdf_table();
    }
    
    // 处理极端值
    if (x < CDF_TABLE_MIN) {
        return normal_cdf(x);  // 回退到精确计算
    }
    if (x > CDF_TABLE_MAX) {
        return normal_cdf(x);  // 回退到精确计算
    }
    
    // 处理 NaN 和无穷大
    if (std::isnan(x)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (std::isinf(x)) {
        return x > 0 ? 1.0 : 0.0;
    }
    
    // 使用线性插值从查找表中获取值
    double index_float = (x - CDF_TABLE_MIN) / CDF_TABLE_STEP;
    int index_low = static_cast<int>(std::floor(index_float));
    int index_high = index_low + 1;
    
    // 边界检查
    if (index_low < 0) {
        index_low = 0;
    }
    if (index_high >= CDF_TABLE_SIZE) {
        index_high = CDF_TABLE_SIZE - 1;
    }
    
    // 线性插值
    if (index_low == index_high) {
        return cdf_lookup_table[index_low];
    }
    
    double fraction = index_float - index_low;
    double value_low = cdf_lookup_table[index_low];
    double value_high = cdf_lookup_table[index_high];
    
    return value_low + fraction * (value_high - value_low);
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
