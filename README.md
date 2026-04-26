# defPlatformGame

A C++ platformer game engine and sample game.

## Features
- Custom game engine with Lua scripting support (via sol2)
- Dynamic entities: player, enemies (mushroom, turtle)
- Level loading and management
- Configurable window and pixel settings via `Assets/config.lua`
- Logging and asset management subsystems

## Getting Started
1. Clone the repository **recursively** to include all submodules:
   ```sh
   git clone --recursive https://github.com/defini7/defPlatformGame.git
   ```
   If you already cloned without `--recursive`, run:
   ```sh
   git submodule update --init --recursive
   ```
2. Build the project using your preferred C++ build system (e.g., Visual Studio, CMake, etc.).
3. Run the game. Configuration is loaded from `Game/Assets/config.lua`.

## Dependencies
- [sol2](https://github.com/ThePhD/sol2)
- [lua](https://www.lua.org/)
- [glfw](https://www.glfw.org/)
