# Zuma（命令行祖玛）

这是一个命令行版的祖玛彩球连锁消除游戏。程序从 `config/level1.txt`、`config/level2.txt`、`config/level3.txt` 读取关卡，在终端显示回合、总分、轨道、等待区、当前球和下一个球，支持发射、丢弃、撤销、普通消除、连锁消除、轨道自动增长以及胜负判断。

## 1. 环境要求

- CMake 3.16 或更高版本
- 支持 C++17 的编译器：
  - Windows + MinGW-w64（本机实际验证使用 `g++ 16.1.0`）
  - Visual Studio / MSVC、Clang 等也可使用
- Git Bash、PowerShell 或 Windows 终端均可

不需要第三方运行库。CMake 只负责生成构建文件，不修改系统环境。

## 2. 构建

以下命令在项目根目录执行。项目根目录是包含 `CMakeLists.txt`、`main.cpp`、`src/` 和 `config/` 的 `Zuma` 目录。

### 2.1 Windows + Git Bash + MinGW Makefiles（本机已验证）

```bash
cd /d/vscode/DataStructure/PJ1/Zuma
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

生成的可执行文件：

```text
build/zuma.exe
```

### 2.2 Visual Studio 等多配置生成器

如果本机安装了 Visual Studio，可以换一个独立构建目录，避免和上面的 MinGW 构建混用：

```bash
cd /d/vscode/DataStructure/PJ1/Zuma
cmake -S . -B build-vs
cmake --build build-vs --config Release
```

多配置生成器的 Release 可执行文件通常位于：

```text
build-vs/Release/zuma.exe
```

CMake 配置会把 `config/` 复制到构建根目录，构建后还会把它复制到可执行文件所在目录，因此从项目根目录、构建根目录或可执行文件目录启动都能找到关卡文件。当前 `level1.txt`、`level2.txt`、`level3.txt` 都在 `config/` 中，构建时整体复制，不需要为第三关单独修改 CMake。

## 3. 运行

### 3.1 从项目根目录启动（推荐）

```bash
cd /d/vscode/DataStructure/PJ1/Zuma
./build/zuma.exe
```

如果使用的是 Visual Studio 多配置构建：

```bash
./build-vs/Release/zuma.exe
```

### 3.2 从构建目录启动

```bash
cd /d/vscode/DataStructure/PJ1/Zuma/build
./zuma.exe
```

从 `build/` 启动时使用 CMake 复制到 `build/config/` 的关卡文件。请不要直接修改构建目录里的副本；需要改关卡时修改项目根目录的 `config/`。修改后重新运行上面的 CMake 配置和构建命令可刷新构建目录中的副本，也可以直接从项目根目录启动以使用源 `config/`。

如果从其他任意目录启动，程序会按相对路径 `config/level1.txt` 查找关卡；该目录下没有 `config/` 时会明确报错并退出，而不会崩溃。

## 4. 操作说明

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

`Round` 初始为 0，表示已完成的有效回合数；`Points` 初始为 0。每次成功发射或丢弃后，`Round` 加 1；即使等待区为空、没有球进入轨道，这个回合也会计数。非法输入和输入结束（EOF）不增加回合，也不消耗球或增长轨道。

可选操作：

- 输入 `s`：发射当前球。随后输入插入位置，范围是 `0` 到当前轨道球数；`0` 表示插到最前面，等于球数表示追加到末尾。
- 输入 `q`：丢弃当前球。丢弃本身不触发消除；`q` 始终表示丢弃，不是退出。
- 输入 `r`：撤销上一有效回合，回到该回合开始前的完整状态。连续输入 `r` 可以逐回合回退；没有历史时会提示且不改变状态。`r` 本身不消耗球、不增加回合或分数。

示例（选择关卡 1，发射当前球到下标 3，然后结束输入）：

```text
1
s
3
```

撤销示例（选择关卡 1，发射到下标 2，然后撤销）：

```text
1
s
2
r
```

发射后显示 `Round 1`、`Points 24`；输入 `r` 后回到 `Round 0`、`Points 0`，轨道恢复为 `RRBBRR`，等待区、当前球和下一个球也恢复到发射前。

输入结束后程序会打印 `Input ended.` 并正常退出。要继续游戏，在每回合提示时继续输入 `s`、`q` 或 `r`。

有效回合顺序为：发射或丢弃当前球（并轮转弹药）→ 发射时进行普通消除和连锁消除 → 等待区加入一个球（等待区为空则跳过）→ `Round` 加 1 → 判断胜负。

- 轨道和等待区同时为空：胜利。
- 轨道球数超过 `MAX_LENGTH`：失败。
- 弹药按 `AMMO` 循环，不会耗尽。
- 胜利或失败时，程序会先打印最终盘面（包含最终的 `Round` 和 `Points`），再显示 `You win!` 或 `Nice try.`，并提示输入 `r` 撤销最后一回合或按 Enter 退出。初始即胜利或失败时没有历史，直接结束，`Round` 和 `Points` 均为 0。
- 撤销不执行发射或丢弃，不消耗球，不增加回合或分数，也不创建新的历史记录。

### 4.1 计分规则

- 每次消除一个连续同色组（至少 3 个球）得 `2^n` 分，其中 `n` 是该组的球数；例如 3 连得 8 分，4 连得 16 分。
- 连锁消除时，每个被消除的组分别累加；例如先消 3 连、再连锁消 4 连，共得 `8 + 16 = 24` 分。
- 丢弃或不产生消除的发射得 0 分。
- 当前实现按每个消除组累加，不额外乘连锁倍率。

### 4.2 撤销操作

- 每次成功发射或丢弃前，程序保存一份完整的回合开始快照；输入 `r` 恢复轨道、等待区、弹药顺序、当前球、下一个球、`Round` 和 `Points`。
- 非法操作、非法位置、重试、输入结束（EOF）和 `r` 本身都不会创建快照。
- 连续输入 `r` 会逐个回退，直到初始状态；没有可撤销回合时提示 `No previous round to undo.`，状态不变。
- 胜负终局也会提示 `r` 撤销最后一回合；按 Enter（或 EOF）安全退出。初始即胜负时没有历史，直接结束。
- 撤销后执行新的发射或丢弃，以恢复后的状态为准；`q` 仍然只表示丢弃，不是退出。

## 5. 关卡文件

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

`level1.txt`、`level2.txt` 保持原有内容；`level3.txt` 使用相同颜色字符，初始没有三连。

字段含义：

| 字段 | 含义 |
|---|---|
| `ACTIVE` | 游戏开始时已经在轨道里的球，可以为空 |
| `FEED` | 之后依次进入轨道的球，可以为空 |
| `AMMO` | 弹药循环规律；第一个字符是初始当前球，第二个字符是初始下一个球，到末尾后从头循环，不能为空 |
| `MAX_LENGTH` | 轨道允许的最大球数，必须是非负整数 |

字母表示不同颜色。程序不要求颜色一定是某种固定字母，但同一字母代表同一颜色。`ACTIVE`、`FEED` 可以为空；如果两者都为空，程序启动后立即判定胜利；如果初始 `ACTIVE` 已经超过 `MAX_LENGTH`，程序会立即判定失败。

## 6. 异常处理

- 非法关卡编号、非法操作字符、非数字位置、越界位置：提示后继续读取，不会消耗球，也不会推进回合、分数或历史。
- 无历史时输入 `r`：提示 `No previous round to undo.`，状态不变。
- 终局输入 `r`：恢复最后一回合开始状态并继续；终局按 Enter 或 EOF：正常退出。
- 输入结束（EOF）：打印 `Input ended.` 并以退出码 `0` 结束。
- 关卡文件不存在：输出 `ERROR: Cannot open level file: ...`，以退出码 `1` 结束。
- 必需字段缺失、`MAX_LENGTH` 非数字或不是非负整数、`AMMO` 为空、颜色字段内部含空白：输出明确的 `ERROR:` 信息，以退出码 `1` 结束。
- 头文件有防重复包含保护；关卡加载不会返回带空弹药队列的有效游戏对象。

## 7. 当前限制

- 仅提供命令行界面，不包含图形界面。
- 内置并接受关卡 1、2、3；没有关卡编辑、提示、存档、随机关卡等功能。
- 撤销仅保留本次运行的内存快照，可连续回退到本次运行开始，不跨进程保存，也没有重做功能。
- 得分按上述每组 `2^n` 累加，不另设连锁倍率。
- 颜色使用单个字母表示。
- CMake 配置本身不绑定某一个编译器；本机实际验证的是 Windows + MinGW Makefiles + g++ 16.1.0。其他生成器按上面的多配置路径使用。
