//
// Created by DirgeWuff on 5/15/25.
//


#include "Tilemap.h"
#include "Error.h"
#include "Utils.h"
#include "raymath.h"

TiledMap::TiledMap() {
    mapData.mapWidth = NULL;
    mapData.mapHeight = NULL;
    mapData.tileWidth = NULL;
    mapData.tileHeight = NULL;
    mapData.data = nullptr;
    mapData.collisionObjects = {};
}

TiledMap::TiledMap(std::string&& filepath, b2WorldId world)  {

    mapData.baseDir = fs::relative(filepath);
    if (mapData.baseDir.has_extension()) {
        mapData.baseDir.remove_filename();
    }

    try {
        tson::Tileson t;
        std::unique_ptr<tson::Map> map = t.parse(fs::path(filepath));

        if (map->getStatus() != tson::ParseStatus::OK) {
            logErr(
                "Constructor init failed: Unable to parse map: " + map->getStatusMessage() + "Ln 32, Tilemap.cpp");
            return;
        }

        const std::shared_ptr<TilesonData> data = std::make_shared<TilesonData>();

        for (const auto& tileset : map->getTilesets()) {
            std::string imagePath;

            if (!mapData.baseDir.empty()) {
                imagePath = mapData.baseDir.string() + tileset.getImage().string();
            }
            else {
                imagePath = tileset.getImage();
            }

            if (!data->textures.contains(imagePath)) {
                const Texture texture = LoadTexture(imagePath.c_str());
                data->textures[imagePath] = texture;
            }

            data->texturePtrs[&tileset] = &data->textures[imagePath];
        }

        for (auto& layer : map->getLayers()) {
            if (layer.getType() == tson::LayerType::ImageLayer) {
                std::string imagePath;

                if (!mapData.baseDir.empty()) {
                    imagePath = mapData.baseDir.string() + layer.getImage();
                }
                else {
                    imagePath = layer.getImage();
                }

                const Texture texture = LoadTexture(imagePath.c_str());
                data->textures[imagePath] = texture;
            }
            else if (layer.getType() == tson::LayerType::ObjectGroup) {
                for (auto& object : layer.getObjects()) {
                    tson::ObjectType objType = object.getObjectType();

                    if (objType == tson::ObjectType::Polyline) {
                        tson::Vector2i pos = object.getPosition();
                        std::vector<tson::Vector2i> tsonPoints = object.getPolylines();
                        std::vector<b2Vec2> points;

                        for (std::size_t i = 0; i < tsonPoints.size(); i++) {
                            tsonPoints[i] = v2iAdd(tsonPoints[i], pos);
                            points.push_back(toB2Vec2(tsonPoints[i]));
                        }

                        mapData.collisionObjects.emplace_back(world, points);
                    }
                    else {
                        logErr("Object layer contains incompatible type. Ln 88, Tilemap.cpp");
                        return;
                    }
                }
            }
            else if (layer.getType() == tson::LayerType::TileLayer) {
                std::vector<TileData> renderData;

                for (auto& [pos, tile] : layer.getTileObjects()) {
                    const tson::Tile* tilePtr = tile.getTile();
                    const tson::Tileset* tileset = tilePtr->getTileset();

                    auto iterator = data->texturePtrs.find(tileset);
                    if (iterator == data->texturePtrs.end()) {
                        logErr("Texture missing. Ln 102, Tilemap.cpp");
                        continue;
                    }

                    TileData tileData;
                    tileData.position = toRayVec2(tile.getPosition());
                    tileData.sourceRect = toRayRect(tile.getDrawingRect());
                    tileData.texture = iterator->second;

                    renderData.push_back(tileData);
                }

                data->layerRenderData[&layer] = std::move(renderData);
            }
        }

        mapData.mapWidth = map->getSize().x;
        mapData.mapHeight = map->getSize().y;
        mapData.tileWidth = map->getTileSize().x;
        mapData.tileHeight = map->getTileSize().y;
        data->map = std::move(map);
        mapData.data = data;
    }
    catch (std::bad_alloc) {
        logErr("Constructor init failed, std::bad_alloc thrown Ln 126, Tilemap.cpp");
    }

    TraceLog(LOG_INFO, "Tiled map loaded successfully.");
}

// NOTE: Seems to throw OpenGL error on texture unload, likely related to Apple Silicon chip
TiledMap::~TiledMap() {
    for (const auto& [name, texture] : mapData.data->textures) {
        UnloadTexture(texture);
    }

    TraceLog(LOG_INFO, "Tiled map unloaded successfully.");
}

void TiledMap::draw(
    const SceneCamera& cam,
    const Vector2 offset,
    const Color color) const
{
    for (const std::vector<tson::Layer>& layers = mapData.data->map->getLayers(); auto& layer : layers) {

        const Vector2 parallaxFactor = toRayVec2(layer.getParallax());
        const Vector2 layerOffset = toRayVec2(layer.getOffset());
        const Vector2 cameraTarget = cam.getCameraPtr()->target;
        const Vector2 parallaxOffset = cameraTarget * (Vector2{1.0f, 1.0f} - parallaxFactor);

        const Vector2 adjustedOffset = offset + layerOffset + parallaxOffset;

        switch (layer.getType()) {
            case tson::LayerType::TileLayer: {
                const std::vector<TileData> renderDataVec = mapData.data->layerRenderData[&layer];

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
                const std::string imagePath = mapData.baseDir.string() + layer.getImage();

                if (!mapData.data->textures.contains(imagePath)) {
                    logErr("Unable to load texture: " + layer.getImage() + ". Ln 190, TileMap.cpp.");
                    return;
                }

                DrawTexture(
                    mapData.data->textures[imagePath],
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
                logErr("Map contains unsupported layer type. Ln 209, Tilemap.cpp");
                break;
            }
        }
    }
}

// Return the width of the map in tiles
float TiledMap::getMapWidth() const {
    return static_cast<float>(mapData.mapWidth);
}

// Return the height of the map in tiles
float TiledMap::getMapHeight() const {
   return static_cast<float>(mapData.mapHeight);
}

// Return the width of a tile in pixels
float TiledMap::getTileWidth() const {
    return static_cast<float>(mapData.tileWidth);
}

// Return the height of a tile in pixels
float TiledMap::getTileHeight() const {
    return static_cast<float>(mapData.tileHeight);
}

// Return collision shapes in this->mapData
collisionWorld_t TiledMap::getCollisionShapes() const {
    return mapData.collisionObjects;
}