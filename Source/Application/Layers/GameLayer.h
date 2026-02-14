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
#include "../../Core/Camera/Camera.h"
#include "../../Core/Entity/Player.h"
#include "../../Core/Renderer/Tilemap.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/Serialization/Save.h"
#include "../../Core/Event/EventBus.h"

namespace RE::Application {
    class GameLayer final : public Core::Layer {
        Core::MapData m_map{};
        b2WorldDef m_worldDef{};
        Core::SceneCamera m_camera{};
        Core::EventBus m_eventBus;
        Core::saveData m_currentSave{};
        RenderTexture2D m_frameBuffer{};
        Shader m_fragShader{};
        std::shared_ptr<Core::AudioManager> m_audioManager{};
        std::shared_ptr<Core::Player> m_playerCharacter{};
        b2WorldId m_worldId{};
        int m_flashlightPosLoc{};
        int m_flashlightDirLoc{};
        int m_screenResLoc{};
        Vector2 m_beamPosition{};
        Vector2 m_beamAngle{};

        void setEventCallbacks();
        void updateBeam();
        void processSensorEvents();
        void destroy() override;
    public:
        explicit GameLayer(const Core::saveData& save);
        ~GameLayer() override;

        GameLayer(const GameLayer&) = delete;
        GameLayer(GameLayer&&) = delete;
        GameLayer& operator=(const GameLayer&) noexcept = delete;
        GameLayer& operator=(GameLayer&&) noexcept = delete;

        void update() override;
        void draw() override;

    };
}

#endif // GAMELAYER_H
