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
- ⏳ Black-Scholes期权定价模型
- ⏳ 二叉树期权定价模型
- ⏳ 希腊值计算
- ⏳ Web界面和可视化
- ⏳ RESTful API

## 许可证

待定
