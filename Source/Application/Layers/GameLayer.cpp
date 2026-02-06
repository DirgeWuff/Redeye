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
// Class definition for GameLayer.h and its member functions. NOTE:
// An attempt was made to move all the functionality of updateBeam()
// into it's own class, and to move ownership of the shader to that
// class, but this caused a bizarre OpenGL bug where the vertex shader
// to become unbound during at the draw call. I spent 3 days trying to
// debug the issue, but to no avail. So for now, it stays in here.

#include <cstdint>
#include "raylib.h"
#include "box2d/box2d.h"
#include "GameLayer.h"
#include "../../Core/Utility/Logging.h"
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Utility/Debug.h"
#include "../../Core/Renderer/TilemapRenderer.h"
#include "../../Core/Utility/Globals.h"
#include "../../Core/Backend/LayerManager.h"
#include "TextAlertLayer.h"

namespace RE::Application {
    void GameLayer::setEventCallbacks() {
        try {
           // Subscribe footpaw sensor.
        m_eventBus.get<Core::playerCollisionEvent>().subscribe(
            Core::subId::PLAYER_FOOTPAW_GROUND_CONTACT,
            [this](const Core::playerCollisionEvent& e) {
                const Core::sensorInfo pawInfo = m_playerCharacter->getFootpawSensorInfo();

                return pawInfo.id == e.info.id && pawInfo.type == e.info.type;
            },
            [this](const Core::playerCollisionEvent& e) {
                if (e.contactBegan) {
                    m_playerCharacter->addContactEvent();
                }
                else {
                    m_playerCharacter->removeContactEvent();
                }
            });

        // Subscribe death colliders.
        m_eventBus.get<Core::playerCollisionEvent>().subscribe(
            Core::subId::PLAYER_DEATH_COLLIDER_CONTACT,
            [this](const Core::playerCollisionEvent& e) {
                return e.info.type == Core::sensorType::MURDER_BOX;
            },
            [this](const Core::playerCollisionEvent& e) {
            if (e.contactBegan) {
                m_playerCharacter->murder();
            }
        });

        // Subscribe checkpoint colliders.
        m_eventBus.get<Core::playerCollisionEvent>().subscribe(
            Core::subId::PLAYER_CHECKPOINT_COLLIDER_CONTACT,
            [](const Core::playerCollisionEvent& e) {
                return e.info.type == Core::sensorType::CHECKPOINT;
            },
            [this](const Core::playerCollisionEvent& e) {
                if (e.contactBegan) {
                    m_currentSave.centerPosition = m_playerCharacter->getPositionCenterMeters();
                    m_currentSave.currentMapPath = m_map.fullMapPath;

                    saveGame(m_currentSave);

                    if (Core::LayerManager::getInstance().stackContains(Core::layerKey::CHECKPOINT_ALERT)) {
                        Core::LayerManager::getInstance().resumeLayer(Core::layerKey::CHECKPOINT_ALERT);
                    }
                    else {
                        try {
                            Core::LayerManager::getInstance().pushLayer(
                            Core::layerKey::CHECKPOINT_ALERT,
                            std::make_unique<Application::TextAlertLayer>("checkpoint reached", 5.0f));
                        }
                        catch (const std::exception& err) {
                            Core::logFatal(std::string("Checkpoint collider failed: ") + std::string(err.what()));
                            return;
                        }
                        catch (...) {
                            Core::logFatal("Checkpoint collider failed due to an unknown error.");
                            return;
                        }
                    }

                    const auto* info = static_cast<Core::sensorInfo*>(b2Shape_GetUserData(e.visitorShape));
                    disableEventCollider(m_map, info->id);
                }
            });
        }
        catch (std::exception& e) {
            Core::logFatal(std::string("Error subscribing event callbacks: ") + std::string(e.what()));
            return;
        }
        catch (...) {
            Core::logFatal("An unknown error occurred");
            return;
        }

        #ifdef DEBUG
            Core::logDbg("Event callbacks set.");
        #endif
    }

    GameLayer::~GameLayer() {
        GameLayer::destroy();
    }

    void GameLayer::updateBeam() {
        // Someone smarter than me could probably do this in half the LoC...
        // Extra comments here because math is fucking hard
        const Vector2 playerWorldPos = m_playerCharacter->getPositionCornerPx();
        const Vector2 camTarget = m_camera.getCameraTarget();
        const Vector2 camOffset = m_camera.getCameraOffset();
        const float camZoom = m_camera.getCameraZoom();

        // Player pos in screen space
        const Vector2 playerScreenPos = {
            (playerWorldPos.x - camTarget.x) * camZoom + camOffset.x,
            (playerWorldPos.y - camTarget.y) * camZoom + camOffset.y
        };

        // Adjust beam offset depending on which direction player is facing, invert beam Y movement
        const bool playerFacingRight = m_playerCharacter->getPlayerDirection() == Core::direction::RIGHT;
        const Vector2 beamOffset = playerFacingRight ? Vector2{g_beamOffsetXRight, g_beamOffsetY} : Vector2{g_beamOffsetXLeft, g_beamOffsetY};
        m_beamPosition = Vector2Add(playerScreenPos, beamOffset);
        m_beamPosition.y = static_cast<float>(GetScreenHeight()) - m_beamPosition.y;

        SetShaderValue(m_fragShader, m_flashlightPosLoc, &m_beamPosition, SHADER_UNIFORM_VEC2);

        // Flip normals 180 deg if player is facing left
        const Vector2 playerXNormals = playerFacingRight ? Vector2{1.0f, 0.0f} : Vector2{-1.0f, 0.0f};
        Vector2 beamDir = m_beamAngle;

        static bool lastFacingRight = playerFacingRight;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            const Vector2 mousePos = GetMousePosition();
            const Vector2 shaderMousePos = { mousePos.x, GetScreenHeight() - mousePos.y };

            // Calculate direction vector from beam to mouse and relative angle
            const Vector2 vecToMouse = Vector2Normalize(Vector2Subtract(shaderMousePos, m_beamPosition));
            float angleToMouse = atan2f(vecToMouse.y, vecToMouse.x) - atan2f(playerXNormals.y, playerXNormals.x);

            // Normalize for both negative pi and pi
            if (angleToMouse > g_pi) angleToMouse -= 2.0f * g_pi;
            if (angleToMouse < -g_pi) angleToMouse += 2.0f * g_pi;

            // Clamp beam y angle
            constexpr float maxAngle = 75.0f * g_pi / 180.0f;
            constexpr float minAngle = -75.0f * g_pi / 180.0f;
            angleToMouse = std::clamp(angleToMouse, minAngle, maxAngle);

            // Calculate final beam direction
            const float finalAngle = atan2f(playerXNormals.y, playerXNormals.x) + angleToMouse;
            beamDir = { std::cos(finalAngle), std::sin(finalAngle) };
        }
        // Flip X normals 180 deg if player turns around
        else if (playerFacingRight != lastFacingRight) {
            beamDir.x *= -1.0f;
        }

