# VSCode C++ 调试环境配置

> 本节是在前面的 **GCC + CMake + Ninja + clangd** 环境基础上继续配置，面向 RoboMaster 电控嵌入式项目。
>
> **本节只配置编译、Kconfig、代码格式化以及后续烧录工具所需的基础环境，不涉及 VSCode 调试器、GDB、断点调试等配置。**
>
> 推荐版本尽量使用当前稳定版；如果项目本身锁定了某个工具链版本，以项目要求为准。

## 1. 需要安装的东西

| 工具 | 推荐版本 | 作用 |
|---|---:|---|
| **Arm GNU Toolchain** | **15.3.rel1** | ARM Cortex-M 交叉编译器，提供 `arm-none-eabi-gcc/g++` |
| **Python** | **3.14.x** | Kconfiglib 的运行环境 |
| **Kconfiglib** | **14.1.0** | 解析 Kconfig、生成配置结果等 |
| **clangd** | **23.1.2** | C/C++ 代码补全、跳转、报错 |
| **clang-format** | **23.1.2** | C/C++ 代码自动格式化 |
| **Node.js** | **24.21.0 LTS** | 部分工程脚本、工具链周边工具的运行环境 |
| **OpenOCD** | **0.12.0** | 后续连接调试器、烧录/下载固件的工具；本教程只完成安装，不配置调试 |

> Arm GNU Toolchain 15.3.rel1 是当前可用的新版 Arm GNU Toolchain，版本号中的 GCC 为 15.3.1。Arm 官方同时提供 Windows 和 Linux 的 `arm-none-eabi` 裸机工具链。
>
> LLVM 23.1.2 同时包含 clangd 和 clang-format，因此不需要分别安装两个 LLVM。
>
> Kconfiglib 当前最新发布版本为 14.1.0，支持 Python 3.2 及以上。

---

## 2. Arm GNU Toolchain

Arm GNU Toolchain 是真正负责把 RoboMaster 电控 C/C++ 代码编译成 **ARM Cortex-M** 可执行代码的编译器。这里安装的是 `arm-none-eabi` 裸机工具链，**原先的 GCC 仅用于你们练习简单的 C++ 代码，并不适用于嵌入式系统**。

官方页面：

<https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>

### Windows

推荐直接从 Arm 官方下载，替代 MinGW 的 `gcc` 。

下载：

```text
arm-gnu-toolchain-15.3.rel1-win32-x86_64-arm-none-eabi.zip
```

解压到一个**路径中不要出现中文和空格**的位置，例如：

```text
C:/Arm/arm-gnu-toolchain-15.3.rel1-win32-x86_64-arm-none-eabi
```

然后把下面的 `bin` 目录加入 Windows 的 PATH：

```text
C:/Arm/arm-gnu-toolchain-15.3.rel1-win32-x86_64-arm-none-eabi/bin
```

加 PATH 的方法：开始菜单搜索「编辑系统环境变量」→「环境变量」→ 找到 `Path` →「编辑」→「新建」。

加完以后**重新打开 VSCode 和终端**。

验证：

```bash
arm-none-eabi-gcc --version
arm-none-eabi-g++ --version
arm-none-eabi-objcopy --version
```

能看到 `15.3.1` 左右的版本信息即可。

### Linux

Ubuntu / Debian x86_64 推荐从 Arm 官方下载：

```text
arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi.tar.xz
```

解压到 `/opt`：

```bash
cd /tmp
tar -xf arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi.tar.xz
sudo mv arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi /opt/
```

加入 PATH：

