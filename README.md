# FreeMove_Square

## Project name aliases / 项目别名

FreeMoveSQ  
FreeMove-SQ  
FreeMove_SQ  
FreeMove Square  
FreeMoveSquare 
FreeMove

---

[English Version](README_EN.md)

FreeMove_Square 是一个用于 **安全移动文件或目录并自动创建链接** 的 Windows 工具，  
可在不破坏程序、快捷方式和依赖关系的前提下，将文件或文件夹迁移到其他磁盘位置。

---

### ⬇️ 获取 FreeMove_Square

**点击此处下载最新的自动构建版本 (Windows x64):**
[➡️ 前往 GitHub Actions 下载页面](https://github.com/jingyu-wang/FreeMove_Square/actions/workflows/build.yml)

> **注意:** 下载后，请选择最新的 **Kawaii Build 运行**，然后找到右侧的 **Artifacts** (FreeMove_Square-Build) 进行下载。

---

## 功能简介

FreeMove_Square 允许你：

- 将文件或文件夹移动到其他磁盘
- 自动创建符号链接（mklink）来保持原路径可访问
- 支持**快速移动模式**（同盘剪切）
- 支持普通复制模式（复制完成后删除源文件）
- 支持隐藏原路径
- 提供多语言界面（中文 / English）
- 支持拖拽操作
- 自动检测管理员权限

---

## 工作原理

FreeMove_Square 的工作流程如下：

1. 将原始文件或目录移动到新位置
2. 在原路径创建一个符号链接（指向新位置）
3. 所有访问旧路径的程序将自动跳转至新位置
4. 对用户和系统来说，路径“看起来没有变化”

---

## 使用方法

1. 以**管理员模式运行**程序
2. 在界面中：
   - 选择要移动的文件或文件夹
   - 选择目标路径
3. 点击【移动】
4. 操作完成后原路径仍然可访问（通过链接重定向）

---

## 注意事项

⚠ **请勿随意移动系统关键目录**

不要尝试移动以下目录，否则可能导致系统异常：

- `C:\Windows`
- `C:\Program Files`
- `C:\Program Files (x86)`
- `C:\Users`
- `C:\ProgramData`

你可以安全移动如下路径中的子目录，如：

```
C:\Users\YourName\AppData\Local\SomeCache
```

---

## 卸载或恢复

### 移除已移动程序

按原来的方式卸载软件即可，符号链接不会影响卸载流程。卸载后可手动删除：
- 原位置的链接
- 新位置中的残留空目录

---

### 还原程序位置

1. 删除原路径中的符号链接
2. 将文件夹移动回原始位置

---

## 管理员权限

本程序需要管理员权限来：

- 创建符号链接（mklink）
- 操作系统目录
- 修改文件属性

如果未以管理员身份启动，程序会提示并提供一键提权按钮。


---

## 开源协议

本项目遵循 GPL v3 协议发布。

你可以自由使用、修改与分发，但请遵守开源协议条款。

---
