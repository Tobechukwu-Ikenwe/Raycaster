# Find the Door — First-person maze raycaster

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

## Why I Chose SDL2 Instead of GLSL Shaders

While GPU shaders are the standard for real-time graphics today, I **chose SDL2 CPU rendering deliberately**:

1. **Transparent CPU-GPU Relationship**  
   SDL2 lets me see exactly how my **raycasting algorithm produces each vertical slice** on the CPU before sending pixels to the GPU. This makes the path to **GPU acceleration and shaders much more intuitive**.

2. **Fine-Grained Debugging**  
   Debugging shaders can be opaque. With SDL2, I **control every calculation step**, making it easier to validate the math, angles, distances, and collision detection — all critical for building a robust engine.

3. **Minimal Setup, Maximum Focus**  
   SDL2 abstracts OS windowing and input. I can focus purely on **raycasting logic** without worrying about OpenGL boilerplate, which keeps the code clean and development efficient.

4. **Surprising Benefit — CPU-Visible Performance Scaling**  
   Using SDL2 first allows me to **experiment with frame rates, resolution, and ray step sizes**. Once the CPU algorithm is solid, moving to GPU shaders is straightforward. This “CPU-first” approach even helps me **write better-optimized shaders**, because I fully understand what each calculation does before handing it to the GPU.

> In short, using SDL2 is not a compromise — it’s a strategic choice to understand the full pipeline and prepare for a smooth transition to GLSL shaders.

---

## Controls

- **W / S** → Move forward/backward  
- **A / D** → Rotate left/right  
- **ESC** → Exit the program  

---


# My goals
I love coding and I also love helping people learn to code. i take the time to ensure that my code can be understood by all coders through my comments which contain analogies and explanations.


# Next Steps

Once it’s running, the next step is to move the raycaster to GLSL shaders, for full GPU acceleration and visual effects.
Dungeon Run uses GLSL shaders; possible next steps: more levels, textures, or a minimap.


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
