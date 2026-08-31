# DaoGame

> **C++20 叙事 RPG 运行时原型——目前只有"runtime entry"和"third-party 依赖冒烟测试"两个 target，是为后续重资产项目铺底座的最小可复现工程。**

## 项目定位

DaoGame 是一个**叙事向 RPG 运行时**的工程底座。它选定的运行时栈是 **SDL3（窗口/事件/渲染） + FreeType + HarfBuzz（高质量多语言文字排版） + miniaudio（音频） + nlohmann::json（数据） + Dear ImGui（编辑器/调试 UI）**——这几乎是当代用 C++ 做 2D 独立游戏最经典的一组组合。

当前阶段：仓库只提交了**两件**事。

1. **生产入口** `src/main.cpp`：只有一行 `std::cout << "DaoGame runtime entry\n"; return 0;`。
2. **依赖冒烟测试** `tests/dependency_smoke.cpp`：独立可执行文件 `DaoGameDependencySmoke`，依次验证：
   - SDL3 初始化视频/音频/事件 + 创建一个隐藏窗口 + 创建一个 renderer + 跑 3 帧的最小渲染循环（画一个深色背景 + 一个暗灰面板 + 一个金色方块）；
   - FreeType 库的 `FT_Init_FreeType / FT_Done_FreeType`；
   - HarfBuzz 的 `hb_buffer_create / hb_buffer_add_utf8 / hb_buffer_guess_segment_properties / hb_buffer_destroy`；
   - miniaudio 的 `ma_engine_config_init`（只检查头文件能编过）；
   - nlohmann::json 构造一个简单的 manifest JSON 并 `dump()`。

它是一个**对工具链、CMake 预设、vcpkg 集成都做了完整工程化**的最小骨架，但**游戏本体一行没写**。`vcpkg.json` 中还为 imgui 声明了 `sdl3-binding` / `sdl3-renderer-binding` / `freetype` features，提示后续会以 ImGui 写调试 UI。

## 仓库结构

```
DaoGame/
├── CMakeLists.txt                  # 2 个 target：DaoGame + DaoGameDependencySmoke
├── CMakePresets.json               # mingw-debug / mingw-release 预设
├── vcpkg.json                      # 依赖清单（7 个 + imgui features）
├── .gitignore
├── src/
│   └── main.cpp                    # 真正的运行时入口（目前只打印一行）
└── tests/
    └── dependency_smoke.cpp        # 第三方依赖全栈冒烟测试
```

## 技术栈

| 层 | 选型 |
|---|---|
| 语言 | C++20（`CMAKE_CXX_STANDARD 20`，禁用 GNU 扩展 `CMAKE_CXX_EXTENSIONS OFF`） |
| 构建 | CMake ≥ 3.28 + `CMakePresets.json`（两个 MinGW UCRT64 预设，generator 固定为 Ninja） |
| 编译器 | MSYS2 UCRT64 GCC（`C:/msys64/ucrt64/bin/g++.exe`） |
| 依赖管理 | **项目本地 vcpkg**（`.vcpkg/`，通过 `CMAKE_TOOLCHAIN_FILE` 接入，三元组 `x64-mingw-dynamic`） |
| 窗口 / 输入 / 渲染 | **SDL3**（`SDL3::SDL3`） |
| 字体 | **FreeType**（`Freetype::Freetype`） |
| 文字 shaping | **HarfBuzz**（`harfbuzz::harfbuzz`） |
| 音频 | **miniaudio**（header-only，源码嵌入） |
| JSON | **nlohmann/json**（`nlohmann_json::nlohmann_json`） |
| 图像 | **stb**（`find_package(Stb REQUIRED)`，`Stb_INCLUDE_DIR` 提供给冒烟测试） |
| 调试 UI | **Dear ImGui**（带 `sdl3-binding` / `sdl3-renderer-binding` / `freetype` features，**当前未在 main.cpp 中使用**） |
| 警告等级 | MSVC `/W4 /permissive-`；GCC/Clang `-Wall -Wextra -Wpedantic`（所有 target 一致） |

## 核心模块

### 1. `src/main.cpp` —— 运行时入口
- 单文件，9 行；
- 当前只打印 `"DaoGame runtime entry"`，**无任何 SDL3 初始化**——也就是说，运行时入口在 main 阶段还没接 SDL3。
- CMake target 名称：`DaoGame`，`target_compile_features(... cxx_std_20)`。

