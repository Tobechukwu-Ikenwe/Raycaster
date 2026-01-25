# My Raycaster Demo (SDL2 CPU)

## Overview

This project is my implementation of a **3D raycaster using SDL2 for CPU rendering**, showing per-pixel wall projection in real-time.  

I deliberately chose SDL2 instead of jumping straight to GLSL shaders, because I wanted **full control over every calculation on the CPU** and a clear understanding of the GPU-CPU relationship.  

This project serves as a **baseline for performance, debugging, and algorithm verification**, before I move to GPU shaders for acceleration and advanced effects.

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
The CPU version ensures that all calculations are correct, which makes the shader version much easier to implement and debug.


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

How to Run

Navigate to the project directory
Verify all files are present:

ls
main.cpp raycaster.cpp raycaster.h player.h map.cpp map.h



# Run

Linux / macOS

```bash

make

./raycaster
```

Windows

```bash

make

raycaster.exe
```

You should see a window with the ceiling, floor, and walls rendered.
