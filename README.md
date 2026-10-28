# Zuma（命令行 + raylib 图形界面）

这是一个祖玛彩球连锁消除游戏，包含两个可执行入口：

- `zuma`：命令行版，从 `config/level1.txt`、`config/level2.txt`、`config/level3.txt` 读取关卡，在终端显示回合、总分、轨道、等待区、当前球和下一个球。
- `zuma_gui`：raylib 图形版，使用窗口绘制彩球和状态，通过键盘完成选关、发射、丢弃和撤销。

两个入口共用 `Zuma` 类，因此消除、连锁、弹药轮转、回合、计分、撤销和胜负规则一致。

## 1. 环境要求

- CMake 3.16 或更高版本。
- 同时支持 C 和 C++17 的编译器。CMake 工程启用了 C 和 CXX，因此即使只构建命令行版，也需要 C 编译器；MinGW 安装通常同时提供 `gcc` 和 `g++`。
- Windows + MinGW-w64 的已验证环境为 `g++ 16.1.0`；Visual Studio/MSVC、Clang 也可用于命令行版。
- 图形界面还需要：
  - 可用的 OpenGL 图形环境/显卡驱动；
  - raylib 5.5 源码，放在项目内的 `third_party/raylib`，不需要安装到系统目录。
- PowerShell、Git Bash 或 Windows 终端均可。

命令行版不需要第三方库。图形界面使用静态链接的 raylib，不需要复制 raylib DLL；但把程序复制到没有 MinGW 运行库的机器时，可能还需要 `libstdc++-6.dll`、`libgcc_s_seh-1.dll`、`libwinpthread-1.dll` 等编译器运行库，不能保证单个 exe 独立运行。

## 2. 获取 raylib（仅图形界面需要）

在项目根目录执行：

```powershell
Set-Location 'D:/vscode/DataStructure/PJ1/Zuma'
New-Item -ItemType Directory -Force third_party | Out-Null
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git third_party/raylib
Test-Path ./third_party/raylib/CMakeLists.txt
```

最后一行应输出 `True`。这里固定使用 raylib 5.5，避免版本漂移，不表示它是最新版本。

如果 Git 下载失败，可以用浏览器下载：

```text
https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip
```

解压后保证目录结构为：

```text
Zuma/
├── third_party/
│   └── raylib/
│       ├── CMakeLists.txt
│       └── src/
├── incl/
├── src/
├── config/
├── main.cpp
├── zuma_gui.cpp
└── CMakeLists.txt
```

`third_party/raylib` 是本地依赖目录，当前未纳入 Git；换机器或重新克隆后需要重新获取。不要提交 `build/`、`build-gui/`，也不要把 `third_party/raylib` 中的嵌套 `.git` 当作项目源码提交。

## 3. 构建

以下命令均在项目根目录执行。项目根目录是包含 `CMakeLists.txt`、`main.cpp`、`zuma_gui.cpp`、`src/` 和 `config/` 的 `Zuma` 目录。

### 3.1 命令行版（不需要 raylib）

