#include "raylib.h"
#include "Scene.h"

constexpr int windowWidth = 1500;
constexpr int windowHeight = 800;

int main() {

    InitWindow(windowWidth, windowHeight, "Redeye");
    SetTargetFPS(30);

    Scene testScene = Scene(
        "../assets/PlayerWalk.png",
        "../assets/Map data/Redeye Test Lvl.tmj");

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