# GitHub 部署指南

本文档介绍如何将 OptionPricer 项目部署到 GitHub。

## 前提条件

1. 拥有 GitHub 账号
2. 安装 Git 客户端
3. 项目已在本地构建并测试通过

## 部署步骤

### 1. 在 GitHub 上创建仓库

1. 登录 [GitHub](https://github.com)
2. 点击右上角的 "+" 按钮，选择 "New repository"
3. 填写仓库信息：
   - Repository name: `option-pricer`
   - Description: `高性能期权定价系统 - 支持 Black-Scholes 和二叉树模型`
   - 选择 Public 或 Private
   - 不要勾选 "Initialize this repository with a README"（因为我们已有 README）
4. 点击 "Create repository"

### 2. 初始化本地 Git 仓库

如果还没有初始化 Git：

```bash
# 在项目根目录下
git init
git add .
git commit -m "Initial commit: OptionPricer project"
```

### 3. 连接到 GitHub 仓库

```bash
# 添加远程仓库（替换 yourusername 为您的 GitHub 用户名）
git remote add origin https://github.com/yourusername/option-pricer.git

# 推送代码
git branch -M main
git push -u origin main
```

### 4. 验证部署

1. 访问您的 GitHub 仓库页面
2. 确认所有文件已上传
3. 检查 README.md 是否正确显示
4. 查看 Actions 标签页，确认 CI 工作流是否运行

## GitHub Actions CI/CD

项目已配置自动化 CI/CD 流程（`.github/workflows/ci.yml`）：

- 自动在 Linux、Windows、macOS 上构建
- 自动运行所有测试
- 在每次 push 和 pull request 时触发

### 查看构建状态

1. 访问仓库的 "Actions" 标签页
2. 查看最新的工作流运行状态
3. 点击具体的运行查看详细日志

### 添加状态徽章

在 README.md 中添加构建状态徽章：

```markdown
[![Build Status](https://github.com/yourusername/option-pricer/workflows/CI/badge.svg)](https://github.com/yourusername/option-pricer/actions)
```

## 发布版本

### 创建 Release

1. 在 GitHub 仓库页面，点击 "Releases"
2. 点击 "Create a new release"
3. 填写版本信息：
   - Tag version: `v1.0.0`
   - Release title: `OptionPricer v1.0.0`
   - Description: 描述此版本的主要功能和改进
4. 可以上传编译好的二进制文件
5. 点击 "Publish release"

### 版本号规范

遵循语义化版本（Semantic Versioning）：

- MAJOR.MINOR.PATCH (例如: 1.0.0)
- MAJOR: 不兼容的 API 变更
- MINOR: 向后兼容的功能新增
- PATCH: 向后兼容的问题修正

## 项目维护

### 分支策略

建议使用以下分支：

- `main`: 稳定的生产版本
- `develop`: 开发分支
- `feature/*`: 功能分支
- `hotfix/*`: 紧急修复分支

### 保护主分支

1. 进入仓库的 Settings > Branches
2. 添加分支保护规则：
   - Branch name pattern: `main`
   - 勾选 "Require pull request reviews before merging"
   - 勾选 "Require status checks to pass before merging"
   - 选择 CI 工作流作为必需的状态检查

### 管理 Issues 和 Pull Requests

1. 使用 Labels 分类问题（bug, enhancement, documentation 等）
2. 使用 Milestones 规划版本
3. 使用 Projects 管理开发进度
4. 及时回复和处理 Issues 和 PRs

## 文档更新

部署后需要更新的内容：

1. 在 README.md 中更新仓库链接：
   - 将 `yourusername` 替换为实际的 GitHub 用户名
   - 更新 Issues 链接
   - 更新联系邮箱

2. 添加项目网站（可选）：
   - 使用 GitHub Pages 托管文档
   - 在 Settings > Pages 中启用

## 常见问题

### 推送失败

如果推送时遇到认证问题：

```bash
# 使用 SSH 方式（推荐）
git remote set-url origin git@github.com:yourusername/option-pricer.git

# 或使用 Personal Access Token
# 在 GitHub Settings > Developer settings > Personal access tokens 创建 token
```

### 大文件问题

如果有大文件（> 100MB）：

1. 添加到 .gitignore
2. 或使用 Git LFS：

```bash
git lfs install
git lfs track "*.bin"
git add .gitattributes
```

### CI 构建失败

1. 检查 Actions 日志
2. 在本地重现问题
3. 修复后重新推送

## 下一步

- 邀请协作者
- 设置 GitHub Discussions 进行社区讨论
- 添加 Wiki 页面提供详细文档
- 考虑添加 Code of Conduct
- 设置 GitHub Sponsors（如果需要）

## 资源链接

- [GitHub 文档](https://docs.github.com)
- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [Git 教程](https://git-scm.com/doc)
