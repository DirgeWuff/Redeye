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
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Event/EventDispatcher.h"
#include "../../Core/Entity/Player.h"
#include "../../Core/Renderer/Tilemap.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/Backend/LayerManager.h"
#include "../../Core/UI/UI.h"
#include "../../Core/Serialization/Save.h"
#include "../../Core/Event/EventBus.h"

// TODO: Go over init order, make sure it's safe and easily understandable

namespace fs = std::filesystem;

class GameLayer final : public Layer {
    MapData m_map{};
    b2WorldDef m_worldDef{};
    SceneCamera m_camera{};
    EventBus m_eventBus;
    Music m_backgroundNoise{};
    saveData m_currentSave{};
    RenderTexture2D m_frameBuffer{};
    Shader m_fragShader{};
    std::shared_ptr<Player> m_playerCharacter{};
    b2WorldId m_worldId{};
    int m_flashlightPosLoc{};
    int m_flashlightDirLoc{};
    int m_screenResLoc{};
    Vector2 m_beamPosition{};
    Vector2 m_beamAngle{};
public:
    template<typename P>
    GameLayer(
        P&& playerSpritePath,
        const saveData& save) :
            m_worldDef(b2DefaultWorldDef())
{
    m_type = layerType::PRIMARY_LAYER;
    m_isEnabled = true;
    m_worldDef.gravity = {0.0f, 50.0f};
    m_worldId = b2CreateWorld(&m_worldDef);
    m_map = loadMap(save.currentMapPath.string(), m_worldId);
    m_currentSave.currentMapPath = fs::path(save.currentMapPath);
    m_currentSave.centerPosition = save.centerPosition;
    m_frameBuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    m_camera = SceneCamera(m_map, 1.5f);
    m_fragShader = LoadShader(NULL, "../assets/Shaders/lighting.fsh");

    if (!IsShaderValid(m_fragShader)) {
        logFatal("Unable to load shader. GameLayer::GameLayer()");
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
        m_playerCharacter = std::make_shared<Player>(
            metersToPixels(m_currentSave.centerPosition.x),
            metersToPixels(m_currentSave.centerPosition.y),
            m_worldId,
            std::forward<P>(playerSpritePath));
    }
    catch (const std::exception& e) {
        logFatal(std::string("GameLayer::GameLayer(Args...) failed: ") + std::string(e.what()));
        return;
    }
    catch (...) {
        logFatal("GameLayer::GameLayer(Args...) failed: An unknown error has occurred.");
        return;
    }

    m_eventBus.addDispatcher(EventDispatcher<playerCollisionEvent>());

    // Subscribe footpaw sensor.
    m_eventBus.get<playerCollisionEvent>().subscribe(
        subId::PLAYER_FOOTPAW_GROUND_CONTACT,
        [this](const playerCollisionEvent& e) {
            const sensorInfo pawInfo = m_playerCharacter->getFootpawSensorInfo();

            return pawInfo.id == e.info.id && pawInfo.type == e.info.type;
        },
        [this](const playerCollisionEvent& e) {
            if (e.contactBegan) {
                m_playerCharacter->addContactEvent();
            }
            else {
                m_playerCharacter->removeContactEvent();
            }
        });

    // Subscribe death colliders.
    m_eventBus.get<playerCollisionEvent>().subscribe(
        subId::PLAYER_DEATH_COLLIDER_CONTACT,
        [this](const playerCollisionEvent& e) {
            return e.info.type == sensorType::MURDER_BOX;
        },
        [this](const playerCollisionEvent& e) {
        if (e.contactBegan) {
            m_playerCharacter->murder();
        }
    });

    // Subscribe checkpoint colliders.
    m_eventBus.get<playerCollisionEvent>().subscribe(
        subId::PLAYER_CHECKPOINT_COLLIDER_CONTACT,
        [](const playerCollisionEvent& e) {
            return e.info.type == sensorType::CHECKPOINT;
        },
        [this](const playerCollisionEvent& e) {
            if (e.contactBegan) {
                m_currentSave.centerPosition = m_playerCharacter->getPositionCenterMeters();
                m_currentSave.currentMapPath = m_map.fullMapPath;

                saveGame(m_currentSave);

                if (LayerManager::getInstance().stackContains(layerKey::CHECKPOINT_ALERT)) {
                    LayerManager::getInstance().resumeLayer(layerKey::CHECKPOINT_ALERT);
                }
                else {
                    try {
                        LayerManager::getInstance().pushLayer(
                        layerKey::CHECKPOINT_ALERT,
                        std::make_unique<TextAlert>("checkpoint reached", 5.0f));
                    }
                    catch (const std::exception& err) {
                        logFatal(std::string("Checkpoint collider failed: ") + std::string(err.what()));
                        return;
                    }
                    catch (...) {
                        logFatal("Checkpoint collider failed due to an unknown error.");
                        return;
                    }
                }

                const auto* info = static_cast<sensorInfo*>(b2Shape_GetUserData(e.visitorShape));
                disableEventCollider(m_map, info->id);
            }
        });

    m_camera.setTarget(*m_playerCharacter);
    m_backgroundNoise = LoadMusicStream(m_map.bgNoisePath.string().c_str());
    if (!IsMusicValid(m_backgroundNoise)) {
        logFatal(std::string("Cannot load background noise: " + m_map.bgNoisePath.string()));
        return;
    }

    SetMusicVolume(m_backgroundNoise, 0.30f);
    PlayMusicStream(m_backgroundNoise);
}
    ~GameLayer() override;

    void updateBeam();
    void processSensorEvents();
    void update() override;
    void draw() override;

    void destroy() override;
};

#endif // GAMELAYER_H
