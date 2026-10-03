# PinyTutorial
<p align="center">
    <a href="http://commitizen.github.io/cz-cli/"><img
            src="https://img.shields.io/badge/commitizen-friendly-brightgreen.svg"
            alt="Commitizen friendly"/></a>
    <a href="https://github.com/semantic-release/semantic-release"><img
            src="https://img.shields.io/badge/semantic--release-angular-e10079?logo=semantic-release"
            alt="semantic-release: angular"/></a>
</p>

# 🏆 Inrtoduction
本项目为电控新生培训框架，建立在电控通用框架的基础之上，主要面向stm32f103c8t6、stm32f407ighx（C板）单片机简单功能实现、外设、电机PID调试、FreeRTOS等基础模块培训内容，不具备直接上车运行能力。

# 🎯 Requirement

1. cmake >= 3.24
2. ninja >= 1.1
3. [arm-none-eabi-toolchains](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) >= 10.3.1 or [llvm](https://github.com/arm/arm-toolchain) (experimental)
4. python >= 3.2 (maybe also need python-is-python3 in ubuntu)
5. [kconfiglib](https://github.com/ulfalizer/Kconfiglib)

we also suggest to install following software:
1. [clangd](https://clangd.llvm.org/) >= 20.0
1. clang-format >= 20.0
2. [commitizen](https://github.com/commitizen/cz-cli)
3. ccache

# 🌟 Getting started
## 🏗️ Build

对于 stm32f103c8t6 单片机的自定义外设配置，需要自行从 cubemx 配置后，导入至 Src/Common/Soc/stm32f103c8tx/hal（替换成自己的绝对路径）

**构建**

```sh
cmake -B build -G Ninja
```

**编译**

```sh
ninja -C build
```

****

支持基于Kconfig对项目进行配置

```sh
cmake --build ./build --target menuconfig
```

支持直接根据预制的配置文件来构建项目 (应存在于根目录，本例文件名为"yourconfig")

```sh
cmake -B build -G Ninja -DCONFIG_NAME=yourconfig
```

## 🐞 Debug

1. openocd >= 0.12.0
2. [cortex-debug](https://github.com/Marus/cortex-debug) / [codelldb](https://github.com/vadimcn/codelldb) (vscode-plugin)
3. Ozone 3.24
4. FreeMASTER/FreeMASTER Lite >= 3.1.3 (后者支持Linux，需装前置Java JRE)
5. systemview

## ⚠️ Notice

1. 在main函数执行之前调用HAL库函数是危险的！请避免使调用HAL库的构造函数的类对象成为全局变量（可以创建全局的指针，或使用./Src/Utils/Lazy中的工具，并在初始化阶段构造）；
1. 首次构建时，本项目会自动从github拉取第三方库，请确保网络可用。

## 🧩 Framework

![frame](.docs/frame.png)