```bash
echo 'export PATH="/opt/arm-gnu-toolchain-15.3.rel1-x86_64-arm-none-eabi/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

验证：

```bash
arm-none-eabi-gcc --version
arm-none-eabi-g++ --version
arm-none-eabi-objcopy --version
```

> 如果使用的是 zsh，把 `~/.bashrc` 换成 `~/.zshrc`。

---

## 3. Python + Kconfiglib

Kconfiglib 是 Kconfig 系统的 Python 实现，后面的电控项目可能会用它生成配置结果、配置头文件等。

### Windows

推荐从 Python 官方下载安装 Python 3.14：

<https://www.python.org/downloads/windows/>

安装时一定注意勾选：

```text
Add python.exe to PATH
```

安装完成后重新打开 bash，验证：

```bash
python --version
python -m pip --version
```

然后安装 Kconfiglib：

```bash
python -m pip install --upgrade pip
python -m pip install kconfiglib
```

验证：

```bash
python -m pip show kconfiglib
menuconfig --help
```

> 如果提示 `menuconfig` 不是命令，不一定代表 Kconfiglib 没装好。Windows 下如果使用 `pip --user`，还需要把 Python 的 `Scripts` 目录加入 PATH。
>
> 本教程只要求 Kconfiglib 能正常安装和运行；如果后续项目需要 Windows 下的终端 `menuconfig` 界面，再额外处理终端 curses 兼容问题。

### Linux

Ubuntu / Debian：

```bash
sudo apt update
sudo apt install python3 python3-pip python3-venv
```

验证：

```bash
python3 --version
python3 -m pip --version
```

安装 Kconfiglib：

```bash
python3 -m pip install --user --upgrade kconfiglib
```

验证：

```bash
python3 -m pip show kconfiglib
```

如果 `menuconfig` 命令找不到，把用户 Python 的 `bin` 目录加入 PATH。常见情况：

```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

然后：

```bash
menuconfig --help
```

> 如果发行版限制直接使用系统 Python 的 pip，优先使用 Python 虚拟环境，而不是强行修改系统 Python。

---

## 4. clangd + clang-format

前面的基础环境已经要求安装 clangd；这里建议把 LLVM 统一到 **23.1.2**，这样同时得到：

```text
clangd
clang-format
```

LLVM 官方发布页：

<https://github.com/llvm/llvm-project/releases>

### Windows

最简单的方法是下载 LLVM 官方 Windows 安装包：

```text
LLVM-23.1.2-win64.exe
```

安装时勾选：

```text
Add LLVM to the system PATH
```

安装完成后重新打开 bash 和 VSCode。

验证：

```bash
clangd --version
clang-format --version
```

两个版本都应为 `23.1.2` 左右。
如果之前已经通过 Chocolatey 安装过 LLVM，不需要重复安装。直接确认：

```bash
clangd --version
clang-format --version
```

如果版本已经满足 `>= 20`，可以继续使用。

### Linux

为了避免 Ubuntu 自带仓库中的 LLVM 版本过旧，这里推荐直接使用 LLVM 官方发布的预编译包。

打开：

<https://github.com/llvm/llvm-project/releases>

下载 Linux x86_64 对应的 LLVM 23.1.2 压缩包，然后解压，例如：

```bash
sudo mkdir -p /opt/llvm-23
sudo tar -xf clang+llvm-23.1.2-x86_64-linux-gnu-ubuntu-22.04.tar.xz \\
    --strip-components=1 -C /opt/llvm-23
```

加入 PATH：

