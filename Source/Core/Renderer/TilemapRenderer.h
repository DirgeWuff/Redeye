//
// Created by DirgeWuff on 9/5/2025.
//

#ifndef TILEMAPRENDERER_H
#define TILEMAPRENDERER_H

#include "raylib.h"
#include "../Camera/Camera.h"
#include "../Renderer/Tilemap.h"

void renderLayer(
    const SceneCamera& cam,
    const MapData& map,
    const tson::Layer& layer,
    Vector2 offset,
    Color color);

void renderLayerGroup(
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
