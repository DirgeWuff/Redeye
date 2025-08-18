//
// Created by DirgeWuff on 5/26/25.
//

#ifndef CAMERA_H
#define CAMERA_H
#include "raylib.h"
#include "Entity.h"
#include "Tilemap.h"

class TiledMap;

class SceneCamera final : public Camera2D {
   std::shared_ptr<Camera2D> m_camera;    // Base Raylib Camera2D ptr
   Rectangle m_cameraRect;                // Rectangle defining the camera's bounds
   Vector2 m_cameraCenter;                // The center point of the camera
   Vector2 m_targetCenter;                // The center of the camera's target
   Vector2 m_mapSize;                     // The size of the map the camera was created on, used to clamp
   Vector2 m_maxCameraPos;                // The maximum x/y position the camera have without viewing outside map bounds
public:
   SceneCamera() = default;
   SceneCamera(const TiledMap& map, float zoomLevel);
   ~SceneCamera();
   void setTarget(const std::shared_ptr<Entity>& targetEntity) const;         // Set the camera to target an entity
   void update(const std::shared_ptr<Entity>& targetEntity);                  // Update the camera's target, based on entity position
   void cameraBegin() const;                                                  // Begin camera context
   void cameraEnd() const;                                                    // End camera context
   [[nodiscard]] Rectangle getCameraRect() const noexcept;                    // Get the camera's view rectangle
   [[nodiscard]] float getCameraRectWidth() const noexcept;                   // Get the camera's view width
   [[nodiscard]] float getCameraRectHeight() const noexcept;                  // Get the camera's view height
   [[nodiscard]] Vector2 getCameraCenter() const noexcept;                    // Get the camera's center
   [[nodiscard]] Vector2 getCameraTarget() const noexcept;                    // Get the camera's target pos
   [[nodiscard]] std::shared_ptr<Camera2D> getCameraPtr() const noexcept;     // Get the camera2D's ptr
};

#endif //CAMERA_H
