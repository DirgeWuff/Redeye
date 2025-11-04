//
// Created by DirgeWuff on 5/15/25.
//

#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <map>
#include <filesystem>
#include "ranges"
#include "raylib.h"
#include "../external_libs/Tson/tileson.hpp"
#include "../Phys/CollisionObject.h"
#include "../Utility/Error.h"
#include "../Utility/Utils.h"

// TODO: Consider breaking up loadMap() into functions so it's more maintainable...

namespace fs = std::filesystem;

class SceneCamera;
class EventCollider;

// Struct containing relevant data about a given tile. Used for fast drawing.
struct TileData {
    Rectangle sourceRect;
    Vector2 position;
    const Texture2D* texture;
};

// Structured data used to render a map.
struct RenderData {
    std::unordered_map<const tson::Tileset*, Texture2D*> texturePtrs;
    std::unordered_map<const tson::Layer*, std::vector<TileData>> layerRenderData;
    std::map<std::string, Texture> textures;
    std::shared_ptr<tson::Map> tsonMapPtr;
};

// Structured data used to load a map. Used on a per-map basis.
struct MapData {
    std::unordered_map<std::string, EventCollider> eventColliders;
    fs::path baseDir;
    fs::path fullMapPath;
    fs::path bgNoisePath;
    std::vector<CollisionObject> collisionObjects;
    std::shared_ptr<RenderData> renderDataPtr;

    // Leaving this as a vec2 so I don't need to set player POS in meters in Tiled
    Vector2 playerStartPos;

    uint8_t tileWidth;
    uint8_t tileHeight;
    uint16_t mapWidth;
    uint16_t mapHeight;
};

// If I end up adding more types, might be good to RTTI this whole thing...
template<typename T>
float getMapPropFloat(const std::unique_ptr<tson::Map>& map, T& propName) {
    tson::PropertyCollection props = map->getProperties();

    if (!props.hasProperty(std::string(propName))) {
        logErr(std::string("Map has no property \"" + std::string(propName) +
            std::string("\". Tilemap::GetMapPropFloat(Args...)")));
        return {};
    }

    return props.getValue<float>(propName);
}

template<typename T>
std::string getMapPropStr(const std::unique_ptr<tson::Map>& map, T& propName) {
    tson::PropertyCollection props = map->getProperties();

    if (!props.hasProperty(std::string(propName))) {
        logErr(std::string("Map has no property \"" + std::string(propName) +
            std::string("\". Tilemap::GetMapPropStr(Args...)")));
        return {};
    }

    return props.getValue<std::string>(propName);
}

