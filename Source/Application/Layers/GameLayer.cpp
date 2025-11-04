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

static constexpr float g_worldStep = 1.0f / 60.0f;
static constexpr std::uint8_t g_subStep = 4;
static constexpr double g_pi = 3.14159265359;

GameLayer::~GameLayer() {
    GameLayer::destroy();
}

// TODO: Move all this shader logic to it's own class
void GameLayer::updateBeam() {
    // Someone smarter than me could probably do this in half the LoC...
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
    const bool playerFacingRight = m_playerCharacter->getPlayerDirection() == directions::RIGHT;
    const Vector2 beamOffset = playerFacingRight ? Vector2{110.0f, 132.5f} : Vector2{30.0f, 132.5f};
    m_beamPosition = Vector2Add(playerScreenPos, beamOffset);
    m_beamPosition.y = GetScreenHeight() - m_beamPosition.y;

    SetShaderValue(m_fragShader, m_flashlightPosLoc, &m_beamPosition, SHADER_UNIFORM_VEC2);

    // Flip normals 180 deg if player is facing left
    const Vector2 playerXNormals = playerFacingRight ? Vector2{1.0f, 0.0f} : Vector2{-1.0f, 0.0f};
    Vector2 beamDir = m_beamAngle;

    static bool lastFacingRight = playerFacingRight;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        // Calculate direction vector to mouse and light angle
        Vector2 vecToMouse = Vector2Subtract(GetMousePosition(), m_beamPosition);
        vecToMouse.y = -vecToMouse.y;
        float relativeAngle = std::atan2f(vecToMouse.y, vecToMouse.x) - std::atan2f(playerXNormals.y, playerXNormals.x);

        // Normalize for both negative pi and pi
        if (relativeAngle >  g_pi) relativeAngle -= 2.0f * g_pi;
        if (relativeAngle < -g_pi) relativeAngle += 2.0f * g_pi;

        // Clamp beam y angle
        constexpr float minAngle = -75.0f * g_pi / 180.0f;
        constexpr float maxAngle = 75.0f * g_pi / 180.0f;
        relativeAngle = std::clamp(relativeAngle, minAngle, maxAngle);

        // Build a normalized direction vector
        const float worldAngle = relativeAngle + std::atan2f(playerXNormals.y, playerXNormals.x);
        beamDir = Vector2Normalize({std::cos(worldAngle), std::sin(worldAngle)});

    } // Flip X normals 180 deg if player turns around
    else if (playerFacingRight != lastFacingRight) {
        beamDir.x *= -1.0f;
    }

    m_beamAngle = beamDir;
    SetShaderValue(m_fragShader, m_flashlightDirLoc, &m_beamAngle, SHADER_UNIFORM_VEC2);
    lastFacingRight = playerFacingRight;
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
    assert(m_playerCharacter);
    assert(b2World_IsValid(m_worldId));

    pollEvents();
    b2World_Step(m_worldId, g_worldStep, g_subStep);

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
            logErr("Unhandled b2BeginContactEvent. sensorShapeId.userData blank. GameLayer::update()");
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
            logErr("Unhandled b2EndContactEvent. senorShapeId.userData blank. GameLayer::update()");
            break;
        }
    }

    m_playerCharacter->update();
    m_camera.update(*m_playerCharacter);
    UpdateMusicStream(m_backgroundNoise);
    updateBeam();
}

void GameLayer::draw() {
    assert(m_playerCharacter);

    if (!m_playerCharacter->isDead()) {
        BeginTextureMode(m_frameBuffer);
            ClearBackground(BLACK);
            m_camera.cameraBegin();
                renderMap(m_camera, m_map, {0.0f, 0.0f}, WHITE);
            m_camera.cameraEnd();
        EndTextureMode();

        // Single render pass for the shader.
        BeginShaderMode(m_fragShader);
            DrawTextureRec(
                m_frameBuffer.texture,
                {
                    0.0f,
                    0.0f,
                    static_cast<float>(m_frameBuffer.texture.width),
                    static_cast<float>(-m_frameBuffer.texture.height)},
                {0.0f, 0.0f},
                WHITE);
        EndShaderMode();

        m_camera.cameraBegin();
            m_playerCharacter->draw();
        m_camera.cameraEnd();

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
#endif
    }
}

void GameLayer::destroy() {
    #ifdef DEBUG
        std::cout << "GameLayer object destroyed at address: " << this << std::endl;
    #endif

    UnloadMusicStream(m_backgroundNoise);
    UnloadShader(m_fragShader);
    unloadMap(m_map);
}
