//
// Created by DirgeWuff on 5/15/25.
//

#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <map>
#include <filesystem>
#include <cstdint>
#include "ranges"
#include "raylib.h"
#include "../external_libs/Tson/tileson.hpp"
#include "../Phys/CollisionObject.h"
#include "../Utility/Error.h"
#include "../Utility/Utils.h"

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
};

// Structured data used to load a map. Used on a per-map basis.
struct MapData {
    std::unordered_map<std::string, EventCollider> eventColliders;
    fs::path baseDir;
    fs::path fullMapPath;
    fs::path bgNoisePath;
    std::vector<CollisionObject> collisionObjects;
    std::shared_ptr<RenderData> renderDataPtr;
    std::shared_ptr<tson::Map> tsonMapPtr;

    // Leaving this as a vec2 so I don't need to set player POS in meters in Tiled
    Vector2 playerStartPos;

    uint8_t tileWidth;
    uint8_t tileHeight;
    uint16_t mapWidth;
    uint16_t mapHeight;
};

void loadTilesets(
    const MapData& mapData,
    const std::unique_ptr<tson::Map>& map,
    const std::shared_ptr<RenderData>& renderData);

void loadImageLayer(
    const MapData& mapData,
    const tson::Layer& layer,
    const std::shared_ptr<RenderData>& renderData);

void loadCollisionMesh(
    MapData& mapData,
    tson::Layer& layer,
    b2WorldId world);

void loadEventColliders(
    MapData& mapData,
    tson::Layer& layer,
    b2WorldId world);

void loadTileLayer(
    tson::Layer& layer,
    const std::shared_ptr<RenderData>& renderData);


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
MapData loadMap(T&& filepath, const b2WorldId world) {
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
        tson::Tileson t;
        std::unique_ptr<tson::Map> map = t.parse(fs::path(filepath));

        if (map->getStatus() != tson::ParseStatus::OK) {
            logErr(
                "Constructor init failed: Unable to parse map: " + map->getStatusMessage() + ". loadMap(Args...)");
            return {};
        }

        const auto data = std::make_shared<RenderData>();
        loadTilesets(mapData, map, data);

        for (auto& layer : map->getLayers()) {
            if (layer.getType() == tson::LayerType::ImageLayer) {
                loadImageLayer(mapData, layer, data);
            }
            else if (layer.getType() == tson::LayerType::ObjectGroup && layer.getName() == "Collision mesh") {
                loadCollisionMesh(mapData, layer, world);
            }
            else if (layer.getType() == tson::LayerType::ObjectGroup && layer.getName() == "Event colliders") {
                loadEventColliders(mapData, layer, world);
            }
            else if (layer.getType() == tson::LayerType::TileLayer) {
                loadTileLayer(layer, data);
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
        mapData.tsonMapPtr = std::move(map);
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