//
// Created by DirgeWuff on 5/26/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "../Entity/Player.h"
#include "../Renderer/Tilemap.h"

class SceneCamera final : public Camera2D {
   Camera2D m_camera{};
   Rectangle m_cameraRect{};
   Vector2 m_cameraCenter{};
   Vector2 m_targetCenter{};
   Vector2 m_mapSize{};
   Vector2 m_maxCameraPos{};
public:
   SceneCamera() = default;
   SceneCamera(const MapData& map, float zoomLevel);

   SceneCamera(SceneCamera&) = delete;
   SceneCamera(SceneCamera&&) = default;
   SceneCamera& operator=(SceneCamera&) = delete;
   SceneCamera& operator=(SceneCamera&&) = default;

   ~SceneCamera();

   void setTarget(const Player& player);
   void update(const Player& player);
   void cameraBegin() const;
   void cameraEnd() const;
   [[nodiscard]] Rectangle getCameraRect() const noexcept;
   [[nodiscard]] float getCameraRectWidth() const noexcept;
   [[nodiscard]] float getCameraRectHeight() const noexcept;
   [[nodiscard]] Vector2 getCameraCenter() const noexcept;
   [[nodiscard]] Vector2 getCameraTarget() const noexcept;
   [[nodiscard]] Camera2D getCamera() const noexcept;
};

#endif //CAMERA_H
