include(FetchContent)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
)

set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Don't build games" FORCE)

# Supress warnings related to stb_image
set(CMAKE_C_FLAGS "-w ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-w ${CMAKE_CXX_FLAGS}")

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v3.1.0
)

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Don't build unit tests" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Don't build testbed" FORCE)
set(BOX2D_BUILD_SHARED OFF CACHE BOOL "Build Box2D static libs only" FORCE)

FetchContent_MakeAvailable(box2d)