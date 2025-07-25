//
// Created by DirgeWuff on 5/15/25.
//

// The following is a heavily modified version of Rob Loach's raylib-tileson implementation,
// with classes, improvements to memory safety, const correctness, error handling,
// and the addition of calculations for parallax.

// Link to the original: https://github.com/RobLoach/raylib-tileson.
// Used and modified under the permissions granted by the BSD license.

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
                "Constructor init failed: Unable to parse map: " + map->getStatusMessage() + "Ln 43, Tilemap.cpp");
            return;
        }

        const std::shared_ptr<TilesonData> data = std::make_shared<TilesonData>();

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

                    // MAY cause problems treating Polyline and Polygon the same (it does!)
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
                        logErr("Object layer contains incompatible type. Ln 81, Tilemap.cpp");
                        return;
                    }
                }
            }
        }

        for (const auto& image : map->getTilesets()) {
            std::string imagePath;

            if (!mapData.baseDir.empty()) {
                imagePath = mapData.baseDir.string() + image.getImage().string();
            }
            else {
                imagePath = image.getImage();
            }

            const Texture texture = LoadTexture(imagePath.c_str());
            data->textures[imagePath] = texture;
        }

        mapData.mapWidth = map->getSize().x;
        mapData.mapHeight = map->getSize().y;
        mapData.tileWidth = map->getTileSize().x;
        mapData.tileHeight = map->getTileSize().y;
        data->map = std::move(map);
        mapData.data = data;
    }
    catch (std::bad_alloc) {
        logErr("Constructor init failed, std::bad_alloc thrown Ln 110, Tilemap.cpp");
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


void TiledMap::drawMap(
    const SceneCamera& cam,
    const Vector2 offset,
    const Color color) const
{
    for (std::vector<tson::Layer>& layers = mapData.data->map->getLayers(); auto& layer : layers) {

        const Vector2 parallaxFactor = toRayVec2(layer.getParallax());
        const Vector2 layerOffset = toRayVec2(layer.getOffset());
        const Vector2 parallaxOrigin = toRayVec2(layer.getMap()->getParallaxOrigin());
        const Vector2 cameraOffset = GetWorldToScreen2D(parallaxOrigin, *cam.getCameraPtr());

        // Unsure why / 10 was required to get the correct value??? Never seen this in any sample code.
        const Vector2 adjustedOffset = offset + layerOffset - cameraOffset * (Vector2{1.0, 1.0} - parallaxFactor) / 10;

        switch (layer.getType()) {
            case tson::LayerType::TileLayer: {
                for (auto& [pos, tile] : layer.getTileObjects()) {
                    const tson::Tileset* tileset = tile.getTile()->getTileset();

                    const Vector2 position = toRayVec2(tile.getPosition());
                    const Rectangle drawingRect = toRayRect(tile.getDrawingRect());

                    const Rectangle tileBounds = {
                        position.x + adjustedOffset.x,
                        position.y + adjustedOffset.y,
                        drawingRect.width,
                        drawingRect.height
                    };

                    // Camera cull
                    if (!CheckCollisionRecs(tileBounds, cam.getCameraRect())) {
                        continue;
                    }

                    std::string tilesetPath = mapData.baseDir.string() + tileset->getImage().string();

                    if (!mapData.data->textures.contains(tilesetPath)) {
                        break;
                    }

                    DrawTextureRec(
                        mapData.data->textures[tilesetPath],
                        drawingRect,
                        {position.x + adjustedOffset.x, position.y + adjustedOffset.y},
                        color);
                }
                break;
            }

            case tson::LayerType::ImageLayer: {
                const std::string imagePath = mapData.baseDir.string() + layer.getImage();
                const tson::Vector2f imageLayerOffset = layer.getOffset();

                if (!mapData.data->textures.contains(imagePath)) {
                    logErr("Unable to load texture: " + layer.getImage() + ". Ln 181, TileMap.cpp.");
                    return;
                }

                DrawTexture(
                    mapData.data->textures[imagePath],
                    imageLayerOffset.x - adjustedOffset.x,
                    imageLayerOffset.y - adjustedOffset.y,
                    color);

                break;
            }

            // Need to handle this even though it's a NOP, otherwise default is hit
            case tson::LayerType::ObjectGroup: {
                break;
            }

            default: {
                logErr("Map contains unsupported layer type. Ln 200, Tilemap.cpp");
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