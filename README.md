# Redeye

Redeye is a 2D Survival Horror game prototype I developed over the past few months in C++, with only a few dependencies (Raylib, Box2D and Tileson).
At the time of writing this (August 2025) the game is pre-alpha, and this repo is highly volatile and updated regularly to add new features, add gameplay, improve performance, and fix bugs. This is NOT a final product, and only includes basic functionality. 

The following code is not guaranteed to be performant, nor complete, and should not be taken as such. However, much effort and research has been put into it, and it's being published in the hopes it can help others who wish to create a similar project.

Expect to see regular updates to the repo.

![Gameplay screenshot](/Images/Game.png)

## BUILDING AND RUNNING

### Requirements

- CMake version 3.30 or greater 
- Ninja build system version 1.13 or greater
- C++ compiler and libraries capable of compiling C++ 20 or greater
- A working version of git

### Building

With these in place, run the following commands in your terminal of choice:

```shell
git clone github.com/DirgeWuff/Redeye.git
cd Redeye
mkdir build
cd build
```

From here, you can choose to build either the debug or release build. 
The debug build includes a set of debugging tools that can draw invisible geometry and
provide other additional functionality. The release build will generate optimized assembly 
for your platform, increasing performance and removing the debugging tools.

For the release build, run: `cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..`

Or for the debug build, run: `cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ..`

Then run `cmake --build .`

This will generate an executable for you to run.

> [!Note]
> This build system is new and still getting the kinks worked out of it, if you have any 
> issues compiling, please contact me and I'll assist you.

## ACKNOWLEDGEMENTS:

Huge thank you to the creators of Raylib, Box2D and Tileson, the dependencies that made this game possible.

- https://github.com/raysan5/raylib
- https://github.com/erincatto/box2d
- https://github.com/SSBMTonberry/tileson/blob/master/tileson.hpp

Also huge thank you to the creators of Tiled, the tiled map editor used to create the maps for this project.

- https://github.com/mapeditor/tiled