### 2. `tests/dependency_smoke.cpp` —— 第三方依赖冒烟测试
独立 target `DaoGameDependencySmoke`：
- `SdlSmokeApp` 内部 RAII：
  - 构造时：`SDL_Init(VIDEO|AUDIO|EVENTS)` + `SDL_CreateWindow(..., SDL_WINDOW_HIDDEN)` + `SDL_CreateRenderer(window, nullptr)`；
  - 析构时反向销毁。
- `render_frames(frame_count)`：每帧先 `SDL_PollEvent` 排空事件，然后清屏（`14,16,20,255` 深色）→ 画一个深灰面板（`36,43,54,255`）→ 画一个金色方块（`211,180,104,255`，48px 边长）→ `SDL_RenderPresent`；
- `verify_third_party_headers()`：
  - FreeType：`FT_Init_FreeType(&lib)` + `FT_Done_FreeType(lib)`；
  - HarfBuzz：建 buffer → 灌入 `"DaoGame"` → `hb_buffer_guess_segment_properties` → 销毁；
  - miniaudio：调一次 `ma_engine_config_init`（不真正初始化引擎）；
  - nlohmann::json：构造 `{target, libraries}` manifest 并 `dump()`。
- `main` 用 `try / catch` 包住，错误时 `std::cerr << error.what() << '\n'; return 1;`。

### 3. CMake 工具链
- `find_package(SDL3 CONFIG REQUIRED)` / `find_package(Freetype REQUIRED)` / `find_package(harfbuzz CONFIG REQUIRED)` / `find_package(nlohmann_json CONFIG REQUIRED)` / `find_package(Stb REQUIRED)`；
- `find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h" REQUIRED)`——因为 miniaudio 是 header-only，单独走 `find_path` 拿头文件路径；
- 两个 target 都开 `cxx_std_20`；
- `DAOGAME_TARGETS` 列表里统一加 MSVC `/W4 /permissive-` 或 GCC `-Wall -Wextra -Wpedantic`。

### 4. CMakePresets
两个 preset：
- `mingw-debug`：Debug 构建，开启 `CMAKE_EXPORT_COMPILE_commands`；
- `mingw-release`：Release 构建。
两者都使用项目本地 `.vcpkg/` 工具链，输出到 `build/mingw-{debug,release}/`。

## 已完成 / 进行中

- [x] CMake + Preset + vcpkg manifest 全套工程化
- [x] 7 个第三方依赖 + ImGui 全部 manifest 声明到位
- [x] 完整的依赖冒烟测试（SDL3 渲染 / FreeType / HarfBuzz / miniaudio / nlohmann::json）
- [x] 严格警告等级（`/W4 -permissive-` / `-Wall -Wextra -Wpedantic`）
- [ ] `src/main.cpp` 真正接入 SDL3（创建窗口、事件循环、渲染）
- [ ] 任何 RPG 子系统（剧情脚本系统、对话树、角色、状态机、存档）
- [ ] Dear ImGui 调试面板（vcpkg 里已声明但代码里还没出现）

## 本地构建

仓库自带 README 给了完整步骤（已被本次任务覆盖为当前 README，原 README 已包含）；要点：

```powershell
# 一次性：拉项目本地 vcpkg 并 bootstrap
git clone https://github.com/microsoft/vcpkg.git .vcpkg
.\.vcpkg\bootstrap-vcpkg.bat

# 配置 + 编译
cmake --preset mingw-debug
cmake --build build\mingw-debug

# 跑运行时（目前只打印一行）
.\build\mingw-debug\DaoGame.exe

# 跑依赖冒烟测试（应创建隐藏窗口 + 渲染 3 帧 + 打印 manifest）
.\build\mingw-debug\DaoGameDependencySmoke.exe
```

CLion / VSCode + CMake Tools 也能直接吃这两个 preset。

## 状态

- **版本**：0.1.0（依赖 + 冒烟测试，运行时仅占位）
- **架构阶段**：工具链验证期
- **可运行性**：可编译，运行时无任何游戏行为

## License

仓库内未附 LICENSE 文件，源码默认遵循 "All rights reserved"。
