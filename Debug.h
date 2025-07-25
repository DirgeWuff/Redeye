//
// Created by DirgeWuff on 6/27/25.
//

#ifndef DEBUG_H
#define DEBUG_H

// #define DEBUG_DRAW_PLAYER_SHAPES
// #define DEBUG_DRAW_PLAYER_SENSOR_STATUS
// #define DEBUG_DRAW_PLAYER_POSITION
// #define DEBUG_DRAW_TERRAIN_SHAPES
// #define DEBUG_DRAW_CAMERA_CROSSHAIR
// #define DEBUG_DRAW_CAMERA_RECT


#include "Camera.h"
#include "Entity.h"
#include "Player.h"

// Draw all the shapes bound to an Entity for debugging
void drawDebugBodyShapes(const Entity* targetEntity);

// Draw the center of an Entity for debugging
void drawDebugBodyCenter(const Entity* targetEntity);

// Draw "crosshair" at center of camera for debugging
void drawDebugCameraCrosshair(const std::unique_ptr<SceneCamera>& camera);

// Draw rectangle representing the camera frame for debugging
void drawDebugCameraRect(const std::unique_ptr<SceneCamera>& camera);

// Draw the current position of the player for debugging
void drawDebugPlayerPosition(
    const std::shared_ptr<Player>& player,
    const std::unique_ptr<SceneCamera>& camera);

// Draw contact status of player footpaw sensor for debugging
void drawDebugFootpawSensorStatus(
    const std::shared_ptr<Player>& player,
    const std::unique_ptr<SceneCamera>& camera);

// Draw collision shapes in a world, based on vertices in collisionWorld_t
void drawDebugCollisionShapes(const std::unique_ptr<TiledMap>& map);

#endif //DEBUG_H
