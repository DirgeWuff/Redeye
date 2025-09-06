//
// Created by DirgeWuff on 5/15/25.
//

#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <map>
#include <filesystem>
#include "raylib.h"
#include "../external_libs/Tson/tileson.hpp"
#include "CollisionObject.h"
#include "EventCollider.h"
#include "Error.h"
#include "Utils.h"

class SceneCamera;
class EventCollider;

// Struct containing relevant data about a given tile. Used for fast drawing.
struct TileData {
    Vector2 position;
    Rectangle sourceRect;
    const Texture2D* texture;
};

// Structured data used to render a map.
struct RenderData {
    std::map<std::string, Texture> textures;
    std::unordered_map<const tson::Tileset*, Texture2D*> texturePtrs;
    std::unordered_map<const tson::Layer*, std::vector<TileData>> layerRenderData;
    std::shared_ptr<tson::Map> tsonMapPtr;
};

// Structured data used to load a map. Used on a per-map basis.
struct MapData {
    uint8_t tileWidth;
    uint8_t tileHeight;
    uint16_t mapWidth;
    uint16_t mapHeight;

    fs::path baseDir;
    std::shared_ptr<RenderData> renderDataPtr;
    std::vector<CollisionObject> collisionObjects;
    std::unordered_map<std::string, EventCollider> eventColliders;
};

template<typename T>
MapData loadMap(const T& filepath, b2WorldId world) {
    MapData mapData;

    if (!exists(std::filesystem::path(filepath))) {
        logErr(std::string("Invalid filepath: " + std::string(filepath)));
        return {};
    }

    mapData.baseDir = std::filesystem::relative(filepath);
    if (mapData.baseDir.has_extension()) {
        mapData.baseDir.remove_filename();
    }

    try {
        tson::Tileson t;
        std::unique_ptr<tson::Map> map = t.parse(std::filesystem::path(filepath));

        if (map->getStatus() != tson::ParseStatus::OK) {
            logErr(
                "Constructor init failed: Unable to parse map: " + map->getStatusMessage() + ". Ln 64, Tilemap.h");
            return {};
        }

        const std::shared_ptr<RenderData> data = std::make_shared<RenderData>();

        for (const auto& tileset : map->getTilesets()) {
            std::string imagePath;

            if (!mapData.baseDir.empty()) {
                imagePath = mapData.baseDir.string() + tileset.getImage().string();
            }
            else {
                imagePath = tileset.getImage().string();
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
            else if (layer.getType() == tson::LayerType::ObjectGroup && layer.getName() == "Collision mesh") {
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
                        logErr("Object layer contains incompatible type. Ln 124, Tilemap.h");
                        return {};
                    }
                }
            }
            else if (layer.getType() == tson::LayerType::ObjectGroup && layer.getName() == "Event colliders") {
                std::vector<tson::Object> objects = layer.getObjects();

                 for (const auto& object : layer.getObjects()) {
                     const tson::Vector2i pos = object.getPosition();
                     const tson::Vector2i size = object.getSize();


                     if (object.getName() == "MurderBox") {
                         static uint8_t murderBoxCnt{};

                         mapData.eventColliders.emplace(
                             "MurderBox" + std::to_string(murderBoxCnt),
                             EventCollider(
                                static_cast<float>(pos.x),
                                static_cast<float>(pos.y),
                                static_cast<float>(size.x),
                                static_cast<float>(size.y),
                                "MurderBox" + std::to_string(murderBoxCnt),
                                world));

                         murderBoxCnt++;
                     }
                     else if (object.getName() == "Checkpoint") {
                         static uint8_t checkpointCnt{};

                         mapData.eventColliders.emplace(
                             "Checkpoint" + std::to_string(checkpointCnt),
                             EventCollider(
                                static_cast<float>(pos.x),
                                static_cast<float>(pos.y),
                                static_cast<float>(size.x),
                                static_cast<float>(size.y),
                                "Checkpoint" + std::to_string(checkpointCnt),
                                world));

                         checkpointCnt++;
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
                        logErr("Texture missing. Ln 174, Tilemap.h");
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
        data->tsonMapPtr = std::move(map);
        mapData.renderDataPtr = data;
    }
    catch (std::bad_alloc) {
        logErr("Constructor init failed, std::bad_alloc thrown Ln 198, Tilemap.h");
    }

    TraceLog(LOG_INFO, "Map loaded successfully.");

    return mapData;
}

inline void unloadMap(const MapData& map) {
    for (const auto& [name, texture] : map.renderDataPtr->textures) {
        UnloadTexture(texture);
    }

    TraceLog(LOG_INFO, "Map unloaded successfully.");
}

template<typename T>
void disableEventCollider(const MapData& map, const T& id) {
    const auto it = map.eventColliders.find(id);
    if (it != map.eventColliders.end()) {
        it->second.disableCollider();
    }
}

#endif //TILEMAP_H