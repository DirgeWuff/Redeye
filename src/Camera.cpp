//
// Created by DirgeWuff on 5/26/25.
//

#include <memory>
#include "raylib.h"
#include "raymath.h"
#include "Camera.h"
#include "Error.h"

SceneCamera::SceneCamera(const TiledMap& map, const float zoomLevel) {
    try {
        m_camera = std::make_shared<Camera2D>();
        m_camera->offset = {1500.0f / 2.0f, 800.0f / 2.0f}; // Hard coded value, no bueno, fix later
        m_camera->zoom = zoomLevel;
        m_camera->rotation = 0.0f;

        m_cameraRect = {
            (GetScreenWidth() - static_cast<float>(GetScreenWidth()) / m_camera->zoom) / 2.0f,
            (GetScreenHeight() - static_cast<float>(GetScreenHeight()) / m_camera->zoom) / 2.0f,
            GetScreenWidth() / m_camera->zoom,
            GetScreenHeight() / m_camera->zoom
        };

        m_cameraCenter = {m_cameraRect.x + m_cameraRect.width / 2.0f, m_cameraRect.y + m_cameraRect.height / 2.0f};
        m_targetCenter = {0.0f, 0.0f}; // Init to zero might be bad???
        m_mapSize = {
            map.getTileWidth() * map.getMapWidth(),
            map.getTileHeight() * map.getMapHeight()
        };
        m_maxCameraPos = {
            m_mapSize.x - m_cameraRect.width / 2.0f,
            m_mapSize.y - m_cameraRect.height / 2.0f
        };
    }
    catch (std::bad_alloc) {
        logErr(
            "Constructor init failed, std::bad_alloc thrown. Ln 39, Camera.cpp.");
    }
    catch (...) {
        logErr(
            "Constructor init failed, unknown error thrown. Ln 43, Camera.cpp.");
    }

    TraceLog(LOG_INFO, "SceneCamera created successfully.");
}

SceneCamera::~SceneCamera() = default;

void SceneCamera::setTarget(const std::shared_ptr<Entity>& targetEntity) const {
    Vector2 targetEntityCenter = Vector2Add(targetEntity->getPositionCornerPx(), targetEntity->getSizePx() / 2);

    if (m_camera == nullptr) {
        logErr("m_camera == nullptr. Ln 55, Camera.cpp");
        return;
    }

    m_camera->target.y = targetEntityCenter.y;
    m_camera->target.x = targetEntityCenter.x;

    m_camera->target = Vector2Clamp(
        m_camera->target,
        {m_cameraRect.width / 2.0f, m_cameraRect.height / 2.0f},
        m_maxCameraPos);
}

void SceneCamera::update(const std::shared_ptr<Entity>& targetEntity) {
    if (targetEntity == nullptr) {
        logErr("targetEntity == nullptr. Ln 70, Camera.cpp.");
        return;
    }

    m_targetCenter = {
        targetEntity->getPositionCornerPx().x + targetEntity->getSizePx().x / 2.0f,
        targetEntity->getPositionCornerPx().y + targetEntity->getSizePx().y / 2.0f
    };

    m_camera->target = m_targetCenter;
    m_camera->target = Vector2Clamp(
        m_camera->target,
        {m_cameraRect.width / 2.0f, m_cameraRect.height / 2.0f},
        m_maxCameraPos);

    m_cameraRect.x = m_camera->target.x - m_cameraRect.width / 2.0f;
    m_cameraRect.y = m_camera->target.y - m_cameraRect.height / 2.0f;

    m_cameraCenter.x = m_cameraRect.x + m_cameraRect.width / 2.0f;
    m_cameraCenter.y = m_cameraRect.y + m_cameraRect.height / 2.0f;
}

// Unfortunate I have to use these stupid interface functions, but it is what it is
void SceneCamera::cameraBegin() const {
    BeginMode2D(*m_camera);
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

// NOTE: Changed this recently, if shit starts breaking, look here!
[[nodiscard]] Vector2 SceneCamera::getCameraCenter() const noexcept {
    return m_cameraCenter;
}

[[nodiscard]] Vector2 SceneCamera::getCameraTarget() const noexcept {
    return m_camera->target;
}

[[nodiscard]] std::shared_ptr<Camera2D> SceneCamera::getCameraPtr() const noexcept {
    return m_camera;
}
