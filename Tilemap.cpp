//
// Created by DirgeWuff on 5/15/25.
//

// The following is a modified version of Rob Loach's raylib-tileson implementation,
// with class encapsulation, improvements to memory safety, const correctness, error handling,
// and the addition of calculations for parallax.
// Link to the original: https://github.com/RobLoach/raylib-tileson.
// Used and modified under the permissions granted by the BSD license.


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TO DO:
// Add error handling
// Change name of 'offset' arg to reflect the use of parallax

#include "Tilemap.h"
#include "Error.h"
#include "raymath.h"

TiledMap::TiledMap() {
    mapData.mapWidth = NULL;
    mapData.mapHeight = NULL;
    mapData.tileWidth = NULL;
    mapData.tileHeight = NULL;
    mapData.data = nullptr;
}

TiledMap::TiledMap(std::string&& filepath) {
    mapData.baseDir = fs::relative(filepath);
    if (mapData.baseDir.has_extension()) {
        mapData.baseDir.remove_filename();
    }

    tson::Tileson t;
    try {
        std::unique_ptr<tson::Map> map = t.parse(fs::path(filepath));

        if (map->getStatus() != tson::ParseStatus::OK) {
            logErr(
                "Constructor init failed: TiledMap::TiledMap. Unable to parse map: " + map->getStatusMessage());
            return;
        }

        const std::shared_ptr<TilesonData> data = std::make_shared<TilesonData>();

        for (const auto& layer : map->getLayers()) {
            if (layer.getType() == tson::LayerType::ImageLayer) {
                loadLayer(data, mapData.baseDir, layer.getImage());
            }
        }
        for (const auto& image : map->getTilesets()) {
            loadLayer(data, mapData.baseDir, image.getImage().string());
        }

        mapData.mapWidth = map->getSize().x;
        mapData.mapHeight = map->getSize().y;
        mapData.tileWidth = map->getTileSize().x;
        mapData.tileHeight = map->getTileSize().y;
        data->map = std::move(map);
        mapData.data = data;
    }
    catch (std::bad_alloc) {
        logErr("Constructor init failed: TiledMap::TiledMap. std::bad_alloc thrown.");
    }

    TraceLog(LOG_INFO, "Tiled map loaded successfully.");
}

// NOTE: Seems to throw OpenGL error on texture unload, possibly related to Apple Silicon chip
TiledMap::~TiledMap() {
    for (const auto& [name, texture] : mapData.data->textures) {
        UnloadTexture(texture);
    }

    TraceLog(LOG_INFO, "Tiled map unloaded successfully.");
}

Rectangle TiledMap::toRayRect(const tson::Rect rect) {
    Rectangle output;

    output.x = rect.x;
    output.y = rect.y;
    output.width = rect.width;
    output.height = rect.height;

    return output;
}

Vector2 TiledMap::toRayVec2(const tson::Vector2f vec) {
    Vector2 output;

    output.x = vec.x;
    output.y = vec.y;

    return output;
}

void TiledMap::loadLayer(
    std::shared_ptr<TilesonData> data,
    const std::string& baseImageDir,
    const std::string& imageName) {

    const char* imagePath;

    if (baseImageDir.length() > 0) {
        imagePath = TextFormat("%s%s", baseImageDir.c_str(), imageName.c_str());
    }
    else {
        imagePath = imageName.c_str();
    }

    const Texture texture = LoadTexture(imagePath);
    data->textures[imagePath] = texture;
}

void TiledMap::drawTiledLayer(
    tson::Layer& layer,
    const SceneCamera& cam,
    const float offsetX,
    const float offsetY,
    const Color color) {

    for (auto& [pos, tile] : layer.getTileObjects()) {
        const tson::Tileset* tileset = tile.getTile()->getTileset();

        const Vector2 position = toRayVec2(tile.getPosition());
        const Rectangle drawingRect = toRayRect(tile.getDrawingRect());

        const Rectangle tileBounds = {
            position.x + offsetX,
            position.y + offsetY,
            drawingRect.width,
            drawingRect.height
        };

        // Camera cull
        if (!CheckCollisionRecs(tileBounds, cam.getCameraRect())) {
            continue;
        }

        std::string imagePath = mapData.baseDir.string() + tileset->getImage().string();

        if (!mapData.data->textures.contains(imagePath)) {
            break;
        }

        DrawTextureRec(
            mapData.data->textures[imagePath],
            drawingRect,
            {position.x + offsetX, position.y + offsetY},
            color);
    }
}

void TiledMap::drawImageLayer(
    const tson::Layer& layer,
    const float offsetX,
    const float offsetY,
    const Color color) {

    const std::string imagePath = mapData.baseDir.string() + layer.getImage();
    const tson::Vector2f offset = layer.getOffset();

    if (!mapData.data->textures.contains(imagePath)) {
        logErr("Unable to load texture: " + layer.getImage() + ". Ln 167, TileMap.cpp.");
        return;
    }

    DrawTexture(mapData.data->textures[imagePath], offset.x - offsetX, offset.y - offsetY, color);
}

void TiledMap::drawAnyLayer(
    tson::Layer& layer,
    const SceneCamera& cam,
    const float offsetX,
    const float offsetY,
    const Color color) {

    switch (layer.getType()) {
        case tson::LayerType::TileLayer:
            drawTiledLayer(layer, cam, offsetX, offsetY, color);
            break;

        case tson::LayerType::ImageLayer:
            drawImageLayer(layer, offsetX, offsetY, color);
            break;

        default:
            logErr("Map contains unsupported layer type.");
    }
}

void TiledMap::drawMap(
    SceneCamera& cam,
    Vector2 offset,
    const Color color) {
    for (std::vector<tson::Layer>& layers = mapData.data->map->getLayers(); auto& layer : layers) {

        const Vector2 parallaxFactor = toRayVec2(layer.getParallax());
        const Vector2 layerOffset = toRayVec2(layer.getOffset());
        const Vector2 parallaxOrigin = toRayVec2(layer.getMap()->getParallaxOrigin());
        const Vector2 cameraOffset = GetWorldToScreen2D(parallaxOrigin, *cam.getCameraPtr());

        // Unsure why / 10 was required to get the correct value??? Never seen this in any sample code.
        Vector2 adjustedOffset = offset + layerOffset - (cameraOffset * (Vector2{1.0, 1.0} - parallaxFactor) / 10);
        drawAnyLayer(layer, cam, adjustedOffset.x, adjustedOffset.y, color);
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
