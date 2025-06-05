//
// Created by DirgeWuff on 5/26/25.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <raylib.h>
#include "Entity.h"
#include "Tilemap.h"

class TiledMap; // Lazy forward declaration to fix circular dependency

class SceneCamera : public Camera2D {
   std::shared_ptr<Camera2D> camera;
   Rectangle cameraRect;
   Vector2 cameraCenter;
   float cameraVelocityY;
   Vector2 targetCenter;
   Vector2 mapSize;
public:
   SceneCamera() = default;
   SceneCamera(const TiledMap& map);
   ~SceneCamera();
   void setTarget(const std::shared_ptr<Entity>& targetEntity);
   void update(const std::shared_ptr<Entity>& targetEntity);
   void cameraBegin() const;
   void cameraEnd() const;
   Rectangle getCameraRect() const;
   Vector2 getCameraCenter() const;
   Vector2 getCameraTarget() const;
   std::shared_ptr<Camera2D> getCameraPtr() const;
   void drawCameraDebugRect() const;
   void drawDebugCrosshair() const;

};

#endif //CAMERA_H
