//
// Created by DirgeWuff on 6/27/25.
//

#ifndef DEBUG_H
#define DEBUG_H

#include <memory>
#include "../Camera/Camera.h"
#include "../../Core/Entity/Player.h"

// Draw all the shapes bound to a Player object for debugging
void drawDebugBodyShapes(const std::shared_ptr<Player>& player);

// Draw the center of an Entity for debugging
void drawDebugBodyCenter(const std::shared_ptr<Player>& player);

// Draw "crosshair" at center of camera for debugging
void drawDebugCameraCrosshair(const std::unique_ptr<SceneCamera>& camera);

// Draw rectangle representing the camera frame for debugging
void drawDebugCameraRect(const std::unique_ptr<SceneCamera>& camera);

// Draw the current position of the player for debugging
// Must be called AFTER SceneCamera->cameraEnd()
void drawDebugPlayerPosition(const std::shared_ptr<Player>& player);

// Draw contact status of player footpaw sensor for debugging
// Must be called AFTER SceneCamera->cameraEnd()
void drawDebugFootpawSensorStatus(const std::shared_ptr<Player>& player);

// Draw collision shapes in a world, based on vertices in collisionWorld_t
void drawDebugCollisionShapes(const MapData& map);

// Draw collision vertices in a world, based on collisionWorld_t
void drawDebugCollisionVerts(const MapData& map);

// Draw event colliders in the world
void drawDebugEventColliders(const MapData& map);

// Draw controls window for debugging features
// Must be called AFTER SceneCamera->cameraEnd()
void drawControlsWindow();

#endif //DEBUG_H
