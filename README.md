# DaoGame

C++20 narrative RPG runtime prototype.

## Toolchain

This project is configured for Windows + MinGW UCRT64:

- CMake 3.28+
- Ninja
- MSYS2 UCRT64 GCC at `C:/msys64/ucrt64/bin/g++.exe`
- Project-local vcpkg at `.vcpkg/`

The CMake presets use the vcpkg manifest in `vcpkg.json` and the `x64-mingw-dynamic` triplet.

## Dependencies

Managed by vcpkg:

- SDL3
- FreeType
- HarfBuzz
- miniaudio
- nlohmann-json
- stb
- Dear ImGui with SDL3 and SDL3 Renderer bindings

## First Setup

```powershell
git clone https://github.com/microsoft/vcpkg.git .vcpkg
.\.vcpkg\bootstrap-vcpkg.bat
cmake --preset mingw-debug
```

## Build

```powershell
cmake --build build\mingw-debug
```

## Run

```powershell
.\build\mingw-debug\DaoGame.exe
```

For dependency verification:

```powershell
.\build\mingw-debug\DaoGameDependencySmoke.exe
```

The production entry point is `src/main.cpp`. Third-party dependency checks live in `tests/dependency_smoke.cpp`.

## CLion

Open the project directory and select the `mingw-debug` CMake preset. The preset already points to the local vcpkg toolchain and MSYS2 UCRT64 compilers.

## VSCode

Recommended extensions:

- C/C++
- CMake Tools

Open the project directory in VSCode, then:

- Press `Ctrl+Shift+B` to build the debug executable.
- Press `F5` and select `Debug DaoGame` to configure, build, and launch under GDB.
- Run the `Run DaoGame` task to configure, build, and start without the debugger.
- Run the `Smoke Test Dependencies` task for a quick third-party dependency check.
