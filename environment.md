# 一、零基础必看:编程语言的两种运行方式

用编程语言写出来的代码,计算机是没法直接识别的——在计算机眼中,你的代码只是一堆文本,需要有一个"翻译"把它变成机器能执行的指令。

编程语言按"翻译"方式的不同分成两类:

- **解释型语言**:运行时由解释器一边读代码、一边翻译成机器指令(Python 就是解释型);
- **编译型语言**:提前由编译器把整份代码一次性编译成**二进制文件**(机器能直接听懂的语言),运行时直接执行这个二进制文件(C/C++ 就是编译型)。

两种方式没有本质优劣,只是语言特性决定了更适合哪种(Python 也有编译器,C++ 也有解释器,只是都不好用而已)。

我们(电控组)因为控制对实时性的要求,选用 **C/C++ 混编**的方式来写代码(顺带一提:新兴的 Rust 运行性能不输 C++,也是未来可期)。

那么现在开始 C++ 的第一步,也是最容易劝退的一步——**安装环境**。

# 二、VSCode C++ 开发环境配置

这套环境只需要装 **4 个工具 + 2 个扩展**,记住 **3 条命令**。

## 1. 需要安装的东西

| 工具 | 作用 |
|---|---|
| **GCC** | 编译器,把 C++ 源码变成可执行程序(安装最新稳定发行版即可) |
| **CMake** | 构建系统,自动组织编译过程 |
| **Ninja** | 构建工具,配合 CMake 实际执行编译 |
| **clangd** | 语言服务,VSCode 里的补全、报错、跳转 |

> 小贴士:下面是对这四个工具的通俗讲解,赶时间可以直接跳到「2. 验证装好了」照着做。

- **GCC** 就是编译器,任何一个 `.cpp` 文件都能直接用它编译:

  ```bash
  gcc main.cpp -o main    # 把 main.cpp 编译成可执行文件 main
  ```

  这个命令了解一下就行。它最大的问题是:文件一多、分布在多个文件夹时,每次编译都要把所有源文件写全、写对,非常痛苦。

- **CMake + Ninja** 就是来替你"管理文件名"的。分工是:CMake 读取人类友好的 `CMakeLists.txt`,生成 Ninja 的构建文件;Ninja 的语法适合机器生成、不适合手写,它读取构建文件后负责调用 GCC 完成编译。项目规则写好之后,每次编译只需要一条很短的命令,项目再大命令也不变。

- **clangd** 来自 LLVM/clang 项目(clang 是另一套 C++ 编译器)。clang 在 Windows 上兼容性麻烦,所以我们编译器继续用 GCC,只借用 clangd 的语法服务——大型项目下它的补全、报错、跳转比 VSCode 自带的 C/C++ 扩展更快更准(clang 在 Linux 上体验很好,感兴趣可以自己试试)。

### Linux(Ubuntu 系)

```bash
sudo apt install gcc g++ cmake ninja-build clangd
```

> 装的是发行版仓库里的最新稳定版 GCC,用默认的就行,不必指定版本。

### Linux(Arch 系)

```bash
sudo pacman -S gcc cmake ninja clang
```

> 注意:`g++` 在 `gcc` 包里、`clangd` 在 `clang` 包里,没有独立包名。Arch 的软件源总是最新稳定版,默认装就行。

### Windows

用包管理器 **Chocolatey** 一键安装(需要管理员权限):

1. 管理员身份打开 PowerShell(开始菜单搜 PowerShell → 右键 → **以管理员身份运行**)。
2. 没装过 Chocolatey 的先装:去 <https://chocolatey.org/install> 按官方命令装。
3. 执行:

   ```powershell
   choco install mingw cmake ninja llvm -y
   ```

4. 装完**重开终端和 VSCode**(choco 已自动配好 PATH)。

> `mingw` 包自带最新稳定版 GCC(UCRT 版),`llvm` 包提供 clangd。

**想改安装位置?**(比如装到 D 盘)两点须知:choco 的 `--install-directory` 参数是**付费版才有的功能**,免费版不支持;而且 `mingw` 包默认固定装到 `C:\ProgramData\mingw64`。想自由选路径,就用下面的 WinLibs 方案。