```bash
echo 'export PATH="/opt/llvm-23/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

验证：

```bash
clangd --version
clang-format --version
```

> 如果你使用的发行版提供了 `clangd >= 20` 和 `clang-format >= 20`，也可以直接使用发行版的软件包，不必手动安装 LLVM。

安装完成后，进入 **打开用户设置界面（JSON）**，copy 大括号这份代码至 ``setting.json``，替代原先的 mingw，这会对你的所有项目生效：
```json
{
  "clangd.arguments": [
      "--compile-commands-dir=${workspaceFolder}/build",
      "--log=verbose",
      "--pretty",
      "--all-scopes-completion",
      "--completion-style=bundled",
      "--cross-file-rename",
      "--header-insertion=iwyu",
      "--header-insertion-decorators",
      "--background-index",
      "--clang-tidy",
      "--clang-tidy-checks=cppcoreguidelines-*,performance-*,bugprone-*,portability-*,modernize-*,google-*",
      "--pch-storage=memory",
      "--function-arg-placeholders=false",
      "--fallback-style=llvm",
      // Windows，替换成自己的
      "--query-driver=D:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin/arm-none-eabi*",
      // Linux，替换成自己的
      "--query-driver=/home/gdfish/usr/arm-gnu-toolchain/bin/arm-none-eabi*"
  ],
} 
```

---

## 5. Node.js 24

Node.js 是可以在电脑端运行的 JavaScript 的运行时环境。

官方下载页面：

<https://nodejs.org/en/download/>

当前 24.x LTS 为：

```text
Node.js v24.21.0 LTS
```

### Windows

可以直接安装官方 MSI：`node-v24.21.0-x64.msi`

也可以使用管理员 bash：

```bash
winget install --id OpenJS.NodeJS.24 -e
```

安装完成后重新打开终端：

```bash
node --version
npm --version
```

应分别看到类似：

```text
v24.21.0
11.x
```

### Linux

推荐使用 `nvm`，以后切换 Node.js 版本比较方便：

```bash
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.7/install.sh | bash
```

重新加载：

```bash
source ~/.bashrc
```

安装 Node.js 24：

```bash
nvm install 24
nvm use 24
nvm alias default 24
```

验证：

```bash
node --version
npm --version
```

---

## 6. OpenOCD

OpenOCD 是：`Open On-Chip Debugger`

它负责让电脑上的调试软件与 STM32 等 MCU 的调试接口进行通信。

OpenOCD 官网：

<https://openocd.org/>

官方 Getting OpenOCD 页面：

<https://openocd.org/pages/getting-openocd.html>

> Windows 和 Linux 都统一从 OpenOCD 官方网站进入下载/获取页面。
>
> 需要注意：OpenOCD 官方网站目前主要提供源码和发行版获取说明，并不长期维护一个独立的 Windows 官方安装器。官网的 Getting OpenOCD 页面明确区分了操作系统软件仓库、源码以及第三方预编译二进制包。

---

### Windows

打开 OpenOCD 官网：

<https://openocd.org/>

进入：

<https://openocd.org/pages/getting-openocd.html>

在 **Getting OpenOCD** 页面查看 Windows 可用的二进制发行版。

如果使用预编译的 Windows ZIP：

1. 下载 Windows 64 位版本。
2. 解压到固定目录并加入系统 PATH。

重新打开 bash：

```bash
openocd --version
```

能够正常输出版本号即可。

### 关于 USB 驱动

OpenOCD 本身只是软件，还需要操作系统能够识别实际的调试器，
调试时需要断点测试和实时查看变量：**在 VsCode上下载 Cortex-Debug 插件**。

本文只负责把 OpenOCD 安装好，对于 ST-Link/J-Link 的具体驱动，需要自行找官网，按需配置：
- **ST-Link**

ST官方驱动页面：
<https://www.st.com/en/development-tools/stsw-link009.html>

下载并安装完成后，将 ST-Link 连接电脑即可。

可以打开 Windows「设备管理器」检查 ST-Link 是否正常识别。

Linux 的 ST-Link 通常不需要像 Windows 那样安装一个独立的 ST-Link USB 驱动，主要是配置 udev 规则和 OpenOCD/其他调试软件的访问权限。

- **J-Link**

SEGGER 官方提供的是 J-Link Software and Documentation Pack，里面包含 J-Link 所需的软件和驱动。

官方下载链接：
<https://www.segger.com/downloads/jlink?utm_source=chatgpt.com>

安装完成后重新连接 J-Link 即可。

可以使用 SEGGER 提供的 JLinkExe 检查 J-Link 是否正常识别。

---

## Linux

Linux 同样从 OpenOCD 官方网站进入：

<https://openocd.org/pages/getting-openocd.html>

Linux 用户有两种方式：

### 方式一：使用发行版软件包

Ubuntu / Debian：

```bash
sudo apt update
sudo apt install openocd
```

安装完成：

```bash
openocd --version
```

如果发行版仓库版本满足项目要求，直接使用即可。

### 方式二：从 OpenOCD 官方源码构建

如果需要使用 OpenOCD 官网提供的源码版本，则从：

<https://openocd.org/pages/getting-openocd.html>

进入官方源码获取页面。

下载源码压缩包后按照官方文档进行构建。

OpenOCD 官方文档：

<https://openocd.org/doc/html/>

构建完成后验证：

```bash
openocd --version
```
---

## 7. 一次性验证整个环境

把前面所有环境配置好后，重新打开终端。

### Windows

在 bash 执行：

```bash
arm-none-eabi-gcc --version
python --version
python -m pip show kconfiglib
clangd --version
clang-format --version
node --version
npm --version
openocd --version
cmake --version
ninja --version
```

### Linux

在终端执行：

```bash
arm-none-eabi-gcc --version
python3 --version
python3 -m pip show kconfiglib
clangd --version
clang-format --version
node --version
npm --version
openocd --version
cmake --version
ninja --version
```

---

## 8. 常见问题

| 问题 | Windows | Linux |
|---|---|---|
| `arm-none-eabi-gcc` 不是命令 | 检查 Arm GNU Toolchain 的 `bin` 是否加入 PATH，然后重开终端 | 检查 `/opt/arm-gnu-toolchain-*/bin` 是否加入 PATH，然后重新加载 shell |
| `python` / `python3` 不是命令 | 重新安装 Python，并勾选 `Add python.exe to PATH` | 安装 `python3`，并确认 `/usr/bin/python3` 存在 |
| Kconfiglib 安装后 `menuconfig` 不是命令 | 检查 Python `Scripts` 目录是否在 PATH | 检查 `~/.local/bin` 是否在 PATH |
| `pip install kconfiglib` 报权限错误 | 使用 `python -m pip install --user kconfiglib` | 使用 `--user` 或 Python 虚拟环境，不要破坏系统 Python |
| `clangd` 版本太旧 | 不要混用多个 LLVM，建议统一安装 LLVM 23.1.2 | 优先使用 LLVM 官方版本，或确认发行版版本满足 `>= 20` |
| `clang-format` 不是命令 | 确认 LLVM 的 `bin` 在 PATH | 确认 `/opt/llvm-23/bin` 或系统 LLVM 的 `bin` 在 PATH |
| `node` 不是命令 | 重开终端；确认 `winget` 安装成功 | 检查 `nvm` 是否加载，再执行 `nvm use 24` |
| `openocd` 不是命令 | 检查 `C:\msys64\ucrt64\bin` 是否加入 PATH | 重新执行 `sudo apt install openocd` |
| Arm 工具链能找到但 CMake 找不到 | 后续 CMake 配置时确认使用的是 `arm-none-eabi-gcc`，不要误用 PC 上的 x86 GCC | 同左 |
| PATH 修改后没有效果 | **重开 VSCode 和终端**，已经打开的终端不会自动刷新 PATH | 重新打开终端或 `source ~/.bashrc` |

---

## 9. 配置完成后的工具链关系

到这里，电控项目从编译到运行的架构为：

```text
                    C/C++ 源代码
                         │
                         ▼
              ┌─────────────────────┐
              │        CMake        │
              │    管理整个工程构建    │
              └──────────┬──────────┘
                         │
                         ▼
              ┌─────────────────────┐
              │       Ninja         │
              │     执行构建任务      │
              └──────────┬──────────┘
                         │
                         ▼
              ┌─────────────────────┐
              │  Arm GNU Toolchain  │
              │ arm-none-eabi-gcc   │
              │ arm-none-eabi-g++   │
              └──────────┬──────────┘
                         │
                         ▼
                       .elf


