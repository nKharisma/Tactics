# Isometric Turn-Based Strategy (C++ / SDL2)

Compact turn-based isometric strategy prototype written in C++ using SDL2.  
Features: isometric tile map rendering, A* pathfinding, turn manager, simple AI, UI buttons, sprite atlas-based animations.

## Features
- Isometric grid map with walkable/non-walkable tiles  
- Turn-based system with TurnManager (player & AI units)  
- A* pathfinding for unit movement  
- Unit animations (idle, move, attack) via texture atlases  
- UI with clickable buttons and HUD (current turn, tile info)  
- Basic enemy AI: move toward and attack nearest player unit

## Project layout (important files)
- game/ — game source (unit, map, UI, turnmanager, aStar, game loop)  
- assets/ — texture atlases, fonts, map text files (place here)  
- json/atlas_parser.* — atlas parsing utility  
- build scripts / project files — optional (not included)

## Map file format
Map loader reads characters per tile. Supported chars:
- G = Green_Grass_Full (walkable)  
- S = Sand_Full (walkable)  
- D = Diagonal_Path_Green (walkable)  
- W = Water_Full (not walkable)  
- I = Straight_Path_Full (walkable)  
- T = T_Path_Full (walkable)  
- s = Sand_Half (walkable)  
- g = Green_Grass_Half (walkable)

Place a map text file with MAP_WIDTH x MAP_HEIGHT characters in assets and pass its path at initialization.

## Dependencies
- C++17 compatible compiler (g++ / MSVC)  
- SDL2  
- SDL2_image  
- SDL2_ttf

On Windows using MSYS2 (recommended):
- Install MSYS2 and then in MINGW64 shell:
  - pacman -Syu
  - pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf

## Build (MSYS2 / MinGW64)
Open MINGW64 shell in MSYS2 and run (from project root):
```bash
g++ -std=c++17 -I. game/*.cpp json/*.cpp -o isometric_game \
  $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf)
```
Adjust file list or use a build system (Makefile / CMake) for larger projects.

## Build (Visual Studio)
- Create a Visual Studio C++ project.  
- Add project files.  
- Add include directories for SDL2, SDL2_image, SDL2_ttf and link with SDL2.lib, SDL2main.lib, SDL2_image.lib, SDL2_ttf.lib.  
- Copy runtime DLLs (SDL2.dll, SDL2_image.dll, SDL2_ttf.dll) to the executable folder.

## Run
- Ensure `assets/` contains required atlases, textures, fonts and a map file.  
- Run the executable. The console/log will print debug messages (useful for missing textures / atlas slices).

## Controls
- Mouse to hover / click UI buttons.  
- Click on tiles to move/attack depending on selected action (Move/Attack).  
- UI shows current turn in top-right.

## Troubleshooting
- "Parameter 'texture' is invalid" — a texture was null/invalid. Check atlas loading and that texture assets are in correct path.  
- Multiple "Game cleaned up successfully" — ensure cleanup is called once at shutdown.

## Suggested next improvements
- Add health bars, sound, more AI behaviors, save/load, and a main menu.  
- Add CMake or a proper build system.

## Contributing
- Fork, add features/fixes, and submit PRs. Keep changes modular: assets, animations, AI, or UI.

## TODO
Cleanup coding structure on multiple files, get rid of debugging lines, expand upon combat system
