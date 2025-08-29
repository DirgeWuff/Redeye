cmake_minimum_required(VERSION 3.30)
project(Redeye)

include_directories(external_libs/Box2D/include)
include_directories(external_libs/Raylib/include)

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

add_executable(Redeye src/main.cpp
        src/Player.cpp
        src/Player.h
        src/CommandListener.h
        src/InputHandler.cpp
        src/InputHandler.h
        src/Scene.cpp
        src/Scene.h
        src/Tilemap.cpp
        src/Tilemap.h
        src/Camera.cpp
        src/Camera.h
        src/Error.h
        src/Utils.cpp
        src/Utils.h
        src/Debug.cpp
        src/Debug.h
        src/Entity.cpp
        src/Entity.h
        src/BoxBody.cpp
        src/BoxBody.h
        src/CollisionObject.cpp
        src/CollisionObject.h
        src/EventDispatcher.h
        src/EventCollider.cpp
        src/EventCollider.h
        src/UI.cpp
        src/UI.h
        src/Audio.cpp
        src/Audio.h
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Setting release flags...")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O3 -march=native -flto=${LTO_JOBS} -DNDEBUG)
        add_link_options(-flto=${LTO_JOBS})
    elseif (MSVC)
        add_link_options(/02 /DNDEBUG)
    endif ()
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Setting debug definitions...")
    target_compile_definitions(Redeye PRIVATE DEBUG)
elseif(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif ()


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
    target_link_libraries(Redeye PRIVATE m pthread dl GL X11 Xi Xrandr Xxf86vm Xinerama Xcursor)
endif()

find_library(tileson
        NAMES tson tileson
        HINTS ${PROJECT_SOURCE_DIR}/external_libs
        NO_DEFAULT_PATH)