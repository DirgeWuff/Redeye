//
// Author: DirgeWuff
// Created on: 5/14/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class declaration and constructor for the primary GameLayer.
// This layer contains all the classes and logic that facilitate
// actual gameplay. This layer should be considered the core
// of the game itself.

#ifndef GAMELAYER_H
#define GAMELAYER_H

#define GLSL_VERSION 330

#include <memory>
#include <filesystem>
#include "../../Core/Camera/Camera.h"
#include "../../Core/Event/EventDispatcher.h"
#include "../../Core/Entity/Player.h"
#include "../../Core/Renderer/Tilemap.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/UI/UI.h"
#include "../../Core/Serialization/Save.h"
#include "../../Core/Event/EventBus.h"

// TODO: Go over init order, make sure it's safe and easily understandable

namespace fs = std::filesystem;

namespace RE::Application {
    class GameLayer final : public Core::Layer {
        Core::MapData m_map{};
        Core::AudioManager m_audioManager{};
        b2WorldDef m_worldDef{};
        Core::SceneCamera m_camera{};
        Core::EventBus m_eventBus;
        Core::saveData m_currentSave{};
        RenderTexture2D m_frameBuffer{};
        Shader m_fragShader{};
        std::shared_ptr<Core::Player> m_playerCharacter{};
        b2WorldId m_worldId{};
        int m_flashlightPosLoc{};
        int m_flashlightDirLoc{};
        int m_screenResLoc{};
        Vector2 m_beamPosition{};
        Vector2 m_beamAngle{};

        void setEventCallbacks();
    public:
        template<typename P>
        GameLayer(
            P&& playerSpritePath,
            const Core::saveData& save) :
                m_worldDef(b2DefaultWorldDef())
    {
        m_type = Core::layerType::PRIMARY_LAYER;
        m_isEnabled = true;
        m_worldDef.gravity = {0.0f, 50.0f};
        m_worldId = b2CreateWorld(&m_worldDef);
        m_map = Core::loadMap(save.currentMapPath.string(), m_worldId);
        m_currentSave.currentMapPath = fs::path(save.currentMapPath);
        m_currentSave.centerPosition = save.centerPosition;
        m_frameBuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        m_camera = Core::SceneCamera(m_map, 1.5f);
        m_fragShader = LoadShader(NULL, "../assets/Shaders/lighting.fsh"); // NOLINT

        if (!IsShaderValid(m_fragShader)) {
            Core::logFatal("Unable to load shader. GameLayer::GameLayer()");
            return;
        }

        m_flashlightPosLoc = GetShaderLocation(m_fragShader, "lightPos");
        m_flashlightDirLoc = GetShaderLocation(m_fragShader, "lightDirection");
        m_screenResLoc = GetShaderLocation(m_fragShader, "screenResolution");

        const Vector2 screenRes = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

        SetShaderValue(
            m_fragShader,
            m_screenResLoc,
            &screenRes,
            SHADER_UNIFORM_VEC2);

        try {
            // Has to be ptr. b2Body enters undefined state when moved.
            m_playerCharacter = std::make_shared<Core::Player>(
                Core::metersToPixels(m_currentSave.centerPosition.x),
                Core::metersToPixels(m_currentSave.centerPosition.y),
                m_worldId,
                std::forward<P>(playerSpritePath));

            m_audioManager.pushSound(
                Core::soundId::PLAYER_FOOTSTEP,
                std::make_unique<Core::SoundArray>("../assets/Player assets/Sounds/Walk"));

            m_audioManager.pushMusic(
                Core::musicId::BACKGROUND_NOISE,
                Core::Music("../assets/Map data/Sounds/Brown noise.wav"));

            m_audioManager.setMusicVolume(Core::musicId::BACKGROUND_NOISE, 0.30f);
            m_audioManager.startMusic(Core::musicId::BACKGROUND_NOISE);
        }
        catch (const std::exception& e) {
            Core::logFatal(std::string("GameLayer::GameLayer(Args...) failed: ") + std::string(e.what()));
            return;
        }
        catch (...) {
            Core::logFatal("GameLayer::GameLayer(Args...) failed: An unknown error has occurred.");
            return;
        }

        m_eventBus.addDispatcher(Core::EventDispatcher<Core::playerCollisionEvent>());
        this->setEventCallbacks();
        m_camera.setTarget(*m_playerCharacter);
    }
        ~GameLayer() override;

        GameLayer(const GameLayer&) = delete;
        GameLayer(GameLayer&&) = delete;
        GameLayer& operator=(const GameLayer&) noexcept = delete;
        GameLayer& operator=(GameLayer&&) noexcept = delete;

        void updateBeam();
        void processSensorEvents();
        void update() override;
        void draw() override;

        void destroy() override;
    };
}

#endif // GAMELAYER_H
