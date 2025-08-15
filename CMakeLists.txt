cmake_minimum_required(VERSION 3.30)
project(Redeye)

include_directories(external_libs/Box2D/include)
include_directories(external_libs/Raylib/include)

set(CMAKE_CXX_STANDARD 23)

add_executable(Redeye main.cpp
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
        CollisionObject.cpp
        CollisionObject.h
        EventDispatcher.h
        EventCollider.cpp
        EventCollider.h
        UI.cpp
        UI.h
)

target_link_libraries(Redeye PRIVATE ${CMAKE_SOURCE_DIR}/external_libs/Box2D/lib/libbox2d.a)
target_link_libraries(Redeye PRIVATE
        ${CMAKE_SOURCE_DIR}/external_libs/Raylib/lib/libraylib.a
        -lopengl32
        -lgdi32
        -lwinmm
)

find_library(tileson
        NAMES tson tileson
        HINTS ${PROJECT_SOURCE_DIR}/external_libs
        NO_DEFAULT_PATH)