        m_beamAngle = beamDir;
        lastFacingRight = playerFacingRight;
        SetShaderValue(m_fragShader, m_flashlightDirLoc, &m_beamAngle, SHADER_UNIFORM_VEC2);
    }

    void GameLayer::processSensorEvents() {
        const b2SensorEvents sensorContactEvents = b2World_GetSensorEvents(m_worldId);

        // Process all b2SensorBeginTouch events for the current step.
        for (std::size_t i = 0; i < sensorContactEvents.beginCount; i++) {
            const auto& [sensorShapeId, visitorShapeId] = sensorContactEvents.beginEvents[i];
            const auto* userData = static_cast<Core::sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
            if (userData) {
                m_eventBus.get<Core::playerCollisionEvent>().dispatch(
                    Core::playerCollisionEvent{
                        true,
                        visitorShapeId,
                        *userData});
            }
            else {
                Core::logFatal("Unhandled b2BeginContactEvent. sensorShapeId.userData blank. GameLayer::update()");
                break;
            }
        }

        // Process all the b2SensorEndTouch events for the step.
        for (std::size_t i = 0; i < sensorContactEvents.endCount; i++) {
            const auto& [sensorShapeId, visitorShapeId] = sensorContactEvents.endEvents[i];
            const auto* userData = static_cast<Core::sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
            if (userData) {
                m_eventBus.get<Core::playerCollisionEvent>().dispatch(
                    Core::playerCollisionEvent{
                        false,
                        visitorShapeId,
                        *userData});
            }
            else {
                Core::logFatal("Unhandled b2EndContactEvent. senorShapeId.userData blank. GameLayer::update()");
                break;
            }
        }
    }

    void GameLayer::update() {
        assert(m_playerCharacter);
        assert(b2World_IsValid(m_worldId));

        m_playerCharacter->pollEvents();
        b2World_Step(m_worldId, g_worldStep, g_subStep);
        this->processSensorEvents();
        m_playerCharacter->update(m_worldId, m_audioManager);
        m_camera.update(*m_playerCharacter);
        m_audioManager.updateMusic();

        if (g_drawShaderEffects)
            updateBeam();
    }

    void GameLayer::draw() {
        assert(m_playerCharacter);

        if (!m_playerCharacter->isDead()) {
            BeginTextureMode(m_frameBuffer);
                ClearBackground(BLACK);
                m_camera.cameraBegin();
                    Core::renderBackgroundLayers(m_camera, m_map, {0.0f, 0.0f}, WHITE);
                m_camera.cameraEnd();
            EndTextureMode();

            // Single render pass for the shader.
            if (g_drawShaderEffects) BeginShaderMode(m_fragShader);
            DrawTextureRec(
                m_frameBuffer.texture,
                {
                    0.0f,
                    0.0f,
                    static_cast<float>(m_frameBuffer.texture.width),
                    static_cast<float>(-m_frameBuffer.texture.height)},
                {0.0f, 0.0f},
                WHITE);
            if (g_drawShaderEffects) EndShaderMode();

            m_camera.cameraBegin();
                m_playerCharacter->draw();
                Core::renderForegroundLayers(m_camera, m_map, {0.0f, 0.0f}, WHITE);

                // TODO: Make a debug layer
                #ifdef DEBUG
                    Core::drawDebugBodyShapes(*m_playerCharacter);
                    Core::drawDebugCollisionShapes(m_map);
                    Core::drawDebugBodyCenter(*m_playerCharacter);
                    Core::drawDebugCollisionVerts(m_map);
                    Core::drawDebugCameraCrosshair(m_camera);
                    Core::drawDebugCameraRect(m_camera);
                    Core::drawDebugEventColliders(m_map);
                #endif
            m_camera.cameraEnd();

            #ifdef DEBUG
                // Draw these outside of camera context!
                Core::drawControlsWindow();
                Core::drawDebugFootpawSensorStatus(*m_playerCharacter);
                Core::drawDebugPlayerPosition(*m_playerCharacter);
                Core::drawDebugPlayerAnimId(m_playerCharacter->getCurrentAnimId());
                Core::drawDebugPlayerAnimState(m_playerCharacter->getCurrentActionState());
            #endif
        }
    }

    void GameLayer::destroy() {
        #ifdef DEBUG
            logDbg("GameLayer destroyed at address: ", this);
        #endif

        UnloadShader(m_fragShader);
        Core::unloadMap(m_map);
    }
}

