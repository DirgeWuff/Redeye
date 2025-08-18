//
// Created by DirgeWuff on 5/15/25.
//

#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <map>
#include "raylib.h"
#include "../external_libs/Tson/tileson.hpp"
#include "CollisionObject.h"
#include "EventCollider.h"

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
    int tileWidth;
    int tileHeight;
    int mapWidth;
    int mapHeight;

    fs::path baseDir;
    std::shared_ptr<RenderData> renderDataPtr;
    std::vector<CollisionObject> collisionObjects;
    std::vector<EventCollider> eventColliders;
};

class TiledMap {
    MapData mapData;                                    // MapData for this instance TiledMap
public:
    TiledMap();
    TiledMap(std::string&& filepath, b2WorldId world);
    ~TiledMap();

    // Draw an already loaded TiledMap
    void draw(
    const SceneCamera& cam,
    Vector2 offset,
    Color color
    ) const;

    [[nodiscard]] float getMapWidth() const noexcept;                                           // Get the TiledMap's width in number of tiles
    [[nodiscard]] float getMapHeight() const noexcept;                                          // Get the TieldMap's height in number of tiles
    [[nodiscard]] float getTileWidth() const noexcept;                                          // Get the width of TiledMap's tiles in pixels
    [[nodiscard]] float getTileHeight() const noexcept;                                         // Get the height of the TiledMap's tiles in pixels
    [[nodiscard]] const std::vector<CollisionObject>& getCollisionShapes() const noexcept;      // Return a reference to the TiledMap's CollisionObjects
};

#endif //TILEMAP_H