Kconfig  ──→  Kconfiglib  ──→  配置结果 / 配置头文件

VSCode   ──→  clangd      ──→  补全 / 定义跳转 / 代码检查

C/C++    ──→  clang-format ─→  统一代码格式

工程脚本 ──→  Node.js 24  ──→  JavaScript / Node 工具

后续烧录 ──→  OpenOCD    ──→  调试器 / MCU
```

## 10. 构建你的 STM32 项目

1、在 cubemx 中创建一个项目，名为 `test`，在 `Project Manager` 的 `Toochain/IDE` 选择 Cmake, Code Genderator 中选择 `Copy only the necessary library file`。这样，你们原先在 Keil 上的工作可以在 VsCode 上全部完成。

2、在项目的 `CMakeLists.txt` 第 20 行添加 `include(cmake/gcc-arm-none-eabi.cmake)`

3、编译
```bash
cmake -B build -G Ninja
ninja -C build
```
4、烧录
```bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg  -c 'program ./build/test.elf verify reset exit'
```
其中 `target/stm32f1x.cfg` 与 `./build/test.elf` 在其他文件中需要换成自己实际用的芯片型号和 elf 文件

5、烧录成功会提示：
```test
Programming Finished
Verify Started
Verified OK
Resetting Target
```

6、调试文件
在项目中创建 `.vscode/launch.json` 文件:
```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "showDevDebugOutput": "none",
      "cwd": "${workspaceFolder}",
      "executable": "./build/test.elf",
      "name": "dap",
      "request": "launch",
      "type": "cortex-debug",
      "servertype": "openocd",
      "configFiles": [
        "interface/cmsis-dap.cfg",
        "target/stm32f1x.cfg"
      ],
      "liveWatch": {
        "enabled": true,
        "samplesPerSecond": 1
      }
    }
  ]
}
```
编译成功后点击 `Run nad Debug` 运行和调试, 点击 `dap` 开始调试。
