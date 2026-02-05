//
// Author: DirgeWuff
// Created on: 5/15/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Contains basic data structures and functions used to load and store
// a tiled map for use by the renderer and physics engine. Loosely based
// off of raylib-tileson written by Rob Loach: https://github.com/RobLoach/raylib-tileson

#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <map>
#include <filesystem>
#include <cstdint>
#include "raylib.h"
#include "../Event/EventCollider.h"
#include "../external_libs/Tson/tileson.hpp"
#include "../Phys/CollisionSpline.h"
#include "../Utility/Logging.h"
#include "../Utility/Utils.h"

namespace fs = std::filesystem;

namespace RE::Core {
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
        std::vector<EventCollider> eventColliders;
        fs::path baseDir;
        fs::path fullMapPath;
        fs::path bgNoisePath;
        std::vector<CollisionSpline> collisionObjects;
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

    void unloadMap(const MapData& map);
    void disableEventCollider(const MapData& map, const guid& colliderGuid);

    // If I end up adding more types, might be good to RTTI this whole thing...
    template<typename T>
    float getMapPropFloat(const std::unique_ptr<tson::Map>& map, T& propName) {
        tson::PropertyCollection props = map->getProperties();

        if (!props.hasProperty(std::string(propName))) {
            logFatal(std::string("Map has no property \"" + std::string(propName) +
                std::string("\". Tilemap::GetMapPropFloat(Args...)")));
            return {};
        }

        return props.getValue<float>(propName);
    }

    template<typename T>
    std::string getMapPropStr(const std::unique_ptr<tson::Map>& map, T& propName) {
        tson::PropertyCollection props = map->getProperties();

        if (!props.hasProperty(std::string(propName))) {
            logFatal(std::string("Map has no property \"" + std::string(propName) +
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
            logFatal(std::string("Invalid filepath to map: ") + filepath + std::string("loadMap(Args...)"));
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
                logFatal(
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
                    logFatal("Incompatible layer type: Group Layer: loadMap(Args...)");
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
        catch (const std::exception& e) {
            logFatal(std::string("loadMap(Args...) failed: ") + std::string(e.what()));
            return {};
        }
        catch (...) {
            logFatal("loadMap(Args...) failed, an unknown error has occurred");
            return {};
        }

        #ifdef DEBUG
            logDbg("Tilemap loaded successfully.");
        #endif

        return mapData;
    }
}

#endif //TILEMAP_H