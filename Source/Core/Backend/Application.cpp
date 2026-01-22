//
// Created by DirgeWuff on 10/1/25.
//

#include <memory>
#include <filesystem>
#include "raylib.h"
#include "Application.h"
#include "LayerManager.h"
#include "../Utility/Enum.h"
#include "../../Application/Layers/GameLayer.h"
#include "../../Application/Layers/StartMenuLayer.h"
#include "../Utility/Globals.h"
#include "../Utility/Logging.h"
#include "../Serialization/Save.h"

namespace fs = std::filesystem;

Application Application::m_appInstance = Application();

Application::Application() :
m_gameRunning(true)
{
    #ifdef DEBUG
        logDbg("Application created at address: ", this);
    #endif
}

void Application::init() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(g_windowWidth, g_windowHeight, "Redeye");
    InitAudioDevice();
    SetTargetFPS(60);

    logDbg("Render API successfully initialized.");

    createNewSessionLog();
    saveData initSave{};

    if (fs::exists(g_saveFilePath)) {
        initSave = loadGame();
    }
    else {
        createDefaultSave();
        initSave = loadGame();
    }

    try {
        LayerManager::getInstance().pushLayer(
            layerKey::START_MENU,
            std::make_unique<StartMenuLayer>());

        LayerManager::getInstance().pushLayer(
             layerKey::GAME_LAYER,
             std::make_unique<GameLayer>(g_playerSpritePath, initSave));
    }
    catch (const std::exception& e) {
        logFatal(std::string("Application::init() failed: " + std::string(e.what())));
        m_gameRunning = false;
        return;
    }
    catch (...) {
        logFatal("Application::init() failed: An unknown error has occurred.");
        m_gameRunning = false;
        return;
    }

    LayerManager::getInstance().suspendLayer(layerKey::GAME_LAYER);

    logDbg("Application successfully initialized.");
}

// Main game loop
void Application::run() const {
    logDbg("Starting main loop...");

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

    logDbg("Main loop ended.");
}

[[nodiscard]] Application& Application::getInstance() noexcept {
    return m_appInstance;
}

void Application::shutdown() noexcept {
    m_gameRunning = false;

    logDbg("Game shutdown requested.");
}
