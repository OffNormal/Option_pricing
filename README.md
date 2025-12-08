# OptionPricer - 期权定价平台

<div align="center">

**一个基于C++的高性能期权定价系统，支持Black-Scholes模型和二叉树模型**

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

</div>

## 📋 目录

- [简介](#简介)
- [功能特性](#功能特性)
- [快速开始](#快速开始)
- [依赖项安装](#依赖项安装)
- [编译和运行](#编译和运行)
- [使用指南](#使用指南)
- [API文档](#api文档)
- [项目结构](#项目结构)
- [测试](#测试)
- [性能](#性能)
- [错误处理](#错误处理)
- [贡献指南](#贡献指南)
- [许可证](#许可证)

## 简介

OptionPricer是一个专业的期权定价平台，为金融从业者、量化分析师和研究人员提供准确、快速的期权价格计算功能。系统采用C++17实现核心计算引擎，确保高性能和数值稳定性，并通过现代化的Web界面提供直观的用户体验。

### 核心优势

- 🚀 **高性能**: Black-Scholes计算 < 100ms，二叉树模型（100步）< 500ms
- 📊 **双模型支持**: Black-Scholes解析解 + 二叉树数值方法
- 📈 **完整希腊值**: Delta、Gamma、Theta、Vega、Rho全覆盖
- 🎨 **可视化**: 交互式图表展示期权价格和希腊值曲线
- 🛡️ **健壮性**: 全面的输入验证、异常处理和错误日志
- 🌐 **易用性**: RESTful API + 响应式Web界面

## 功能特性

### ✅ 已实现功能

- **定价模型**
  - Black-Scholes解析解模型（欧式期权）
  - 二叉树数值模型（支持10-1000步）
  - 看涨期权和看跌期权定价

- **希腊值计算**
  - Black-Scholes解析公式计算
  - 二叉树数值方法（有限差分）
  - 五个核心希腊值：Delta、Gamma、Theta、Vega、Rho

- **输入验证**
  - 参数有效性检查（正数、范围验证）
  - 详细错误消息提示
  - 极端值检测和警告

- **Web界面**
  - 响应式设计，支持移动端
  - 实时表单验证
  - 结果展示（价格、希腊值、图表）
  - Chart.js可视化图表

- **RESTful API**
  - `/api/price` - 期权定价接口
  - `/api/visualize` - 可视化数据生成
  - JSON格式请求/响应
  - CORS支持

- **错误处理**
  - 异常捕获和处理
  - 数值稳定性检测
  - 详细错误日志记录
  - 超时控制（5秒）

## 快速开始

### 前置要求

- **编译器**: 支持C++17的编译器
  - GCC 7+ / Clang 5+ (Linux/macOS)
  - MSVC 2017+ (Windows)
- **构建工具**: CMake 3.15+
- **网络**: 首次构建需要下载依赖项

### 5分钟快速启动

```bash
# 1. 克隆仓库
git clone https://github.com/yourusername/option-pricer.git
cd option-pricer

# 2. 创建构建目录
mkdir build && cd build

# 3. 配置和编译
cmake ..
cmake --build . --config Release

# 4. 运行服务器
./option_pricer --port 8080

# 5. 打开浏览器访问
# http://localhost:8080
```

就这么简单！现在您可以在浏览器中使用期权定价功能了。

## 依赖项安装

### 自动依赖管理

项目使用CMake的FetchContent功能自动下载和配置所有依赖项，无需手动安装：

- **nlohmann/json** (v3.11.3) - JSON解析库
- **cpp-httplib** (v0.14.3) - HTTP服务器库
- **Google Test** (v1.14.0) - 单元测试框架（可选）

### 手动安装依赖（可选）

如果您希望使用系统已安装的依赖项：

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    nlohmann-json3-dev \
    libgtest-dev
```

#### macOS

```bash
brew install cmake nlohmann-json googletest
```

#### Windows

使用vcpkg包管理器：

```cmd
vcpkg install nlohmann-json cpp-httplib gtest
```

## 编译和运行

### Linux/macOS编译

```bash
# 标准编译
mkdir build && cd build
cmake ..
make -j$(nproc)

# 运行服务器（默认端口8080）
./option_pricer

# 指定端口运行
./option_pricer --port 9000
```

### Windows编译

```cmd
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译（Release模式）
cmake --build . --config Release

# 运行服务器
.\Release\option_pricer.exe --port 8080
```

### 编译选项

```bash
# Debug模式（包含调试符号）
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release模式（优化性能）
cmake -DCMAKE_BUILD_TYPE=Release ..

# 禁用测试
cmake -DBUILD_TESTING=OFF ..
```

### 运行参数

```bash
./option_pricer [选项]

选项:
  --port <端口号>    指定HTTP服务器端口（默认: 8080）
  --help            显示帮助信息
  --version         显示版本信息
```

## 使用指南

### Web界面使用

1. **启动服务器**
   ```bash
   ./option_pricer --port 8080
   ```

2. **打开浏览器**
   访问 `http://localhost:8080`

3. **输入参数**
   - 选择定价模型（Black-Scholes 或 二叉树）
   - 选择期权类型（看涨 或 看跌）
   - 输入期权参数：
     - 标的资产价格 (S)
     - 执行价格 (K)
     - 到期时间 (T，年)
     - 无风险利率 (r)
     - 波动率 (σ)
   - 如果选择二叉树模型，设置步数（10-1000）

4. **查看结果**
   - 期权价格（精确到4位小数）
   - 五个希腊值
   - 价格曲线图表
   - 希腊值曲线图表

### 示例计算

#### 示例1: 平值看涨期权

```
模型: Black-Scholes
期权类型: 看涨期权
标的资产价格: 100.00
执行价格: 100.00
到期时间: 1.0年
无风险利率: 5%
波动率: 20%

结果:
期权价格: 10.4506
Delta: 0.6368
Gamma: 0.0184
Theta: -6.4135
Vega: 36.7311
Rho: 51.8644
```

#### 示例2: 二叉树模型

```
模型: 二叉树
步数: 100
期权类型: 看跌期权
标的资产价格: 100.00
执行价格: 105.00
到期时间: 0.5年
无风险利率: 3%
波动率: 25%

结果:
期权价格: 8.9234
Delta: -0.4521
Gamma: 0.0231
Theta: -8.1234
Vega: 28.9012
Rho: -20.3456
```

## API文档

### 端点概览

| 端点 | 方法 | 描述 |
|------|------|------|
| `/` | GET | Web界面主页 |
| `/api/price` | POST | 期权定价计算 |
| `/api/visualize` | POST | 生成可视化数据 |

### POST /api/price

计算期权价格和希腊值。

**请求体 (JSON)**

```json
{
  "model": "black-scholes",
  "spot_price": 100.0,
  "strike_price": 105.0,
  "time_to_maturity": 1.0,
  "risk_free_rate": 0.05,
  "volatility": 0.2,
  "option_type": "call",
  "binomial_steps": 100,
  "calculate_greeks": true
}
```

**参数说明**

| 参数 | 类型 | 必需 | 说明 | 范围 |
|------|------|------|------|------|
| `model` | string | 是 | 定价模型 | "black-scholes" 或 "binomial-tree" |
| `spot_price` | number | 是 | 标的资产价格 | > 0 |
| `strike_price` | number | 是 | 执行价格 | > 0 |
| `time_to_maturity` | number | 是 | 到期时间（年） | > 0 |
| `risk_free_rate` | number | 是 | 无风险利率 | 任意有限值 |
| `volatility` | number | 是 | 波动率 | ≥ 0 |
| `option_type` | string | 是 | 期权类型 | "call" 或 "put" |
| `binomial_steps` | integer | 否 | 二叉树步数 | 10-1000（仅二叉树模型） |
| `calculate_greeks` | boolean | 否 | 是否计算希腊值 | 默认true |

**成功响应 (200 OK)**

```json
{
  "success": true,
  "model": "black-scholes",
  "option_price": 8.9162,
  "greeks": {
    "delta": 0.5596,
    "gamma": 0.0184,
    "theta": -6.4135,
    "vega": 36.7311,
    "rho": 51.8644
  },
  "computation_time_ms": 12
}
```

**错误响应 (400 Bad Request)**

```json
{
  "success": false,
  "error": "Invalid input parameters",
  "details": [
    "Spot price must be positive",
    "Volatility must be non-negative"
  ]
}
```

### POST /api/visualize

生成期权价格和希腊值的可视化数据。

**请求体 (JSON)**

```json
{
  "model": "black-scholes",
  "spot_price": 100.0,
  "strike_price": 105.0,
  "time_to_maturity": 1.0,
  "risk_free_rate": 0.05,
  "volatility": 0.2,
  "option_type": "call",
  "spot_min": 80.0,
  "spot_max": 120.0,
  "points": 50
}
```

**响应 (200 OK)**

```json
{
  "success": true,
  "price_curve": {
    "x": [80.0, 81.0, 82.0, ...],
    "y": [0.52, 0.68, 0.87, ...]
  },
  "greeks_curves": {
    "delta": {
      "x": [80.0, 81.0, ...],
      "y": [0.12, 0.15, ...]
    },
    "gamma": { ... },
    "theta": { ... },
    "vega": { ... },
    "rho": { ... }
  }
}
```

### cURL示例

```bash
# 计算期权价格
curl -X POST http://localhost:8080/api/price \
  -H "Content-Type: application/json" \
  -d '{
    "model": "black-scholes",
    "spot_price": 100.0,
    "strike_price": 105.0,
    "time_to_maturity": 1.0,
    "risk_free_rate": 0.05,
    "volatility": 0.2,
    "option_type": "call"
  }'

# 生成可视化数据
curl -X POST http://localhost:8080/api/visualize \
  -H "Content-Type: application/json" \
  -d '{
    "model": "black-scholes",
    "spot_price": 100.0,
    "strike_price": 105.0,
    "time_to_maturity": 1.0,
    "risk_free_rate": 0.05,
    "volatility": 0.2,
    "option_type": "call",
    "spot_min": 80.0,
    "spot_max": 120.0,
    "points": 50
  }'
```

## 项目结构

```
option-pricer/
├── include/                    # 头文件
│   ├── option_params.h        # 期权参数结构
│   ├── pricing_result.h       # 定价结果结构
│   ├── greeks.h               # 希腊值结构
│   ├── pricing_engine.h       # 定价引擎接口
│   ├── black_scholes_engine.h # Black-Scholes引擎
│   ├── binomial_tree_engine.h # 二叉树引擎
│   ├── math_utils.h           # 数学工具函数
│   ├── input_validator.h      # 输入验证器
│   ├── parameter_validator.h  # 参数验证器
│   ├── error_logger.h         # 错误日志记录器
│   ├── json_serialization.h   # JSON序列化
│   ├── visualization_generator.h # 可视化数据生成
│   └── option_pricer_server.h # HTTP服务器
│
├── src/                       # 源文件
│   ├── main.cpp              # 程序入口
│   ├── black_scholes_engine.cpp
│   ├── binomial_tree_engine.cpp
│   ├── math_utils.cpp
│   ├── input_validator.cpp
│   ├── parameter_validator.cpp
│   ├── error_logger.cpp
│   ├── json_serialization.cpp
│   ├── visualization_generator.cpp
│   └── option_pricer_server.cpp
│
├── tests/                     # 测试文件
│   ├── test_core_structures.cpp
│   ├── test_math_utils.cpp
│   ├── test_black_scholes.cpp
│   ├── test_greeks_basic.cpp
│   ├── test_binomial_tree.cpp
│   ├── test_binomial_greeks.cpp
│   ├── test_input_validator.cpp
│   ├── test_error_handling.cpp
│   ├── test_visualization_generator.cpp
│   ├── test_json_serialization.cpp
│   ├── test_timeout_control.cpp
│   └── test_performance.cpp
│
├── web/                       # Web前端资源
│   ├── index.html            # 主页面
│   ├── style.css             # 样式表
│   ├── app.js                # 前端逻辑
│   └── README.md             # 前端文档
│
├── CMakeLists.txt            # CMake配置
└── README.md                 # 项目文档
```

## 测试

### 运行测试

```bash
# 编译测试
cd build
cmake ..
make option_pricer_tests

# 运行所有测试
./option_pricer_tests

# 运行特定测试
./option_pricer_tests --gtest_filter=BlackScholesEngineTest.*
```

### 测试覆盖

项目包含全面的测试套件：

- **单元测试** (Google Test)
  - 数学函数测试（正态分布CDF/PDF）
  - Black-Scholes引擎测试
  - 二叉树引擎测试
  - 希腊值计算测试
  - 输入验证测试
  - JSON序列化测试
  - 错误处理测试

- **集成测试**
  - HTTP API测试
  - 端到端工作流测试

- **性能测试**
  - Black-Scholes性能基准
  - 二叉树性能基准
  - 可视化数据生成性能

### 测试示例

```cpp
// 测试看涨-看跌平价关系
TEST_F(BlackScholesEngineTest, PutCallParity) {
    OptionParams call_params;
    call_params.spot_price = 100.0;
    call_params.strike_price = 105.0;
    call_params.time_to_maturity = 1.0;
    call_params.risk_free_rate = 0.05;
    call_params.volatility = 0.2;
    call_params.option_type = OptionType::Call;
    
    OptionParams put_params = call_params;
    put_params.option_type = OptionType::Put;
    
    PricingResult call_result = engine.calculate(call_params);
    PricingResult put_result = engine.calculate(put_params);
    
    // 验证 C - P = S - K*e^(-rT)
    double left_side = call_result.option_price - put_result.option_price;
    double right_side = call_params.spot_price - 
                       call_params.strike_price * 
                       std::exp(-call_params.risk_free_rate * 
                               call_params.time_to_maturity);
    
    EXPECT_NEAR(left_side, right_side, 0.01);
}
```

## 性能

### 性能指标

| 操作 | 目标时间 | 实际性能 |
|------|----------|----------|
| Black-Scholes计算 | < 100ms | ~10-20ms |
| 二叉树（100步） | < 500ms | ~100-200ms |
| 二叉树（500步） | < 1s | ~400-600ms |
| 可视化数据生成（50点） | < 2s | ~500ms-1s |

### 优化特性

- **编译优化**: `-O3 -march=native -flto`
- **数值优化**: 
  - 高精度正态分布CDF近似
  - 二叉树动态规划避免重复计算
  - 向量预分配减少内存分配
- **并发支持**: HTTP服务器支持多线程请求处理
- **超时控制**: 5秒超时保护，防止长时间计算

## 错误处理

### 输入验证

系统在接收请求后立即进行输入验证：

- ✅ 所有价格参数 > 0
- ✅ 波动率 ≥ 0
- ✅ 到期时间 > 0
- ✅ 无风险利率为有限值
- ✅ 二叉树步数在 [10, 1000] 范围
- ✅ 期权类型为 "call" 或 "put"

验证失败返回HTTP 400和详细错误列表。

### 异常处理

定价引擎捕获并处理：

- 数值溢出/下溢
- 除零错误
- 无效数学运算（如负数平方根）
- 非有限值（NaN、Infinity）

### 极端值警告

系统检测以下极端情况并发出警告：

- 波动率 > 200%
- 到期时间 > 10年
- 价格比率 (S/K) > 10 或 < 0.1
- 利率 > 50% 或 < -10%

警告不阻止计算，但在响应中包含警告消息。

### 错误日志

所有错误和警告记录到标准错误输出：

```
[2024-12-08 10:30:45.123] [ERROR] [BlackScholesEngine] Invalid input: Spot price must be positive (value: -100.0)
[2024-12-08 10:31:12.456] [WARNING] [BinomialTreeEngine] Extreme parameter detected: Volatility = 2.5 (250%)
```

日志包含：
- 时间戳（精确到毫秒）
- 日志级别（INFO/WARNING/ERROR）
- 上下文信息（引擎名称）
- 详细错误描述

## 贡献指南

欢迎贡献！请遵循以下步骤：

1. Fork本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

### 代码规范

- 遵循C++17标准
- 使用4空格缩进
- 添加适当的注释和文档
- 确保所有测试通过
- 保持代码覆盖率 > 80%

## 许可证

本项目采用MIT许可证 - 详见 [LICENSE](LICENSE) 文件

## 联系方式

- 项目主页: https://github.com/yourusername/option-pricer
- 问题反馈: https://github.com/yourusername/option-pricer/issues
- 邮箱: your.email@example.com

## 致谢

- Black-Scholes模型: Fischer Black, Myron Scholes, Robert Merton
- 二叉树模型: John Cox, Stephen Ross, Mark Rubinstein
- 依赖库: nlohmann/json, cpp-httplib, Google Test, Chart.js

---

<div align="center">
Made with ❤️ by the OptionPricer Team
</div>
