#include "raylib.h"
#include "Scene.h"

constexpr uint16_t g_windowWidth = 1500;
constexpr uint16_t g_windowHeight = 800;

// TODO: Create audio handler
// TODO: Write proper copy/move/etc. constructors for custom classes  where needed

int main() {

    InitWindow(g_windowWidth, g_windowHeight, "Redeye");
    InitAudioDevice();
    SetTargetFPS(60);

    Scene testScene = Scene(
        "../assets/Player assets/PlayerWalk.png",
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