> **没有管理员权限 / 想自定义安装路径**的备选:用免安装的 WinLibs(<https://winlibs.com>,下载 **UCRT 版** zip 解压到任意目录,如 `D:\mingw64`,把 `D:\mingw64\bin` 手动加进 PATH)装 GCC;CMake 用 <https://cmake.org> 安装包(勾选 *Add CMake to PATH*);Ninja 下载 `ninja-win.zip`,把 `ninja.exe` 放进同目录;clangd 用 `winget install LLVM.LLVM`。

### VSCode 扩展

在 VSCode 的扩展市场里按名字搜索安装,装这两个:

| 扩展 | 作用 |
|---|---|
| **clangd**(`llvm-vs-code-extensions.vscode-clangd`) | clangd 客户端,提供补全、报错、跳转 |
| **CMake Tools**(`ms-vscode.cmake-tools`) | CMake 支持,状态栏一键构建 |

> 别装微软的 C/C++ 扩展,它的提示会和 clangd 打架。

## 2. 验证装好了

```bash
gcc --version
cmake --version
ninja --version
clangd --version
```

四条都有输出就 OK。

## 3. 拿到 demo 项目

最快的方式:直接解压本文档同目录的 **demo.zip**——里面的 `CMakeLists.txt`、`main.cpp`、`.clangd` 三个文件都写好了,你只需要改 `main.cpp` 里的代码。

> 解压得到 `demo` 文件夹,用 VSCode 打开这个文件夹。`.clangd` 是隐藏文件,文件管理器里看不到是正常的,不用管它(可以勾选文件管理器选项查看隐藏文件)。

想自己动手,就照下面建(内容与 zip 里一模一样):

```
demo/
├── CMakeLists.txt
├── main.cpp
└── .clangd
```

**CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.25)   # 要求 CMake 至少 3.25
project(demo CXX)                      # 项目名 demo,使用 C++
set(CMAKE_CXX_STANDARD 20)             # 使用 C++20 标准
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # 导出编译信息,给 clangd 智能提示用
add_executable(demo main.cpp)          # 生成可执行程序 demo
```

**main.cpp**

```cpp
#include <iostream>

int main() {
    std::cout << "Hello C++!" << std::endl;
    return 0;
}
```

**.clangd**(让 clangd 认识 GCC,否则会误报找不到头文件)

在配置前先确认使用的系统，然后把下面的内容复制到.clangd文件

- Windows:
```yaml
CompileFlags:
  Compiler: "C:/ProgramData/mingw64/mingw64/bin/g++*"
  # 用 WinLibs 装的 mingw把上面Compiler后面双引号里的路径改成 C:/mingw64/bin/g++
  Add: 
    - -std=c++17
    - --target=x86_64-w64-mingw32
```

- Linux
```yaml
CompileFlags:
  QueryDriver:
    - /usr/bin/gcc*
    - /usr/bin/g++
```

如果以上配置在电脑上报错，那直接在vscode上方：显示并运行命令 -> 首选项：打开用户设置(JSON)
把这串复制进去：
```json
"clangd.arguments": [
  // Windows
  "--query-driver=C:/ProgramData/mingw64/mingw64/bin/gcc*,C:/ProgramData/mingw64/mingw64/bin/g++.exe,C:/mingw64/bin/gcc*,C:/mingw64/bin/g++.exe"
  // Linux
  "--query-driver=/usr/bin/g++,/usr/bin/gcc*"
]
```


## 4. 构建、编译、运行(记住这 3 条)

用 VSCode 打开 `demo` 文件夹,在**底部终端**里敲(Ctrl+` 呼出):

```bash
cmake -B build -G Ninja   # 1. 配置:生成 build/ 目录(第一次需要)
ninja -C build            # 2. 编译:调用 gcc,产物在 build/
build\demo.exe            # 3. 运行(Windows)
#./build/demo             # 3. 运行(Linux)
```

- `-B build` 指定构建目录叫 `build`, `-G Ninja` 指定用 Ninja 生成构建文件。
- 以后只改代码,一条 `ninja -C build` 就够了(改了 CMakeLists.txt 它也会自动重新配置,不用再敲第一条)。
- 这就是养成工程化习惯的意义:项目再大、文件再多,构建流程始终是这两条命令。

