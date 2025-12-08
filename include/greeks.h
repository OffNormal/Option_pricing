#ifndef GREEKS_H
#define GREEKS_H

namespace option_pricer {

// 希腊值结构
struct Greeks {
    double delta;  // Delta: 期权价格对标的资产价格的敏感度
    double gamma;  // Gamma: Delta对标的资产价格的敏感度
    double theta;  // Theta: 期权价格对时间的敏感度
    double vega;   // Vega: 期权价格对波动率的敏感度
    double rho;    // Rho: 期权价格对无风险利率的敏感度
};

} // namespace option_pricer

#endif // GREEKS_H
