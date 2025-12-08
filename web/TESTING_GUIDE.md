# OptionPricer 前后端通信测试指南

## 快速开始

### 1. 启动服务器

```bash
# 编译项目（如果还没有编译）
mkdir -p build
cd build
cmake ..
make

# 启动服务器
./option_pricer
```

服务器将在 `http://localhost:8080` 上运行。

### 2. 测试方法

有两种测试方法：

#### 方法 A: 使用主界面（推荐）

1. 在浏览器中打开：`http://localhost:8080/`
2. 填写期权参数表单
3. 点击"计算期权价格"按钮
4. 观察结果显示和可视化图表

#### 方法 B: 使用API测试页面

1. 在浏览器中打开：`http://localhost:8080/test_api_communication.html`
2. 依次点击各个测试按钮
3. 查看每个测试的响应结果

## 测试场景

### 场景 1: 正常计算（Black-Scholes）

**输入参数**:
- 模型: Black-Scholes
- 期权类型: 看涨期权
- 标的资产价格: 100
- 执行价格: 105
- 到期时间: 1.0年
- 无风险利率: 0.05 (5%)
- 波动率: 0.20 (20%)

**期望结果**:
- 显示期权价格（约8-9）
- 显示所有希腊值
- 显示价格曲线图表
- 显示希腊值曲线图表
- 计算时间 < 100ms

### 场景 2: 正常计算（二叉树模型）

**输入参数**:
- 模型: 二叉树模型
- 二叉树步数: 100
- 期权类型: 看跌期权
- 标的资产价格: 100
- 执行价格: 105
- 到期时间: 1.0年
- 无风险利率: 0.05
- 波动率: 0.20

**期望结果**:
- 显示期权价格
- 显示所有希腊值
- 显示可视化图表
- 计算时间 < 500ms

### 场景 3: 输入验证错误

**测试步骤**:
1. 输入负数标的资产价格: -100
2. 点击计算

**期望结果**:
- 显示错误消息："标的资产价格必须为正数"
- 不发送API请求
- 表单保持可编辑状态

### 场景 4: 服务器错误处理

**测试步骤**:
1. 停止服务器
2. 尝试计算期权价格

**期望结果**:
- 显示错误消息："网络错误：无法连接到服务器"
- 按钮恢复正常状态
- 可以重新尝试

### 场景 5: 超时处理

**测试步骤**:
1. 选择二叉树模型
2. 设置步数为 1000
3. 点击计算

**期望结果**:
- 如果计算超过5秒，显示超时错误
- 请求被中止
- 用户可以调整参数重试

## 验证清单

### 功能验证

- [ ] Fetch API正确发送POST请求
- [ ] JSON数据正确序列化和反序列化
- [ ] 加载状态正确显示
- [ ] 计算结果正确显示
- [ ] 可视化图表正确渲染
- [ ] 错误消息正确显示

### 错误处理验证

- [ ] 前端输入验证工作正常
- [ ] 网络错误被正确捕获
- [ ] 超时错误被正确处理
- [ ] 服务器错误（400, 500）被正确显示
- [ ] 错误详情列表正确显示

### 用户体验验证

- [ ] 加载时按钮和输入被禁用
- [ ] 加载动画正确显示
- [ ] 错误消息清晰易懂
- [ ] 结果显示格式良好
- [ ] 页面滚动到结果区域

### 性能验证

- [ ] Black-Scholes计算 < 100ms
- [ ] 二叉树(100步)计算 < 500ms
- [ ] 可视化生成 < 2s
- [ ] 超时机制正常工作

## 常见问题

### Q: 服务器无法启动
**A**: 检查端口8080是否被占用，或修改端口号

### Q: 无法连接到服务器
**A**: 确保服务器正在运行，检查防火墙设置

### Q: 计算结果不正确
**A**: 检查输入参数是否合理，查看服务器日志

### Q: 可视化图表不显示
**A**: 检查浏览器控制台是否有JavaScript错误

## 调试技巧

### 1. 使用浏览器开发者工具

打开浏览器开发者工具（F12）：
- **Network标签**: 查看HTTP请求和响应
- **Console标签**: 查看JavaScript错误和日志
- **Elements标签**: 检查DOM结构

### 2. 查看请求详情

在Network标签中：
1. 找到 `/api/price` 或 `/api/visualize` 请求
2. 查看Request Headers和Request Payload
3. 查看Response Headers和Response Body
4. 检查状态码和响应时间

### 3. 服务器日志

服务器会在终端输出日志：
```
Starting OptionPricer HTTP server on port 8080...
Web interface: http://localhost:8080/
API endpoints:
  POST http://localhost:8080/api/price
  POST http://localhost:8080/api/visualize
```

## 手动API测试

使用curl命令测试API：

```bash
# 测试价格计算API
curl -X POST http://localhost:8080/api/price \
  -H "Content-Type: application/json" \
  -d '{
    "model": "black-scholes",
    "option_type": "call",
    "spot_price": 100.0,
    "strike_price": 105.0,
    "time_to_maturity": 1.0,
    "risk_free_rate": 0.05,
    "volatility": 0.2
  }'

# 测试可视化API
curl -X POST http://localhost:8080/api/visualize \
  -H "Content-Type: application/json" \
  -d '{
    "model": "black-scholes",
    "option_type": "call",
    "spot_price": 100.0,
    "strike_price": 105.0,
    "time_to_maturity": 1.0,
    "risk_free_rate": 0.05,
    "volatility": 0.2,
    "points": 30
  }'
```

## 报告问题

如果发现问题，请记录：
1. 输入参数
2. 期望结果
3. 实际结果
4. 错误消息（如果有）
5. 浏览器控制台日志
6. 服务器日志

## 总结

完成所有测试场景后，确认：
- ✓ 所有正常场景工作正常
- ✓ 所有错误场景被正确处理
- ✓ 用户体验流畅
- ✓ 性能满足要求

Task 17 实现完成！
