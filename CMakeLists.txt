cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(Redeye)

set(CMAKE_CXX_STANDARD 20)

# Most of the following ripped from the Raylib default CMakeLists

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
find_package(box2d CONFIG REQUIRED)
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    endif()
endif()

# Our Project

include_directories(${PROJECT_SOURCE_DIR}/external_libs)

add_executable(${PROJECT_NAME} main.cpp
        Player.cpp
        Player.h
        CommandListener.h
        InputHandler.cpp
        InputHandler.h
        Scene.cpp
        Scene.h
        Tilemap.cpp
        Tilemap.h
        Camera.cpp
        Camera.h
        Error.cpp
        Error.h
        Utils.cpp
        Utils.h
        Debug.cpp
        Debug.h
        Entity.cpp
        Entity.h
        BoxBody.cpp
        BoxBody.h
)

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} box2d::box2d)
find_library(tileson
NAMES tson tileson
HINTS ${PROJECT_SOURCE_DIR}/external_libs
NO_DEFAULT_PATH)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
