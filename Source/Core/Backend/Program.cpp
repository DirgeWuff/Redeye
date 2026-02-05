//
// Author: DirgeWuff
// Created on: 10/1/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class definition for Program.h, includes main game loop.

#include <memory>
#include <filesystem>
#include "raylib.h"
#include "Program.h"
#include "LayerManager.h"
#include "../Utility/Enum.h"
#include "../../Application/Layers/GameLayer.h"
#include "../../Application/Layers/StartMenuLayer.h"
#include "../Utility/Globals.h"
#include "../Utility/Logging.h"
#include "../Serialization/Save.h"

namespace fs = std::filesystem;

namespace RE::Core {
    Program Program::m_appInstance = Program();

    Program::Program() :
        m_gameRunning(true)
        {
            #ifdef DEBUG
                logDbg("Program created at address: ", this);
            #endif
        }

    void Program::init() {
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
                std::make_unique<RE::Application::StartMenuLayer>());

            LayerManager::getInstance().pushLayer(
                 layerKey::GAME_LAYER,
                 std::make_unique<RE::Application::GameLayer>(g_playerSpritePath, initSave));
        }
        catch (const std::exception& e) {
            logFatal(std::string("Program::init() failed: " + std::string(e.what())));
            m_gameRunning = false;
            return;
        }
        catch (...) {
            logFatal("Program::init() failed: An unknown error has occurred.");
            m_gameRunning = false;
            return;
        }

        LayerManager::getInstance().suspendLayer(layerKey::GAME_LAYER);

        logDbg("Program successfully initialized.");
    }

    // Main game loop
    void Program::run() const {
        logDbg("Starting main loop...");

        while (m_gameRunning && !WindowShouldClose()) {
            LayerManager::getInstance().pollEvents();

            LayerManager::getInstance().update();


            BeginDrawing();
            LayerManager::getInstance().draw();
            EndDrawing();

            LayerManager::getInstance().updateLayerStack();
        }

        CloseWindow();

        logDbg("Main loop ended.");
    }

    [[nodiscard]] Program& Program::getInstance() noexcept {
        return m_appInstance;
    }

    void Program::shutdown() noexcept {
        m_gameRunning = false;

        logDbg("Game shutdown requested.");
    }
}