## 5. VSCode 里直接用

> 📺 VSCode 的具体操作(界面介绍、写代码、运行、断点调试等)在「千行计划」的《一起来学C++》教学视频里有完整讲解,照着操作即可,这里不再重复。
>
> 说明:视频里的开发环境可能和本文不同——本文统一采用 **GCC + CMake + Ninja + clangd** 这一套,是为了和后续进阶学习平滑衔接,避免以后还要再换一遍环境。

1. 打开 `demo` 文件夹,底部状态栏出现 CMake 工具栏,点 **Build** 即可编译(首次会问选哪个编译器 Kit,选 gcc);也可以在底部终端里手动敲第 4 节的命令。
2. `.clangd` 文件 demo.zip 里已经带好了;自己手写项目的记得建(见 §3)。
3. Windows 下如果还是报找不到头文件,先运行 `where gcc` 看实际路径,把 `.clangd` 里对应行改成那个目录,再在命令面板(**Ctrl+Shift+P**)执行 `clangd: Restart language server` 生效。

## 6. 常见问题

| 问题 | 解决 |
|---|---|
| 终端提示 `gcc`/`cmake`/`ninja` 不是内部命令 | PATH 没生效:重开终端和 VSCode;choco 安装的确认是否用了管理员 PowerShell |
| `choco install` 报权限错误 | 用管理员身份运行 PowerShell |
| clangd 不提示 / 满屏找不到头文件 | 确认项目根有 `.clangd`(见 §3),且 `build/compile_commands.json` 存在(先跑过 `cmake -B build -G Ninja`) |
| `cmake` 报错找不到 Ninja | 没装 ninja,或 PATH 没生效 |
| Windows 运行 exe 报缺 DLL | mingw 的 bin 已由 choco 自动加入 PATH,重开终端;还缺就把 `C:\ProgramData\mingw64\mingw64\bin` 手动加进 PATH |
| 编译报 `undefined reference` | 链接问题:用 `target_link_libraries` 把对应的库链上(见附录) |

# 附录:CMake 基础语法(先把 demo 跑起来,再回来看每行是什么意思)

CMakeLists.txt 里的每一条命令都是一个"函数调用",格式统一为 `命令(参数1 参数2 ...)`,`#` 开头是注释。上面用到的命令逐个讲:

| 命令 | 参数 | 作用 |
|---|---|---|
| `cmake_minimum_required(VERSION 3.25)` | `VERSION` + 最低版本号 | 必须写在第一行,CMake 版本低于它直接报错 |
| `project(demo CXX)` | 第一个参数是项目名;后面可跟语言(C / CXX),可省略 | 声明项目名和使用语言 |
| `set(CMAKE_CXX_STANDARD 20)` | 变量名 + 值 | 给变量赋值;`${变量名}` 可取出变量内容。`CMAKE_` 开头的是 CMake 内置变量(标准、导出开关等) |
| `add_executable(demo main.cpp)` | 目标名 + 源文件列表(空格分隔,可写多个,可写相对路径) | 把源文件编译成**可执行程序**,目标名在项目里唯一 |

再补充两个以后一定会用到的:

| 命令 | 参数 | 作用 |
|---|---|---|
| `add_library(myutils utils.cpp)` | 同 add_executable | 把源文件编成**库**(静态库),供其他目标链接 |
| `target_link_libraries(demo PRIVATE myutils)` | 目标名 + `PRIVATE`/`PUBLIC` + 库列表 | 给目标**链接库**。库可以是自己 `add_library` 建的,也可以是系统/第三方库(以后接 OpenCV 就写 `target_link_libraries(demo PRIVATE opencv_world)`) |

> `PRIVATE` / `PUBLIC` 是链接属性的可见范围:初学记一句话——**自己写代码用 `PRIVATE` 就够了**(PUBLIC 会把依赖传递给链接本目标的下游目标,用到再说)。

以后项目会长这样:

```cmake
cmake_minimum_required(VERSION 3.25)
project(demo CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(myutils utils.cpp)               # 库
add_executable(demo main.cpp)                # 可执行程序
target_link_libraries(demo PRIVATE myutils)  # 程序链接库
```