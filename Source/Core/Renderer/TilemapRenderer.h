//
// Author: DirgeWuff
// Created on: 9/5/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Function declarations for map rendering. Includes static functions that
// are called in the main rendering function, and wrapper functions that
// are used externally to perform draw calls on a MapData object.

#ifndef TILEMAPRENDERER_H
#define TILEMAPRENDERER_H

#include "raylib.h"
#include "../Camera/Camera.h"
#include "../Renderer/Tilemap.h"

static void renderLayer(
    const SceneCamera& cam,
    const MapData& map,
    const tson::Layer& layer,
    Vector2 offset,
    Color color);

static void renderLayerGroup(
    const SceneCamera& cam,
    const MapData& map,
    Vector2 offset,
    Color color,
    renderPassType mode);

void renderBackgroundLayers(
    const SceneCamera& cam,
    const MapData& map,
    Vector2 offset,
    Color color);

void renderForegroundLayers(
    const SceneCamera& cam,
    const MapData& map,
    Vector2 offset,
    Color color);

#endif //TILEMAPRENDERER_H
