//
// Created by DirgeWuff on 5/14/25.
//

#include <cstdint>
#include "raylib.h"
#include "box2d/box2d.h"
#include "GameLayer.h"
#include "../../Core/Utility/Logging.h"
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Utility/Debug.h"
#include "../../Core/Renderer/TilemapRenderer.h"
#include "../../Core/Utility/Globals.h"

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
    const bool playerFacingRight = m_playerCharacter->getPlayerDirection() == direction::RIGHT;
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

void GameLayer::processSensorEvents() const {
    const b2SensorEvents sensorContactEvents = b2World_GetSensorEvents(m_worldId);

    // Process all b2SensorBeginTouch events for the current step.
    for (std::size_t i = 0; i < sensorContactEvents.beginCount; i++) {
        const auto& [sensorShapeId, visitorShapeId] = sensorContactEvents.beginEvents[i];
        const auto* userData = static_cast<sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
        if (userData) {
            m_collisionEventDispatcher.dispatch(
                userData->typeId,
                playerContactEvent{true, sensorShapeId});
        }
        else {
            logFatal("Unhandled b2BeginContactEvent. sensorShapeId.userData blank. GameLayer::update()");
            break;
        }
    }

    // Process all the b2SensorEndTouch events for the step.
    for (std::size_t i = 0; i < sensorContactEvents.endCount; i++) {
        const auto& [sensorShapeId, visitorShapeId] = sensorContactEvents.endEvents[i];
        const auto* userData = static_cast<sensorInfo*>(b2Shape_GetUserData(sensorShapeId));
        if (userData) {
            m_collisionEventDispatcher.dispatch(
                userData->typeId,
                playerContactEvent{false, sensorShapeId});
        }
        else {
            logFatal("Unhandled b2EndContactEvent. senorShapeId.userData blank. GameLayer::update()");
            break;
        }
    }
}

void GameLayer::update() {
    assert(m_playerCharacter);
    assert(b2World_IsValid(m_worldId));

    m_playerCharacter->pollEvents();
    b2World_Step(m_worldId, g_worldStep, g_subStep);
    processSensorEvents();
    m_playerCharacter->update(m_worldId);
    m_camera.update(*m_playerCharacter);
    UpdateMusicStream(m_backgroundNoise);

    if (g_drawShaderEffects)
        updateBeam();
}

void GameLayer::draw() {
    assert(m_playerCharacter);

    if (!m_playerCharacter->isDead()) {
        BeginTextureMode(m_frameBuffer);
            ClearBackground(BLACK);
            m_camera.cameraBegin();
                renderBackgroundLayers(m_camera, m_map, {0.0f, 0.0f}, WHITE);
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
            renderForegroundLayers(m_camera, m_map, {0.0f, 0.0f}, WHITE);

            // TODO: Make a debug layer
            #ifdef DEBUG
                drawDebugBodyShapes(*m_playerCharacter);
                drawDebugCollisionShapes(m_map);
                drawDebugBodyCenter(*m_playerCharacter);
                drawDebugCollisionVerts(m_map);
                drawDebugCameraCrosshair(m_camera);
                drawDebugCameraRect(m_camera);
                drawDebugEventColliders(m_map);
            #endif
        m_camera.cameraEnd();

        #ifdef DEBUG
            // Draw these outside of camera context!
            drawControlsWindow();
            drawDebugFootpawSensorStatus(*m_playerCharacter);
            drawDebugPlayerPosition(*m_playerCharacter);
            drawDebugPlayerAnimId(m_playerCharacter->getCurrentAnimId());
            drawDebugPlayerAnimState(m_playerCharacter->getCurrentActionState());
        #endif
    }
}

void GameLayer::destroy() {
    #ifdef DEBUG
        logDbg("GameLayer destroyed at address: ", this);
    #endif

    UnloadMusicStream(m_backgroundNoise);
    UnloadShader(m_fragShader);
    unloadMap(m_map);
}

