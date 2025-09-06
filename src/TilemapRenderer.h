//
// Created by DirgeWuff on 9/5/2025.
//

#ifndef TILEMAPRENDERER_H
#define TILEMAPRENDERER_H

#include "raylib.h"
#include "Camera.h"
#include "Tilemap.h"

void renderMap(
    const std::unique_ptr<SceneCamera>& cam,
    const MapData& map,
    Vector2 offset,
    Color color);

#endif //TILEMAPRENDERER_H
