#include "../external_libs/Raylib/include/raylib.h"
#include "Scene.h"

constexpr uint16_t g_windowWidth = 1500;
constexpr uint16_t g_windowHeight = 800;

int main() {

    InitWindow(g_windowWidth, g_windowHeight, "Redeye");
    SetTargetFPS(60);

    Scene testScene = Scene(
        "../assets/PlayerWalk.png",
        "../assets/Map data/Version 2/TestLvl1.tmj");

    while (!WindowShouldClose()) {
        // Update
        testScene.updateScene();
        // Draw
        BeginDrawing();

        ClearBackground(BLACK);
        testScene.drawScene();

        EndDrawing();
    }

    return 0;
}