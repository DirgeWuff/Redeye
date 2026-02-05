//
// Author: DirgeWuff
// Created on: 6/27/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Collection of definitions for debugging functions. These functions perform
// a large array of tasks, but most are drawing related. These features
// are removed from release mode using guards 


#ifndef DEBUG_H
#define DEBUG_H

#include "../Camera/Camera.h"
#include "../../Core/Entity/Player.h"
#include "./Enum.h"

namespace RE::Core {
    // Draw all the shapes bound to a Player object for debugging
    void drawDebugBodyShapes(const Player& player);

    // Draw the center of an Entity for debugging
    void drawDebugBodyCenter(const Player& player);

    // Draw "crosshair" at center of camera for debugging
    void drawDebugCameraCrosshair(const SceneCamera& camera);

    // Draw rectangle representing the camera frame for debugging
    void drawDebugCameraRect(const SceneCamera& camera);

    // Draw the current position of the player for debugging
    // Must be called AFTER SceneCamera->cameraEnd()
    void drawDebugPlayerPosition(const Player& player);

    // Draw contact status of player footpaw sensor for debugging
    // Must be called AFTER SceneCamera->cameraEnd()
    void drawDebugFootpawSensorStatus(const Player& player);

    // Draw collision shapes in a world, based on vertices in collisionWorld_t
    void drawDebugCollisionShapes(const MapData& map);

    // Draw collision vertices in a world, based on collisionWorld_t
    void drawDebugCollisionVerts(const MapData& map);

    // Draw event colliders in the world
    void drawDebugEventColliders(const MapData& map);

    // Draw the current animation ID of the player
    // Must be called AFTER SceneCamera->cameraEnd()
    void drawDebugPlayerAnimId(const animationId& id);

    // Draw the current player animation state
    // Must be called AFTER SceneCamera->cameraEnd()
    void drawDebugPlayerAnimState(const entityActionState& state);

    // Draw controls window for debugging features
    // Must be called AFTER SceneCamera->cameraEnd()
    void drawControlsWindow();
}

#endif //DEBUG_H
