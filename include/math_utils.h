#ifndef MATH_UTILS_H
#define MATH_UTILS_H

namespace option_pricer {
namespace math {

/**
 * 标准正态分布累积分布函数 (CDF)
 * 计算 P(X <= x)，其中 X ~ N(0,1)
 * 
 * 使用 Abramowitz and Stegun 近似公式，确保数值稳定性
 * 对于极端值使用渐近展开
 * 
 * @param x 输入值
 * @return 累积概率，范围 [0, 1]
 */
double normal_cdf(double x);

/**
 * 标准正态分布累积分布函数 (CDF) - 使用查表法优化
 * 对于常见范围内的值使用预计算的查找表，提高性能
 * 对于超出范围的值回退到精确计算
 * 
 * @param x 输入值
 * @return 累积概率，范围 [0, 1]
 */
double normal_cdf_fast(double x);

/**
 * 标准正态分布概率密度函数 (PDF)
 * 计算 f(x) = (1/sqrt(2π)) * exp(-x²/2)
 * 
 * 使用数值稳定的实现，避免溢出
 * 
 * @param x 输入值
 * @return 概率密度值
 */
double normal_pdf(double x);

} // namespace math
} // namespace option_pricer

#endif // MATH_UTILS_H
