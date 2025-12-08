# 贡献指南

感谢您对 OptionPricer 项目的关注！我们欢迎各种形式的贡献。

## 如何贡献

### 报告问题

如果您发现了 bug 或有功能建议：

1. 在 [Issues](https://github.com/yourusername/option-pricer/issues) 页面搜索是否已有相关问题
2. 如果没有，创建新的 issue，并提供：
   - 清晰的标题和描述
   - 重现步骤（对于 bug）
   - 预期行为和实际行为
   - 系统环境信息（操作系统、编译器版本等）

### 提交代码

1. Fork 本仓库
2. 创建您的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交您的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启一个 Pull Request

### 代码规范

- 遵循 C++17 标准
- 使用 4 空格缩进
- 变量和函数使用 snake_case 命名
- 类名使用 PascalCase 命名
- 添加适当的注释和文档
- 确保所有测试通过
- 保持代码覆盖率 > 80%

### 提交信息规范

使用清晰的提交信息：

- `feat: 添加新功能`
- `fix: 修复 bug`
- `docs: 更新文档`
- `test: 添加或修改测试`
- `refactor: 重构代码`
- `perf: 性能优化`
- `style: 代码格式调整`

## 开发环境设置

```bash
# 克隆仓库
git clone https://github.com/yourusername/option-pricer.git
cd option-pricer

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake ..
cmake --build .

# 运行测试
./option_pricer_tests
```

## 测试

在提交 PR 之前，请确保：

- 所有现有测试通过
- 为新功能添加了测试
- 测试覆盖率没有下降

## 文档

如果您的更改影响了用户界面或 API：

- 更新 README.md
- 更新相关的文档文件
- 在代码中添加注释

## 行为准则

- 尊重所有贡献者
- 保持专业和友好的交流
- 接受建设性的批评
- 关注对项目最有利的事情

## 许可证

通过贡献代码，您同意您的贡献将在 MIT 许可证下发布。

## 问题？

如有任何问题，请通过 [Issues](https://github.com/yourusername/option-pricer/issues) 联系我们。