template<typename T>
MapData loadMap(T&& filepath, b2WorldId world) {
    MapData mapData;

    // Validate the filepath.
    if (!exists(fs::path(filepath))) {
        logErr(std::string("Invalid filepath to map: ") + filepath + std::string("loadMap(Args...)"));
        return {};
    }

    mapData.baseDir = fs::relative(filepath);
    mapData.fullMapPath = fs::relative(filepath);
    if (mapData.baseDir.has_extension()) {
        mapData.baseDir.remove_filename();
    }

    try {
        // Load map file and validate it.
        tson::Tileson t;
        std::unique_ptr<tson::Map> map = t.parse(fs::path(filepath));

        if (map->getStatus() != tson::ParseStatus::OK) {
            logErr(
                "Constructor init failed: Unable to parse map: " + map->getStatusMessage() + ". loadMap(Args...)");
            return {};
        }

        // Load and cache ptrs to the images/textures
        const auto data = std::make_shared<RenderData>();

        for (const auto& tileset : map->getTilesets()) {
            std::string imagePath;

            if (!mapData.baseDir.empty()) {
                imagePath = mapData.baseDir.string() + tileset.getImage().string();
            }
            else {
                imagePath = tileset.getImage().string();
            }

            if (!data->textures.contains(imagePath)) {
                const Texture2D texture = LoadTexture(imagePath.c_str());
                // SetTextureFilter(texture, TEXTURE_FILTER_POINT);
                data->textures[imagePath] = texture;
            }

            data->texturePtrs[&tileset] = &data->textures[imagePath];
        }

        for (auto& layer : map->getLayers()) {
            // Parse image layers. Cheap to draw, so not bothering with ptrs
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
            // Parse collision mesh, create collision objects in Box2D
            else if (layer.getType() == tson::LayerType::ObjectGroup && layer.getName() == "Collision mesh") {
                for (auto& object : layer.getObjects()) {
                    tson::ObjectType objType = object.getObjectType();

                    if (objType == tson::ObjectType::Polyline) {
                        tson::Vector2i pos = object.getPosition();
                        std::vector<tson::Vector2i> tsonPoints = object.getPolylines();
                        std::vector<b2Vec2> points;

                        for (std::size_t i = 0; i < tsonPoints.size(); i++) {
                            tsonPoints[i] = v2iAdd(tsonPoints[i], pos);
                            points.push_back(pixelsToMetersVec(tsonPoints[i]));
                        }

                        mapData.collisionObjects.emplace_back(world, points);
                    }
                    else {
                        logErr("Collision layer contains incompatible type: loadMap(Args...)");
                        return {};
                    }
                }
            }
            // Parse collider layer and create colliders
            else if (layer.getType() == tson::LayerType::ObjectGroup && layer.getName() == "Event colliders") {
                std::vector<tson::Object> objects = layer.getObjects();

                 for (const auto& object : layer.getObjects()) {
                     const tson::Vector2i pos = object.getPosition();
                     const tson::Vector2i size = object.getSize();

                     // Perfect forward in place with std::piecewise_construct, std::forward_as_tuple
                     if (object.getName() == "MurderBox") {
                         // NOTE: Just sorta blindly assuming that there's never gonna be more than 255
                         // colliders of a given type here...
                         static uint8_t murderBoxCnt{};

                         mapData.eventColliders.emplace(
                             std::piecewise_construct,
                             std::forward_as_tuple("MurderBox" + std::to_string(murderBoxCnt)),
                             std::forward_as_tuple(
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
                             std::piecewise_construct,
                             std::forward_as_tuple("Checkpoint" + std::to_string(checkpointCnt)),
                             std::forward_as_tuple(
                                 static_cast<float>(pos.x),
                                 static_cast<float>(pos.y),
                                 static_cast<float>(size.x),
                                 static_cast<float>(size.y),
                                 "Checkpoint" + std::to_string(checkpointCnt),
                                 world));

                         checkpointCnt++;
                     }
                     else {
                         logErr(std::string("Incompatible object layer: " + object.getName() + "loadMap(Args...)"));
                         return {};
                     }
                 }
            }
            // Parse tile layers, precompute position and cache texture ptr for each tile
            else if (layer.getType() == tson::LayerType::TileLayer) {
                std::vector<TileData> renderData;

                for (auto& tile : std::views::values(layer.getTileObjects())) {
                    const tson::Tile* tilePtr = tile.getTile();
                    const tson::Tileset* tileset = tilePtr->getTileset();

                    auto iterator = data->texturePtrs.find(tileset);
                    if (iterator == data->texturePtrs.end()) {
                        logErr("Texture missing: loadMap(Args...)");
                        continue;
                    }

                    TileData tileData;
                    // Vector2 position = toRayVec2(tile.getPosition());
                    // position.x = roundf(position.x);
                    // position.y = roundf(position.y);
                    tileData.position = toRayVec2(tile.getPosition());
                    tileData.sourceRect = toRayRect(tile.getDrawingRect());
                    tileData.texture = iterator->second;

                    renderData.push_back(tileData);
                }

                data->layerRenderData[&layer] = std::move(renderData);
            }
            else {
                logErr("Incompatible layer type: Group Layer: loadMap(Args...)");
                return {};
            }
        }

        // Handle embedded map properties
        mapData.playerStartPos = {
            getMapPropFloat(map, "playerStartX"),
            getMapPropFloat(map, "playerStartY")};

        mapData.bgNoisePath = fs::path(getMapPropStr(map, "bgNoisePath"));

        // Assign everything else
        mapData.mapWidth = map->getSize().x;
        mapData.mapHeight = map->getSize().y;
        mapData.tileWidth = map->getTileSize().x;
        mapData.tileHeight = map->getTileSize().y;
        data->tsonMapPtr = std::move(map);
        mapData.renderDataPtr = data;
    }
    catch (std::bad_alloc) {
        logErr("loadMap(Args...) failed, std::bad_alloc thrown");
        return {};
    }
    catch (...) {
        logErr("loadMap(Args...) failed, an unknown error has occurred");
        return {};
    }

    TraceLog(LOG_INFO, "Map loaded successfully.");
    return mapData;
}

// Unload or destroy map.
inline void unloadMap(const MapData& map) {
    for (const auto& texture : std::views::values(map.renderDataPtr->textures)) {
        UnloadTexture(texture);
    }

    TraceLog(LOG_INFO, "Map unloaded successfully.");
}

// Disable an event collider within the map so player contact is no longer registered.
template<typename T>
void disableEventCollider(const MapData& map, const T& id) {
    const auto it = map.eventColliders.find(id);
    if (it != map.eventColliders.end()) {
        it->second.disableCollider();
    }
    else {
        logErr("No matching collider found in world.");
    }
}

#endif //TILEMAP_H