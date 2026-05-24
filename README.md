# 3D Minesweeper

A three-dimensional adaptation of the classic Minesweeper game written in C++ using the OpenGL library. The game takes the traditional flat board into a 3D space, offering players a new, spatial logic challenge with an interactive graphical user interface.

[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3%2B-%235586A4.svg)](https://www.opengl.org/)
[![Dear ImGui](https://img.shields.io/badge/Dear%20ImGui-purple.svg)](https://github.com/ocornut/imgui)

## About the Project

This project was created to develop 3D graphics programming skills and implement spatial game logic. Instead of a flat grid, the player operates on a cube consisting of smaller blocks (ranging from 3x3x3 to 7x7x7). An advanced collision detection system (Raycasting) has been implemented to allow accurate interaction with 3D objects using the mouse.

### Key Features
* **Spatial 3D Board:** Gameplay on a multi-layered cube.
* **Precise Raycasting:** An algorithm that converts the 2D cursor position into a 3D space ray, allowing accurate block selection.
* **Modern UI (HUD):** Floating windows powered by **Dear ImGui** that do not obscure the main game scene.
* **Dynamic Settings:** The ability to seamlessly change the cube size and mine count during gameplay.
* **Recursive Reveal:** An optimized Flood Fill algorithm that automatically reveals empty, adjacent blocks.
* **Standalone Executable:** The game is compiled with static linking (no external `.dll` files required to run).

---

## Controls

The game combines mouse input (for interacting with the board) and keyboard input (for spatial navigation).

| Key / Button | Action |
| :--- | :--- |
| **Left Mouse Button (LMB)** | Reveal the clicked block |
| **Right Mouse Button (RMB)** | Place / remove a flag (mark a mine) |
| **Arrows (Left / Right)** | Rotate the camera horizontally around the cube |
| **Arrows (Up / Down)** | Rotate the camera vertically around the cube |
| **ENTER** | Quick restart (New Game) |
| **ESC** | Exit game |

---

## Technologies & Libraries

The project relies on modern C++ practices and standard graphics libraries:

* **C++17** - Core game logic and engine
* **OpenGL 3.3+** - 3D graphics rendering
* **GLFW** - Window creation and input management (keyboard/mouse)
* **GLEW** - Loading OpenGL extensions
* **GLM** - Mathematics (matrices, vectors, transformations)
* **Dear ImGui** - Modern graphical user interface (Menu, Settings, Statistics)
* **LodePNG** - Decoding `.png` texture files

---

## How to Play (Quick Start)

You don't need to compile the project to play. The game has been prepared as a Standalone executable.

1. Go to the [Releases](../../releases) tab on GitHub.
2. Download the latest `.zip` package with the game.
3. Extract the archive anywhere on your computer.
4. Make sure that the `gkiw_st_07_win.exe` file, the `assets` folder (containing textures), and the shader files (`.glsl`) are all located in the same folder.
5. Run the `.exe` file and enjoy the game!

---

## Compilation (For Developers)

If you want to modify the source code, you will need a C++ development environment (recommended: **Visual Studio 2022/2026** on Windows).

1. Clone the repository:
   ```bash
   git clone [https://github.com/Kendrej/3Dgame.git](https://github.com/Kendrej/3Dgame.git)
