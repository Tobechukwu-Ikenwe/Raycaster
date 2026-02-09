# Find the Green Door

## Game overview

- **Goal:** Find the **golden key**, pass the **brown locked door**, then reach the **green door** to win.
- **Maze:** 24×24 dungeon with corridors, rooms, and dead ends.
- **Renderer:** Classic raycasting (GPU GLSL or CPU fallback). Walls and floor/ceiling with distance shading.
- **UI:** Timer (countdown), elapsed time, score; **minimap at bottom**; key pickup notification.

### Controls (first-person)


<img width="1596" height="936" alt="Screenshot 2026-02-06 001042" src="https://github.com/user-attachments/assets/0fed2113-6d9c-4f66-b533-f26c6a64b504" />


- **W** → North (up on minimap)  
- **S** → South (down on minimap)  
- **A** → West (left on minimap)  
- **D** → East (right on minimap)  
- **Mouse** → Rotate view  
- **SPACE** → Start game (on title screen)  
- **ESC** → Quit  

WASD is map-aligned for easier navigation with the minimap.

### Game mechanics

- **Timer:** Countdown from 120 seconds. Game over when it reaches 0 (score 0).
- **Score:** Time remaining when you reach the green door (1 point per second).
- **Win:** Reach the green door with the key → "You Win!" plus time taken and score.


<img width="1595" height="928" alt="Screenshot 2026-02-06 001222" src="https://github.com/user-attachments/assets/9641c3d9-f1ca-4f7f-b27c-66f53f878a5e" />

---

## Build & run

### Windows (native)

Requires: Visual Studio 2022 (or Build Tools) with C++ workload, CMake, Git.

```powershell
.\build_windows.ps1
```

The script will install CMake via winget if needed, clone vcpkg, install SDL2 and SDL2_ttf, build, and launch the game. Output: `build\Release\raycaster.exe`.

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
