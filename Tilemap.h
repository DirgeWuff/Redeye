//
// Created by DirgeWuff on 5/15/25.
//

#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <map>
#include "external_libs/Tson/tileson.hpp"
#include "external_libs/Raylib/include/raylib.h"
#include "CollisionObject.h"
#include "EventCollider.h"

class SceneCamera;

// Just for extra clarity

struct TileData {
    Vector2 position;
    Rectangle sourceRect;
    const Texture2D* texture;
};

struct RenderData {
    std::map<std::string, Texture> textures;
    std::unordered_map<const tson::Tileset*, Texture2D*> texturePtrs;
    std::unordered_map<const tson::Layer*, std::vector<TileData>> layerRenderData;
    std::shared_ptr<tson::Map> tsonMapPtr;
};

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
    MapData mapData;
public:
    TiledMap();
    TiledMap(std::string&& filepath, b2WorldId world);
    ~TiledMap();

    void draw(
    const SceneCamera& cam,
    Vector2 offset,
    Color color
    ) const;

    float getMapWidth() const;
    float getMapHeight() const;
    float getTileWidth() const;
    float getTileHeight() const;
    const std::vector<CollisionObject>& getCollisionShapes() const;
};

#endif //TILEMAP_H