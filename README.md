# OptionPricer - 期权定价平台

一个基于C++的高性能期权定价系统，支持Black-Scholes模型和二叉树模型。

## 项目状态

当前正在开发中。项目结构已建立，核心功能将在后续任务中实现。

## 项目结构

```
.
├── src/           # 源代码文件
├── include/       # 头文件
├── tests/         # 测试文件
├── web/           # Web前端资源
├── CMakeLists.txt # CMake构建配置
└── README.md      # 项目文档
```

## 构建说明

### 依赖项

- CMake 3.15+
- C++17兼容的编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- nlohmann/json (自动下载)
- Google Test (可选，用于测试)

### 编译步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
cmake --build .

# 运行
./option_pricer --port 8080
```

### Windows编译

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\Release\option_pricer.exe --port 8080
```

## 功能特性（计划中）

- ✅ 项目结构和构建系统
- ✅ Black-Scholes期权定价模型
- ✅ 二叉树期权定价模型
- ✅ 希腊值计算（解析解和数值方法）
- ✅ 输入验证和错误处理
- ✅ 异常处理和错误日志记录
- ✅ 极端参数值检测和警告
- ⏳ Web界面和可视化
- ⏳ RESTful API

## 错误处理特性

系统实现了全面的错误处理和日志记录机制：

### 异常处理
- 捕获并处理数值溢出/下溢异常
- 处理除零错误和无效数学运算
- 检测非有限值（NaN、Infinity）
- 提供详细的错误消息

### 极端值检测
系统会对以下极端参数值发出警告：
- 波动率 > 200%
- 到期时间 > 10年
- 价格比率 (S/K) > 10 或 < 0.1
- 利率 > 50% 或 < -10%

### 错误日志
所有错误和警告都会记录到标准错误输出，包含：
- 时间戳（精确到毫秒）
- 日志级别（INFO/WARNING/ERROR）
- 上下文信息（引擎名称）
- 详细的错误描述

## 许可证

待定
