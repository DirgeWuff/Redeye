#include "raylib.h"
#include "Scene.h"

constexpr int WINDOW_WIDTH = 1500;
constexpr int WINDOW_HEIGHT = 800;

int main() {

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Redeye");
    SetTargetFPS(60);

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