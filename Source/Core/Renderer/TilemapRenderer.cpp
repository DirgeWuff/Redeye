//
// Created by DirgeWuff on 9/5/2025.
//

#include "raylib.h"
#include "raymath.h"
#include "TilemapRenderer.h"

void renderMap(
    const SceneCamera& cam,
    const MapData& map,
    const Vector2 offset,
    const Color color)
{
    const std::vector<tson::Layer>& layers = map.renderDataPtr->tsonMapPtr->getLayers();
    for (auto& layer : layers) {

        const Vector2 parallaxFactor = toRayVec2(layer.getParallax());
        const Vector2 layerOffset = toRayVec2(layer.getOffset());
        const Vector2 cameraTarget = cam.getCameraTarget();
        const Vector2 parallaxOffset = cameraTarget * (Vector2{1.0f, 1.0f} - parallaxFactor);

        const Vector2 adjustedOffset = offset + layerOffset + parallaxOffset;

        switch (layer.getType()) {
            case tson::LayerType::TileLayer: {
                const std::vector<TileData> renderDataVec = map.renderDataPtr->layerRenderData[&layer];

                for (const auto& data : renderDataVec) {
                    const Vector2 drawingPos = {
                        data.position.x + adjustedOffset.x,
                        data.position.y + adjustedOffset.y
                    };

                    const Rectangle tileBounds = {
                        drawingPos.x,
                        drawingPos.y,
                        data.sourceRect.width,
                        data.sourceRect.height
                    };

                    // Camera cull
                    if (!CheckCollisionRecs(tileBounds, cam.getCameraRect())) {
                        continue;
                    }

                    DrawTextureRec(
                        *data.texture,
                        data.sourceRect,
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

                DrawTexture(
                    map.renderDataPtr->textures[imagePath],
                    adjustedOffset.x,
                    adjustedOffset.y,
                    color);

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
}