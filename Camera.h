//
// Created by DirgeWuff on 5/26/25.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <raylib.h>
#include "Entity.h"
#include "Tilemap.h"

class TiledMap; // Lazy forward declaration to fix circular dependency

class SceneCamera final : public Camera2D {
   std::shared_ptr<Camera2D> m_camera;
   Rectangle m_cameraRect;
   Vector2 m_cameraCenter;
   float m_cameraVelocityY; // Probably not needed anymore?
   Vector2 m_targetCenter;
   Vector2 m_mapSize;
   Vector2 m_maxCameraPos; // Used for clamping
public:
   SceneCamera() = default;
   SceneCamera(const TiledMap& map, float zoomLevel);
   ~SceneCamera();
   void setTarget(const std::shared_ptr<Entity>& targetEntity) const;
   void update(const std::shared_ptr<Entity>& targetEntity);
   void cameraBegin() const;
   void cameraEnd() const;
   [[nodiscard]] Rectangle getCameraRect() const noexcept;
   [[nodiscard]] float getCameraRectWidth() const noexcept;
   [[nodiscard]] float getCameraRectHeight() const noexcept;
   [[nodiscard]] Vector2 getCameraCenter() const noexcept;
   [[nodiscard]] Vector2 getCameraTarget() const noexcept;
   [[nodiscard]] std::shared_ptr<Camera2D> getCameraPtr() const noexcept;
};

#endif //CAMERA_H
