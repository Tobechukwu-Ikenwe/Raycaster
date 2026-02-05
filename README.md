# Find the Door

## Game overview

- **Goal:** Find the **golden key**, then reach the **brown locked door**, then the **green exit** to win.
- **Maze:** Fixed 2D grid (16×16). Each cell is empty or wall; special cells: key, locked door, exit.
- **Renderer:** Classic raycasting (GPU GLSL or CPU fallback). Walls and floor/ceiling with distance shading (depth effect).
- **UI:** Timer (countdown), elapsed time, score on screen; **minimap at bottom** to help find the door.

### Controls (first-person)

- **W / S** → Move forward / backward  
- **A / D** → Strafe left / right  
- **Mouse** → Rotate view  
- **SPACE** → Start game (on title screen)  
- **ESC** → Quit  

### Game mechanics

- **Timer:** Countdown from 120 seconds. When it reaches 0, game over (score 0).
- **Score:** Time remaining when you reach the exit (1 point per second survived).
- **Win:** Reach the exit door with the key → full-screen “You Win!” plus time taken and score.

### Build & run (Linux / WSL)

```bash
sudo apt install libsdl2-dev   # Ubuntu/Debian
make
./raycaster
```

OpenGL 3.3-capable driver recommended (e.g. WSLg on Windows 11). If GL is unavailable, the game falls back to a CPU raycaster at 1280×720.

---

## Earlier: CPU raycaster (SDL2)

The codebase started as a **CPU raycaster** with SDL2 for learning. That version is replaced by the GL shader build above; the design and map layout are reused for Dungeon Run.

---

## I Implemented GL shaders

While GPU shaders are now fully integrated, I started with SDL2 CPU rendering deliberately before moving to shaders:

Transparent CPU-GPU Transition
SDL2 let me see exactly how my raycasting algorithm produces each vertical slice on the CPU. This made GPU acceleration and GLSL shaders much more intuitive once I implemented them.

Stepwise Debugging
Shaders can be opaque to debug. By first controlling every CPU calculation — math, angles, distances, collisions — I built a solid foundation that now translates cleanly into GLSL.

Minimal Boilerplate, Maximum Focus
SDL2 still handles windowing, input, and context setup, letting me focus purely on rendering logic. Adding GLSL shaders now fits seamlessly on top without disrupting the core engine.

CPU-Visible Optimization
Testing performance on the CPU first helped me experiment with frame rates, resolution, and ray step sizes. This informed the shader implementation, leading to better-optimized GPU code.
## Controls

- **W / S** → Move forward/backward  
- **A / D** → Rotate left/right  
- **ESC** → Exit the program  

---





# Prerequisites

# Linux / macOS
```bash

sudo apt install libsdl2-dev        # Ubuntu/Debian
brew install sdl2                   # macOS
```
# Windows

Download SDL2 development libraries: https://www.libsdl.org

Extract the include and lib folders

Ensure SDL2.dll is next to the executable

### Run

From the project directory run `make` then `./raycaster`. You should see a 2560×1440 window: find the key (gold wall in the inner room), then go to the green exit at the bottom.
