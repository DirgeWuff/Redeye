cmake_minimum_required(VERSION 3.28.3)
project(Redeye)

include_directories(${PROJECT_SOURCE_DIR}/external_libs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically detect number of cores on the machine
include(ProcessorCount)
ProcessorCount(NCPU)

If(NCPU EQUAL 0)
    set(NCPU 2)
endif ()

# Set number of jobs to cores -1
math(EXPR LTO_JOBS "${NCPU} - 1")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs only" FORCE)

include(CMake/FetchDependencies.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Setting release flags...")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_link_options(-fsanitize=address)
        add_link_options(-flto=${LTO_JOBS})
    elseif (MSVC)
        add_link_options(/O2 /DNDEBUG)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Setting debug flags...")
    add_compile_options(-O3 -march=native -flto=${LTO_JOBS} -DNDEBUG)
    add_compile_options(-fsanitize=address -g -O1 -fno-omit-frame-pointer)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
    add_link_options(-static-libgcc -static-libstdc++)
    add_link_options(-fsanitize=address)
endif()

add_executable(Redeye Source/Core/Main/main.cpp
        Source/Core/Entity/Player.cpp
        Source/Core/Entity/Player.h
        Source/Application/Layers/GameLayer.cpp
        Source/Application/Layers/GameLayer.h
        Source/Core/Renderer/Tilemap.h
        Source/Core/Camera/Camera.cpp
        Source/Core/Camera/Camera.h
        Source/Core/Utility/Error.h
        Source/Core/Utility/Utils.cpp
        Source/Core/Utility/Utils.h
        Source/Core/Utility/Debug.cpp
        Source/Core/Utility/Debug.h
        Source/Core/Phys/BoxBody.cpp
        Source/Core/Phys/BoxBody.h
        Source/Core/Phys/CollisionObject.cpp
        Source/Core/Phys/CollisionObject.h
        Source/Core/Event/EventDispatcher.h
        Source/Core/Event/EventCollider.cpp
        Source/Core/Event/EventCollider.h
        Source/Core/UI/UI.cpp
        Source/Core/UI/UI.h
        Source/Core/Audio/Audio.cpp
        Source/Core/Audio/Audio.h
        Source/Core/Renderer/TilemapRenderer.cpp
        Source/Core/Renderer/TilemapRenderer.h
        Source/Core/Backend/Layer.h
        Source/Core/Backend/LayerManager.cpp
        Source/Core/Backend/LayerManager.h
        Source/Core/Backend/Application.cpp
        Source/Core/Backend/Application.h
        Source/Application/Layers/StartMenuLayer.cpp
        Source/Application/Layers/StartMenuLayer.h
        Source/Application/Layers/DeathMenuLayer.cpp
        Source/Application/Layers/DeathMenuLayer.h
)

target_compile_definitions(Redeye PRIVATE DEBUG)

target_link_libraries(Redeye PRIVATE raylib box2d)
if(WIN32)
    target_link_libraries(Redeye PRIVATE opengl32 gdi32 winmm)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CoreVideo_LIBRARY CoreVideo)
    target_link_libraries(Redeye PRIVATE
            ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${CoreVideo_LIBRARY})
elseif (UNIX AND NOT APPLE)
    target_link_libraries(Redeye PRIVATE m pthread dl GL X11 Xi Xrandr Xinerama Xcursor)
endif()

find_library(tileson
        NAMES tson tileson
        HINTS ${PROJECT_SOURCE_DIR}/external_libs
        NO_DEFAULT_PATH)
