Redeye is a 2D Survival Horror game prototyle I developed over the past few months in C++, with only a few dependencies (Raylib, Box2D and Tileson).
At the time of writing this (August 2025) the game is pre-alpha, and this repo is highly volatile and updated regularly to add new features, add gameplay, improve performance, and fix bugs. This is NOT a final product, and only includes basic functionality. 


The following code is not gaurunteed to be performant, nor complete, and should not be taken as such. However, much effort and research has been put into it, and it's being published in the hopes it can help others who wish to create a similar project.

Expect to see regular updates to the repo.

BUILDING AND RUNNING:
This code is intended to be compiled on Windows with g++, and requires CMake. It includes statically compiled libraries that are platform specific to Windows, and it will not compile on Linux or Mac without refactoring the build toolchian. 
A more rubust and platform agnostic build system will be added later and will make building much easier for uses with other compilers and operating systems. 

For now, the easiest way to experience the prototype is to run the precompiled binary included with the repo. This binary can be found in the cmake-build-debug directory. There is also a precompiled Binary for Apple Silicon as well. 

If you would like to compile on your own with MSYS2 using Cmake and gcc/g++, you can do the following :

mkdir build && cd build
cmake ..

Stay tuned for future updates!
