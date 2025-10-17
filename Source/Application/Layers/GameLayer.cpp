//
// Created by DirgeWuff on 5/14/25.
//

#include <cstdint>
#include "raylib.h"
#include "box2d/box2d.h"
#include "GameLayer.h"
#include "../../Core/Utility/Error.h"
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Utility/Debug.h"
#include "../../Core/Renderer/TilemapRenderer.h"

constexpr float g_worldStep = 1.0f / 60.0f;
constexpr uint8_t g_subStep = 4;

GameLayer::~GameLayer() {
    destroy();
}

void GameLayer::pollEvents() {
    if (IsKeyDown(KEY_A)) {
        m_playerCharacter->moveLeft();
    }
    else if (IsKeyDown(KEY_D)) {
        m_playerCharacter->moveRight();
    }

    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
        m_playerCharacter->moveNowhere();
    }

    if (IsKeyDown(KEY_SPACE)) {
        m_playerCharacter->jump();
    }
}

void GameLayer::update() {
    pollEvents();

    if (!b2World_IsValid(m_worldId)) {
        logErr("m_worldId invalid: GameLayer::update()");
        return;
    }

    b2World_Step(m_worldId, g_worldStep, g_subStep);

    const b2SensorEvents sensorContactEvents = b2World_GetSensorEvents(m_worldId);

    // Process all b2SensorBeginTouch events for the step.
    for (std::size_t i = 0; i < sensorContactEvents.beginCount; i++) {
        const auto& [sensorShapeId, visitorShapeId] = sensorContactEvents.beginEvents[i];
        const auto* userData = static_cast<sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
        if (userData) {
            m_collisionEventDispatcher->dispatch(
                userData->typeId,
                playerContactEvent{true, sensorShapeId});
        }
        else {
            logErr("Unhandled b2BeginContactEvent. sensorShapeId.userData blank. GameLayer::update()");
            break;
        }
    }

    // Process all the b2SensorEndTouch events for the step.
    for (std::size_t i = 0; i < sensorContactEvents.endCount; i++) {
        const auto& [sensorShapeId, visitorShapeId] = sensorContactEvents.endEvents[i];
        const auto* userData = static_cast<sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
        if (userData) {
            m_collisionEventDispatcher->dispatch(
                userData->typeId,
                playerContactEvent{false, sensorShapeId});
        }
        else {
            logErr("Unhandled b2EndContactEvent. senorShapeId.userData blank. GameLayer::update()");
            break;
        }
    }

    m_playerCharacter->update();
    m_camera->update(m_playerCharacter);
    UpdateMusicStream(m_backgroundNoise);
}

void GameLayer::draw() {
    if (!m_playerCharacter->isDead()) {
        m_camera->cameraBegin();

        ClearBackground(BLACK);
        renderMap(m_camera, m_map, {0.0f, 0.0f}, WHITE);
        m_playerCharacter->draw();

        // TODO: Make a debug layer
#ifdef DEBUG
        drawDebugBodyShapes(m_playerCharacter);
        drawDebugCollisionShapes(m_map);
        drawDebugBodyCenter(m_playerCharacter);
        drawDebugCollisionVerts(m_map);
        drawDebugCameraCrosshair(m_camera);
        drawDebugCameraRect(m_camera);
        drawDebugEventColliders(m_map);
#endif

        m_camera->cameraEnd();

#ifdef DEBUG
        // Draw these outside of camera context!
        drawControlsWindow();
        drawDebugFootpawSensorStatus(m_playerCharacter);
        drawDebugPlayerPosition(m_playerCharacter);
#endif
    }
}

void GameLayer::destroy() {
    #ifdef DEBUG
        std::cout << "GameLayer object destroyed at address: " << this << std::endl;
    #endif

    UnloadMusicStream(m_backgroundNoise);
    unloadMap(m_map);
}