```bash
cd /d/vscode/DataStructure/PJ1/Zuma
cmake -S . -B build -G "MinGW Makefiles" -DZUMA_BUILD_GUI=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

生成的程序：

```text
build/zuma.exe
```

CMake 选项 `ZUMA_BUILD_GUI` 默认为 `OFF`；这里再显式写 `OFF`，即使 `build/` 里残留了之前的 `ON` 缓存，也不会构建 GUI，不会要求 `third_party/raylib` 存在。

### 3.2 图形界面版（PowerShell + MinGW + raylib）

先按第 2 节准备好 `third_party/raylib`，再使用独立的 `build-gui` 目录：

```powershell
Set-Location 'D:/vscode/DataStructure/PJ1/Zuma'
cmake -S . -B build-gui -G 'MinGW Makefiles' '-DZUMA_BUILD_GUI:BOOL=ON' '-DCMAKE_BUILD_TYPE:STRING=Release' '-DCMAKE_POLICY_VERSION_MINIMUM:STRING=3.5'
cmake --build build-gui --target zuma_gui --config Release
```

说明：

- `-DZUMA_BUILD_GUI:BOOL=ON` 打开 GUI target；默认关闭。
- 每个 `-D` 参数整体用单引号包住，尤其是 `'-DCMAKE_POLICY_VERSION_MINIMUM:STRING=3.5'`，避免 PowerShell 把 `3.5` 拆成 `3` 和 `.5`。
- `CMAKE_POLICY_VERSION_MINIMUM` 用于兼容 CMake 4.x 与 raylib 5.5 的旧策略声明；使用 CMake 3.x 时可以去掉这一项。
- 不要复用命令行版的 `build/`，以免缓存和 GUI 配置互相影响。

生成的可执行文件：

```text
build-gui/zuma_gui.exe
```

如果 `zuma_gui.cpp` 缺失，CMake 会在配置阶段报找不到源文件；发布 GUI 版本时请确认该入口文件随源码提供。

### 3.3 Visual Studio 等多配置生成器（命令行版）

如果本机安装了 Visual Studio，可以换一个独立构建目录：

```bash
cd /d/vscode/DataStructure/PJ1/Zuma
cmake -S . -B build-vs -DZUMA_BUILD_GUI=OFF
cmake --build build-vs --config Release
```

多配置生成器的 Release 可执行文件通常位于：

```text
build-vs/Release/zuma.exe
```

上面的 GUI 命令按 MinGW Makefiles 编写，Visual Studio 下的 GUI 构建未在本说明中验证。

## 4. 运行

### 4.1 命令行版

从项目根目录启动：

```bash
cd /d/vscode/DataStructure/PJ1/Zuma
./build/zuma.exe
```

也可以从构建目录启动：

```bash
cd /d/vscode/DataStructure/PJ1/Zuma/build
./zuma.exe
```

### 4.2 图形界面版

```powershell
Set-Location 'D:/vscode/DataStructure/PJ1/Zuma'
./build-gui/zuma_gui.exe
```

启动后出现 1280×720 的窗口，按数字键选关，按 `Esc` 或窗口关闭按钮退出。

关卡路径是相对路径 `config/levelN.txt`。CMake 配置时会把 `config/` 复制到构建根目录，构建后还会复制到可执行文件所在目录，因此从项目根目录、`build/`、`build-gui/` 或 Visual Studio 的 `Release/` 目录启动都能找到关卡文件。如果从其他目录启动，且该目录下没有 `config/`，程序会提示加载失败。

## 5. 操作说明

### 5.1 命令行版

启动后先选择关卡：

```text
Please select a difficulty level (1, 2, or 3) :
```

输入 `1`、`2` 或 `3`。

每个回合显示：

```text
===== Round 1 | Points 24 =====  # 已完成的有效回合数与总分
Current active balls: ...         # 当前轨道
Balls to appear: ...              # 等待进入轨道的球
Current ammo: ...                 # 当前球
Next ammo: ...                    # 下一个球
```

可选操作：

- 输入 `s`：发射当前球。随后输入插入位置，范围是 `0` 到当前轨道球数；`0` 表示插到最前面，等于球数表示追加到末尾。
- 输入 `q`：丢弃当前球。丢弃本身不触发消除；`q` 始终表示丢弃，不是退出。
- 输入 `r`：撤销上一有效回合，回到该回合开始前的完整状态。连续输入 `r` 可以逐回合回退；没有历史时会提示且不改变状态。
- 输入结束（EOF）会打印 `Input ended.` 并正常退出。

有效回合顺序为：发射或丢弃当前球（并轮转弹药）→ 发射时进行普通消除和连锁消除 → 等待区加入一个球（等待区为空则跳过）→ `Round` 加 1 → 判断胜负。

### 5.2 图形界面版

| 按键 | 功能 |
|---|---|
| `1 / 2 / 3` | 在开始页面选择第一、二、三关 |
| `← / →` | 选择插入位置，范围是 `0` 到当前轨道球数，包含末尾位置 |
| `Space` | 发射当前球 |
| `Q` | 丢弃当前球，不触发消除 |
| `R` | 撤销上一有效回合，终局也可以使用 |
| `Esc` | 关闭窗口并退出 |

选关后不能返回选关页面；要更换关卡，请关闭程序后重新启动。

图形界面用颜色圆球加字母显示球，顶部显示关卡、`Round`、`Points` 和轨道球数，并显示等待区、当前球和下一个球。界面文字为英文，默认字体不含中文字形，暂不显示中文。

胜负出现后，界面显示 `YOU WIN!` 或 `NICE TRY.`，此时发射和丢弃被忽略，仍可按 `R` 撤销最后一回合；按 `Esc` 或关闭窗口退出。

## 6. 回合与计分

- `Round` 初始为 0，表示已完成的有效回合数；`Points` 初始为 0。
- 每次成功发射或丢弃后，`Round` 加 1；即使等待区为空、没有球进入轨道，这个回合也会计数。
- 命令行版的非法输入、非法位置和输入结束不增加回合，也不消耗球或增长轨道。
- 每次消除一个连续同色组（至少 3 个球）得 `2^n` 分，其中 `n` 是该组的球数；例如 3 连得 8 分，4 连得 16 分。
- 连锁消除时，每个被消除的组分别累加；例如先消 3 连、再连锁消 4 连，共得 `8 + 16 = 24` 分。
- 丢弃或不产生消除的发射得 0 分。

示例（命令行版，选择关卡 1，发射当前球到下标 2）：

```text
1
s
2
```

应显示 `Round 1`、`Points 24`，轨道只剩等待区加入的 `G`。

## 7. 撤销

- 命令行版输入 `r`，图形界面按 `R`。
- 每次成功发射或丢弃前，程序保存一份完整的回合开始快照；撤销会恢复轨道、等待区、弹药顺序、当前球、下一个球、`Round` 和 `Points`。
- 非法操作、非法位置、输入结束（EOF）和撤销本身都不会创建快照。
- 连续撤销会逐个回退，直到本次运行开始；没有可撤销回合时，命令行版提示 `No previous round to undo.`，图形界面显示同一提示。
- 胜负终局也可以撤销最后一回合；撤销后胜负状态清除，可以继续操作。
- 撤销只保留本次运行的内存快照，不跨进程保存，也没有重做功能。

## 8. 关卡文件

关卡文件位于：

```text
config/level1.txt
config/level2.txt
config/level3.txt
```

格式示例：

```text
ACTIVE=RRBBRR
FEED=GBYPRGBGYRBPYG
AMMO=BBGYRRGBYPRG
MAX_LENGTH=12
```

`level3.txt` 的初始轨道为 10 个球，上限为 20：

```text
ACTIVE=RRGGBBYYPP
FEED=RGBYPRGBYPRGBY
AMMO=RGBYPRGBYP
MAX_LENGTH=20
```

字段含义：

| 字段 | 含义 |
|---|---|
| `ACTIVE` | 游戏开始时已经在轨道里的球，可以为空 |
| `FEED` | 之后依次进入轨道的球，可以为空 |
| `AMMO` | 弹药循环规律；第一个字符是初始当前球，第二个字符是初始下一个球，到末尾后从头循环，不能为空 |
| `MAX_LENGTH` | 轨道允许的最大球数，必须是非负整数 |

字母表示不同颜色。程序不要求颜色必须是某种固定字母，但同一字母代表同一颜色。`ACTIVE`、`FEED` 可以为空；如果两者都为空，程序启动后立即判定胜利；如果初始 `ACTIVE` 已经超过 `MAX_LENGTH`，程序会立即判定失败。

## 9. 异常处理

命令行版：

- 非法关卡编号、非法操作字符、非数字位置、越界位置：提示后继续读取，不会消耗球，也不会推进回合、分数或历史。
- 无历史时输入 `r`：提示 `No previous round to undo.`，状态不变。
- 终局输入 `r`：恢复最后一回合开始状态并继续；终局按 Enter 或 EOF：正常退出。
- 输入结束（EOF）：打印 `Input ended.` 并以退出码 `0` 结束。
- 关卡文件不存在：输出 `ERROR: Cannot open level file: ...`，以退出码 `1` 结束。
- 必需字段缺失、`MAX_LENGTH` 非数字或不是非负整数、`AMMO` 为空、颜色字段内部含空白：输出明确的 `ERROR:` 信息，以退出码 `1` 结束。

图形界面版：

- 关卡加载失败时，窗口保留在选关页面并显示 `Load failed: ...`，可以重新按 `1/2/3` 选择；按 `Esc` 或关闭窗口退出。
- 插入位置由左右方向键限制在 `0..当前球数`，不会提交越界位置。
- 胜负出现后，发射和丢弃被忽略，但仍可按 `R` 撤销。

## 10. 当前限制

- 图形界面使用英文界面和默认字体，暂不显示中文；没有鼠标操作、动画和音效。
- 图形界面每次启动只能选择一个关卡，不能返回选关页面；更换关卡需要重启程序。
- 内置并接受关卡 1、2、3；没有关卡编辑、提示、存档、随机关卡等功能。
- 撤销仅保留本次运行的内存快照，可连续回退到本次运行开始，不跨进程保存，也没有重做功能。
- 得分按每个消除组的 `2^n` 累加，不另设连锁倍率。
- 颜色使用单个字母表示。
- `.vscode/launch.json` 的调试程序仍指向 `build/Debug/outDebug`，当前 CMake 构建不会生成该文件，F5 调试可能失败；请使用本文的终端命令运行。
- GUI 命令按 Windows + MinGW Makefiles 编写；Visual Studio 下的 GUI 构建未验证。

## 11. 验证范围

- 命令行版的构建、运行和主要交互此前已在 Windows + MinGW Makefiles + `g++ 16.1.0` 下验证。
- 图形界面此前已完成 raylib 最小窗口的编译和启动验证；完整图形界面的按键、撤销、计分和胜负流程请按本文操作说明运行确认。
- 本文档更新本身没有重新构建或运行测试。
