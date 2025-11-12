cmake_minimum_required(VERSION 3.28)
project(Redeye)

include(GNUInstallDirs)
include(ProcessorCount)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/external_libs)

# Detect CPU cores
# ======================================================================================================================
ProcessorCount(NCPU)
if(NOT NCPU OR NCPU EQUAL 0)
    set(NCPU 2)
endif()
math(EXPR LTO_JOBS "${NCPU} - 1")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs only" FORCE)

include(CMake/FetchDependencies.cmake)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type" FORCE)
endif()


# Compiler/config flags
# ======================================================================================================================
if (MSVC)
    message(STATUS "Setting MSVC flags...")
    target_compile_options(Redeye PRIVATE /W4 /permissive- /Zc:__cplusplus)

    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")

else()
    message(STATUS "Setting GCC/Clang flags...")

    add_compile_options(-Wall -Wextra -Wpedantic -fno-omit-frame-pointer -march=native)
    add_link_options(-static-libgcc -static-libstdc++)

    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -flto=${LTO_JOBS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto=${LTO_JOBS}")

    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fno-omit-frame-pointer")
endif()


# Target
# ======================================================================================================================
add_executable(Redeye
        Source/Core/Main/main.cpp
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
        Source/Core/Serialization/Save.cpp
        Source/Core/Serialization/Save.h
        Source/Core/Backend/LayerTypes.h
        Source/Core/Backend/LayerTypes.cpp
        Source/Core/Utility/Globals.h
        Source/Core/Renderer/Tilemap.cpp
)

# Compile definitions
# ======================================================================================================================
target_compile_definitions(Redeye PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

#
# External libs
# ======================================================================================================================
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
elseif(UNIX AND NOT APPLE)
    target_link_libraries(Redeye PRIVATE m pthread dl GL X11 Xi Xrandr Xinerama Xcursor)
endif()

find_library(tileson
        NAMES tson tileson
        HINTS ${PROJECT_SOURCE_DIR}/external_libs
        NO_DEFAULT_PATH)