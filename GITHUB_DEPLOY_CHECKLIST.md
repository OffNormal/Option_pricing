# GitHub 部署检查清单

## 部署前准备 ✓

### 1. 检查项目文件
- [x] README.md 已完善
- [x] LICENSE 文件已创建
- [x] .gitignore 已配置
- [x] CONTRIBUTING.md 已创建
- [x] GitHub Actions CI 配置已添加

### 2. 本地测试
```bash
# 确保项目可以正常构建
mkdir build && cd build
cmake ..
cmake --build . --config Release

# 运行测试
./option_pricer_tests

# 测试服务器
./option_pricer --port 8080
# 访问 http://localhost:8080 确认正常工作
```

### 3. 清理构建文件
```bash
# 返回项目根目录
cd ..

# 确保 build 目录在 .gitignore 中
# 删除不需要提交的文件
```

## 部署步骤

### 步骤 1: 在 GitHub 创建仓库

1. 访问 https://github.com/new
2. 填写信息：
   - Repository name: `option-pricer`
   - Description: `高性能期权定价系统 - 支持 Black-Scholes 和二叉树模型`
   - 选择 Public（推荐）或 Private
   - **不要**勾选 "Add a README file"
   - **不要**勾选 "Add .gitignore"
   - **不要**选择 License（我们已有 LICENSE 文件）
3. 点击 "Create repository"

### 步骤 2: 初始化本地仓库

```bash
# 如果还没有初始化 Git
git init

# 添加所有文件
git add .

# 查看将要提交的文件
git status

# 首次提交
git commit -m "Initial commit: OptionPricer - 高性能期权定价系统"
```

### 步骤 3: 连接并推送到 GitHub

```bash
# 添加远程仓库（替换 YOUR_USERNAME 为您的 GitHub 用户名）
git remote add origin https://github.com/YOUR_USERNAME/option-pricer.git

# 设置主分支名称
git branch -M main

# 推送代码
git push -u origin main
```

### 步骤 4: 验证部署

访问您的 GitHub 仓库页面，检查：
- [ ] 所有文件已正确上传
- [ ] README.md 正确显示
- [ ] Actions 标签页显示 CI 工作流正在运行
- [ ] CI 构建成功（绿色勾号）

## 部署后配置

### 1. 更新 README.md 中的链接

在 README.md 中查找并替换：
- `yourusername` → 您的 GitHub 用户名
- `your.email@example.com` → 您的邮箱

```bash
# 编辑 README.md 后
git add README.md
git commit -m "docs: 更新仓库链接和联系信息"
git push
```

### 2. 添加构建状态徽章

在 README.md 顶部添加：

```markdown
[![Build Status](https://github.com/YOUR_USERNAME/option-pricer/workflows/CI/badge.svg)](https://github.com/YOUR_USERNAME/option-pricer/actions)
```

### 3. 配置仓库设置

在 GitHub 仓库页面：

1. **Settings > General**
   - 添加 Topics: `cpp`, `finance`, `options-pricing`, `black-scholes`, `quantitative-finance`
   - 添加 Website（如果有）

2. **Settings > Branches**
   - 添加分支保护规则（可选）
   - Branch name pattern: `main`
   - 勾选 "Require status checks to pass before merging"

3. **Settings > Pages**（可选）
   - 启用 GitHub Pages 托管文档

### 4. 创建第一个 Release

1. 访问 Releases 页面
2. 点击 "Create a new release"
3. 填写：
   - Tag: `v1.0.0`
   - Title: `OptionPricer v1.0.0 - 首次发布`
   - Description: 列出主要功能
4. 发布

## 常见问题解决

### 问题 1: 推送时要求认证

**解决方案 A: 使用 Personal Access Token**
```bash
# 1. 在 GitHub Settings > Developer settings > Personal access tokens 创建 token
# 2. 选择 repo 权限
# 3. 使用 token 作为密码
```

**解决方案 B: 使用 SSH（推荐）**
```bash
# 1. 生成 SSH 密钥
ssh-keygen -t ed25519 -C "your.email@example.com"

# 2. 添加到 ssh-agent
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519

# 3. 复制公钥到 GitHub Settings > SSH and GPG keys
cat ~/.ssh/id_ed25519.pub

# 4. 更改远程 URL
git remote set-url origin git@github.com:YOUR_USERNAME/option-pricer.git
```

### 问题 2: CI 构建失败

1. 访问 Actions 标签页查看日志
2. 检查错误信息
3. 在本地重现并修复
4. 重新推送

### 问题 3: 文件太大无法推送

```bash
# 检查大文件
find . -type f -size +50M

# 添加到 .gitignore
echo "large_file.bin" >> .gitignore

# 如果已经提交，从历史中移除
git rm --cached large_file.bin
git commit -m "Remove large file"
```

## 下一步行动

- [ ] 邀请协作者（Settings > Collaborators）
- [ ] 设置 GitHub Discussions（Settings > Features）
- [ ] 添加 Wiki 页面
- [ ] 创建 Issue 模板
- [ ] 创建 Pull Request 模板
- [ ] 添加 Code of Conduct
- [ ] 设置 GitHub Projects 管理开发

## 推广项目

- 在 README 中添加演示 GIF 或截图
- 写一篇博客介绍项目
- 在相关社区分享（Reddit, HackerNews 等）
- 添加到 awesome 列表
- 在社交媒体宣传

## 维护建议

- 定期更新依赖项
- 及时回复 Issues 和 PRs
- 保持 CI 构建通过
- 定期发布新版本
- 更新文档

---

**完成部署后，您的项目将在以下地址可访问：**
- 仓库: `https://github.com/YOUR_USERNAME/option-pricer`
- Issues: `https://github.com/YOUR_USERNAME/option-pricer/issues`
- Actions: `https://github.com/YOUR_USERNAME/option-pricer/actions`

祝您部署顺利！🚀
