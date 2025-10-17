//
// Created by DirgeWuff on 5/26/25.
//

#include "raylib.h"
#include "raymath.h"
#include "Camera.h"

SceneCamera::SceneCamera(const MapData& map, const float zoomLevel) {
    m_camera = Camera2D();
    m_camera.offset = {1500.0f / 2.0f, 800.0f / 2.0f}; // Hard coded value, no bueno, fix later
    m_camera.zoom = zoomLevel;
    m_camera.rotation = 0.0f;

    m_cameraRect = {
        (GetScreenWidth() - static_cast<float>(GetScreenWidth()) / m_camera.zoom) / 2.0f,
        (GetScreenHeight() - static_cast<float>(GetScreenHeight()) / m_camera.zoom) / 2.0f,
        GetScreenWidth() / m_camera.zoom,
        GetScreenHeight() / m_camera.zoom
    };

    m_cameraCenter = {
        m_cameraRect.x + m_cameraRect.width / 2.0f,
        m_cameraRect.y + m_cameraRect.height / 2.0f};
    m_targetCenter = {0.0f, 0.0f}; // Init to zero might be bad???
    m_mapSize = {
        static_cast<float>(map.tileWidth) * static_cast<float>(map.mapWidth),
        static_cast<float>(map.tileHeight) * static_cast<float>(map.mapHeight)
    };
    m_maxCameraPos = {
        m_mapSize.x - m_cameraRect.width / 2.0f,
        m_mapSize.y - m_cameraRect.height / 2.0f
    };

    TraceLog(LOG_INFO, "SceneCamera created successfully.");
}

SceneCamera::~SceneCamera() = default;

void SceneCamera::setTarget(const std::shared_ptr<Player>& player) {
    Vector2 targetEntityCenter = Vector2Add(player->getPositionCornerPx(), player->getSizePx() / 2.0f);

    m_camera.target.y = targetEntityCenter.y;
    m_camera.target.x = targetEntityCenter.x;

    m_camera.target = Vector2Clamp(
        m_camera.target,
        {m_cameraRect.width / 2.0f, m_cameraRect.height / 2.0f},
        m_maxCameraPos);
}

void SceneCamera::update(const std::shared_ptr<Player>& player) {
    m_targetCenter = {
        player->getPositionCornerPx().x + player->getSizePx().x / 2.0f,
        player->getPositionCornerPx().y + player->getSizePx().y / 2.0f
    };

    m_camera.target = m_targetCenter;
    m_camera.target = Vector2Clamp(
        m_camera.target,
        {m_cameraRect.width / 2.0f, m_cameraRect.height / 2.0f},
        m_maxCameraPos);

    m_cameraRect.x = m_camera.target.x - m_cameraRect.width / 2.0f;
    m_cameraRect.y = m_camera.target.y - m_cameraRect.height / 2.0f;

    m_cameraCenter.x = m_cameraRect.x + m_cameraRect.width / 2.0f;
    m_cameraCenter.y = m_cameraRect.y + m_cameraRect.height / 2.0f;
}

// Deprecate
void SceneCamera::cameraBegin() const {
    BeginMode2D(m_camera);
}

void SceneCamera::cameraEnd() const {
    EndMode2D();
}

[[nodiscard]] Rectangle SceneCamera::getCameraRect() const noexcept {
    return m_cameraRect;
}

[[nodiscard]] float SceneCamera::getCameraRectWidth() const noexcept {
    return m_cameraRect.width;
}

[[nodiscard]] float SceneCamera::getCameraRectHeight() const noexcept {
    return m_cameraRect.height;
}

[[nodiscard]] Vector2 SceneCamera::getCameraCenter() const noexcept {
    return m_cameraCenter;
}

[[nodiscard]] Vector2 SceneCamera::getCameraTarget() const noexcept {
    return m_camera.target;
}

[[nodiscard]] Camera2D SceneCamera::getCamera() const noexcept {
    return m_camera;
}
