//
// Created by DirgeWuff on 5/15/25.
//

#ifndef TILEMAP_H
#define TILEMAP_H
#include "tileson.hpp"
#include "raylib.h"
#include "Camera.h"
#include <map>

class SceneCamera; // Lazy forward declaration to fix circular dependency

// Change this to a struct at some point down the line
class TilesonData {
public:
    std::map<std::string, Texture> textures;
    std::shared_ptr<tson::Map> map;
};

struct MapData {
    int tileWidth;
    int tileHeight;
    int mapWidth;
    int mapHeight;
    fs::path baseDir;
    std::shared_ptr<TilesonData> data;
};

class TiledMap {
    MapData mapData;
    Rectangle toRayRect(tson::Rect rect);
    Vector2 toRayVec2(tson::Vector2f vec);

    void loadLayer(
    const std::shared_ptr<TilesonData>& data,
    const std::string& baseImageDir,
    const std::string& imageName);

    void drawTiledLayer(
    tson::Layer& layer,
    const SceneCamera& cam,
    float offsetX,
    float offsetY,
    Color color);

    void drawImageLayer(
    const tson::Layer& layer,
    float offsetX,
    float offsetY,
    Color color
    ) const;

    void drawAnyLayer(
    tson::Layer& layer,
    const SceneCamera& cam,
    float offsetX,
    float offsetY,
    Color color
);
public:
    TiledMap();
    explicit TiledMap(std::string&& filepath);
    ~TiledMap();

    void drawMap(
    SceneCamera& cam,
    Vector2 offset,
    Color color
    );

    float getMapWidth() const;
    float getMapHeight() const;
    float getTileWidth() const;
    float getTileHeight() const;
};

#endif //TILEMAP_H
