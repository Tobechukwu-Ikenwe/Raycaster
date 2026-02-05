# Find the Green Door

## Game overview

- **Goal:** Find the **golden key**, pass the **brown locked door**, then reach the **green door** to win.
- **Maze:** 24×24 dungeon with corridors, rooms, and dead ends.
- **Renderer:** Classic raycasting (GPU GLSL or CPU fallback). Walls and floor/ceiling with distance shading.
- **UI:** Timer (countdown), elapsed time, score; **minimap at bottom** to navigate.

### Controls (first-person)

- **W / S** → Move forward / backward  
- **A / D** → Strafe left / right  
- **Mouse** → Rotate view  
- **SPACE** → Start game (on title screen)  
- **ESC** → Quit  

### Game mechanics

- **Timer:** Countdown from 120 seconds. Game over when it reaches 0 (score 0).
- **Score:** Time remaining when you reach the green door (1 point per second).
- **Win:** Reach the green door with the key → "You Win!" plus time taken and score.

---

## Build & run

### Windows (native)

Requires: Visual Studio 2022 (or Build Tools) with C++ workload, CMake, Git.

```powershell
.\build_windows.ps1
```

The script will install CMake via winget if needed, clone vcpkg, install SDL2, build, and launch the game. Output: `build\Release\raycaster.exe`.

Or build manually:

```powershell
# One-time: install vcpkg and SDL2
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg.exe install

# Configure and build
mkdir build; cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
cmake --build . --config Release
.\Release\raycaster.exe
```

### Linux / WSL

```bash
sudo apt install libsdl2-dev   # Ubuntu/Debian
make
./raycaster
```

OpenGL 3.3 recommended. Falls back to CPU raycaster at 1280×720 if GL is unavailable.

### macOS

```bash
brew install sdl2
make
./raycaster
```

---

## Project structure

- `src/` — main loop, renderer (GL + CPU), map, raycaster
- `include/` — headers
- `shaders/` — GLSL (embedded in renderer)
- `CMakeLists.txt` — CMake build (Windows + vcpkg)
- `Makefile` — Unix build
- `build_windows.ps1` — Windows build and run script

---

## License

See [LICENSE](LICENSE).
