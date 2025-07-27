//
// Created by DirgeWuff on 5/15/25.
//

#ifndef TILEMAP_H
#define TILEMAP_H

#include "tileson.hpp"
#include "raylib.h"
#include "box2d/types.h"
#include "Camera.h"
#include <vector>
#include <map>
#include "CollisionObject.h"

class SceneCamera;

// Just for extra clarity
typedef std::vector<CollisionObject> collisionWorld_t;

struct TileData {
    Vector2 position;
    Rectangle sourceRect;
    const Texture2D* texture;
};

struct TilesonData {
    std::map<std::string, Texture> textures;
    std::unordered_map<const tson::Tileset*, Texture2D*> texturePtrs;
    std::unordered_map<const tson::Layer*, std::vector<TileData>> layerRenderData;
    std::shared_ptr<tson::Map> map;
};

struct MapData {
    int tileWidth;
    int tileHeight;
    int mapWidth;
    int mapHeight;

    fs::path baseDir;
    std::shared_ptr<TilesonData> data;
    collisionWorld_t collisionObjects;
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
    collisionWorld_t getCollisionShapes() const;
};

#endif //TILEMAP_H
