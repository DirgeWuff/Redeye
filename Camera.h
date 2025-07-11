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
   explicit SceneCamera(const TiledMap& map);
   ~SceneCamera();
   void setTarget(const std::shared_ptr<Entity>& targetEntity) const;
   void update(const std::shared_ptr<Entity>& targetEntity);
   void cameraBegin() const;
   void cameraEnd() const;
   Rectangle getCameraRect() const;
   float getCameraRectWidth() const;
   float getCameraRectHeight() const;
   Vector2 getCameraCenter() const;
   Vector2 getCameraTarget() const;
   std::shared_ptr<Camera2D> getCameraPtr() const;
};

#endif //CAMERA_H
