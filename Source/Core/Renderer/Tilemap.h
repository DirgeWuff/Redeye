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
    std::optional<std::string> ambientSoundsPath;
    fs::path baseDir;
    std::string bgNoisePath;
    std::vector<CollisionObject> collisionObjects;
    std::shared_ptr<RenderData> renderDataPtr;

    Vector2 playerStartPos;

    uint8_t tileWidth;
    uint8_t tileHeight;
    uint16_t mapWidth;
    uint16_t mapHeight;
};

template<typename T>
MapData loadMap(const T& filepath, b2WorldId world) {
    MapData mapData;

    // Validate the filepath.
    if (!exists(fs::path(filepath))) {
        logErr(std::string("Invalid filepath to map: ") + std::string(filepath) + std::string("loadMap(Args...)"));
        return {};
    }

    mapData.baseDir = fs::relative(filepath);
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
                const Texture texture = LoadTexture(imagePath.c_str());
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
                            points.push_back(toB2Vec2(tsonPoints[i]));
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

        // Handle embedded map properties, messy as fuck but it works
        tson::PropertyCollection embeddedData = map->getProperties();

        // Player spawn position
        // Maybe consider moving this to a new obj layer with point, this is rather clunky...
        if (embeddedData.hasProperty("playerStartX")) {
            mapData.playerStartPos.x = embeddedData.getValue<float>("playerStartX");
        }
        else {
            logErr("playerStartX not specified in map: loadMap(Args...)");
            return {};
        }

        if (embeddedData.hasProperty("playerStartY")) {
            mapData.playerStartPos.y = embeddedData.getValue<float>("playerStartY");
        }
        else {
            logErr("playerStartY not specified in map: loadMap(Args...)");
            return {};
        }

        // Background "music" or noise
        if (embeddedData.hasProperty("bgNoisePath")) {
            const auto p = embeddedData.getValue<std::string>("bgNoisePath");
            if (!exists(fs::path(p))) {
                logErr(std::string("Missing or corrupted sound data path: " + p + std::string(". loadMap(Args...)")));
            }

            mapData.bgNoisePath = p;
        }
        else {
            logErr("bgNoisePath not specified in map: loadMap(Args...)");
            return {};
        }

        // Optional ambient sounds to be played at random intervals
        // TODO: Add actual ambient noises to sound design so this actually does something
        if (embeddedData.hasProperty("ambientNoisesPath")) {
            const auto p = embeddedData.getValue<std::string>("ambientNoisesPath");
            if (!exists(fs::path(p))) {
                logErr(std::string("Missing or corrupted sound data path: " + p + std::string(" loadMap(Args...)")));
                return {};
            }

            mapData.ambientSoundsPath = p;
        }

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
}

#endif //TILEMAP_H