//
// Author: DirgeWuff
// Created on: 5/26/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class declaration for a moving camera capable of tracking the player.
// Provides an interface for creating a 2D tracking camera as well as
// querying it for various information.

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
   SceneCamera();
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
   [[nodiscard]] float getCameraZoom() const noexcept;
   [[nodiscard]] Vector2 getCameraCenter() const noexcept;
   [[nodiscard]] Vector2 getCameraTarget() const noexcept;
   [[nodiscard]] Vector2 getCameraOffset() const noexcept;
   [[nodiscard]] Vector2 getToWorldCam(const Vector2& position) const;
};

#endif //CAMERA_H
