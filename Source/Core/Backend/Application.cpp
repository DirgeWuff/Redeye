//
// Created by DirgeWuff on 10/1/25.
//

#include <iostream>
#include <memory>
#include <filesystem>
#include "raylib.h"
#include "Application.h"
#include "LayerManager.h"
#include "LayerTypes.h"
#include "../../Application/Layers/GameLayer.h"
#include "../../Application/Layers/StartMenuLayer.h"
#include "../Utility/Globals.h"
#include "../Serialization/Save.h"

namespace fs = std::filesystem;

Application Application::m_appInstance = Application();

Application::Application() :
m_gameRunning(true)
{
    #ifdef DEBUG
        std::cout << "Application object created at address: " << this << std::endl;
    #endif
}

void Application::init() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(g_windowWidth, g_windowHeight, "Redeye");
    InitAudioDevice();
    SetTargetFPS(60);

    saveData initSave{};

    if (fs::exists("../Savegame/save.toml")) {
        initSave = loadGame();
    }
    else {
        createDefaultSave();
        initSave = loadGame();
    }

    LayerManager::getInstance().pushLayer(
        layerKey::START_MENU,
        std::make_unique<StartMenuLayer>());

    LayerManager::getInstance().pushLayer(
         layerKey::GAME_LAYER,
         std::make_unique<GameLayer>(g_playerSpritePath, initSave));

    LayerManager::getInstance().suspendLayer(layerKey::GAME_LAYER);
}

// Main loop
void Application::run() const {
    while (m_gameRunning && !WindowShouldClose()) {
        // Poll input
        LayerManager::getInstance().pollEvents();

        // Update
        LayerManager::getInstance().update();

        // Draw
        BeginDrawing();
        LayerManager::getInstance().draw();
        EndDrawing();

        // Update layer stack
        LayerManager::getInstance().updateLayerStack();
    }

    CloseWindow();
}

[[nodiscard]] Application& Application::getInstance() noexcept {
    return m_appInstance;
}

void Application::shutdown() noexcept {
    m_gameRunning = false;
}
