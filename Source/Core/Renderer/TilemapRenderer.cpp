//
// Created by DirgeWuff on 9/5/2025.
//

#include "raylib.h"
#include "raymath.h"
#include "TilemapRenderer.h"

// Very sloppy way of dealing with layers of render pass modes in the same function.
// Tried to do this using groups, however there seems to be a bug in tson that causes tiled layers not to load properly
// this seems like the second-best option.

void renderLayer(
    const SceneCamera& cam,
    const MapData& map,
    const tson::Layer& layer,
    const Vector2 offset,
    const Color color)
{
    const Vector2 parallaxFactor = toRayVec2(layer.getParallax());
    const Vector2 layerOffset = toRayVec2(layer.getOffset());
    const Vector2 cameraTarget = cam.getCameraTarget();
    const Vector2 parallaxOffset = cameraTarget * (Vector2{1.0f, 1.0f} - parallaxFactor);
    const Vector2 adjustedOffset = offset + layerOffset + parallaxOffset;

    switch (layer.getType()) {
        case tson::LayerType::TileLayer: {
            const auto it = map.renderDataPtr->layerRenderData.find(&layer);
            if (it == map.renderDataPtr->layerRenderData.end()) return;

            for (const auto& tile : it->second) {
                const Vector2 drawingPos = {
                    tile.position.x + adjustedOffset.x,
                    tile.position.y + adjustedOffset.y
                };

                const Rectangle tileBounds = {
                    drawingPos.x,
                    drawingPos.y,
                    tile.sourceRect.width,
                    tile.sourceRect.height
                };

                // Camera cull
                if (!CheckCollisionRecs(tileBounds, cam.getCameraRect())) {
                    continue;
                }

                DrawTextureRec(
                    *tile.texture,
                    tile.sourceRect,
                    drawingPos,
                    color);
            }

            break;
        }
        case tson::LayerType::ImageLayer: {
            const std::string imagePath = map.baseDir.string() + layer.getImage();

            if (!map.renderDataPtr->textures.contains(imagePath)) {
                logErr("Unable to load texture: " + layer.getImage() + ": renderMap(Args...)");
                return;
            }

            if (layer.hasRepeatX()) {
                const float camWidth = cam.getCameraRectWidth();
                const float imageWidth = map.renderDataPtr->textures[imagePath].width;
                const int mapWidth = map.mapWidth * map.tileWidth;
                const float camTravelX = mapWidth - camWidth;
                const float totalParallaxShift = camTravelX * (1.0f - parallaxFactor.x);

                const std::size_t maxRepetitions = std::ceil((camWidth + totalParallaxShift) / imageWidth);
                const int yPos = adjustedOffset.y;
                int xPos = adjustedOffset.x;

                for (std::size_t i = 0; i < maxRepetitions; i++) {
                    DrawTexture(
                        map.renderDataPtr->textures[imagePath],
                        xPos,
                        yPos,
                        color);

                    xPos += imageWidth;
                }
            }
            else {
                DrawTexture(
                    map.renderDataPtr->textures[imagePath],
                    adjustedOffset.x,
                    adjustedOffset.y,
                    color);
            }

            break;
        }
        // Need to handle this even though it's a NOP, otherwise default is hit
        case tson::LayerType::ObjectGroup: {
            break;
        }
        default: {
            logErr("Map contains unsupported layer type: renderMap(Args...)");
            break;
        }
    }
}

void renderLayerGroup(
    const SceneCamera& cam,
    const MapData& map,
    const Vector2 offset,
    const Color color,
    const renderPassType mode)
{
    for (auto& layer : map.tsonMapPtr->getLayers()) {
        if (layer.getClassType() == "DifferedLayer" && mode == renderPassType::PRIMARY_PASS) continue;
        if (layer.getClassType() == "PrimaryLayer" && mode == renderPassType::DIFFERED_PASS) continue;

        renderLayer(cam, map, layer, offset, color);
    }
}

// Don't need these, but it makes intentions clearer IMO
void renderBackgroundLayers(
    const SceneCamera& cam,
    const MapData& map,
    const Vector2 offset,
    const Color color)
{
    renderLayerGroup(cam, map, offset, color, renderPassType::PRIMARY_PASS);
}

void renderForegroundLayers(
    const SceneCamera& cam,
    const MapData &map,
    const Vector2 offset,
    const Color color)
{
    renderLayerGroup(cam, map, offset, color, renderPassType::DIFFERED_PASS);
}
