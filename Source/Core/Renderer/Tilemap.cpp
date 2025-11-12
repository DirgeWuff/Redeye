//
// Created by DirgeWuff on 11/11/2025.
//

#include "Tilemap.h"
#include "../../Core/Event/EventCollider.h"

class EventCollider;

void loadTilesets(
    const MapData& mapData,
    const std::unique_ptr<tson::Map>& map,
    const std::shared_ptr<RenderData>& renderData)
{
    std::string imagePath;

    for (const auto& tileset : map->getTilesets()) {
        if (!mapData.baseDir.empty()) {
            imagePath = mapData.baseDir.string() + tileset.getImage().string();
        }
        else {
            imagePath = tileset.getImage().string();
        }

        if (!renderData->textures.contains(imagePath)) {
            const Texture2D texture = LoadTexture(imagePath.c_str());
            renderData->textures[imagePath] = texture;
        }

        renderData->texturePtrs[&tileset] = &renderData->textures[imagePath];
    }
}

void loadImageLayer(
    const MapData& mapData,
    const tson::Layer& layer,
    const std::shared_ptr<RenderData>& renderData)
{
    std::string imagePath;

    if (!mapData.baseDir.empty()) {
        imagePath = mapData.baseDir.string() + layer.getImage();
    }
    else {
        imagePath = layer.getImage();
    }

    const Texture texture = LoadTexture(imagePath.c_str());
    renderData->textures[imagePath] = texture;
}

void loadCollisionMesh(
    MapData& mapData,
    tson::Layer& layer,
    b2WorldId world)
{
    for (auto& object : layer.getObjects()) {
        const tson::ObjectType objType = object.getObjectType();

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
            return;
        }
    }
}

void loadEventColliders(
    MapData& mapData,
    tson::Layer& layer,
    b2WorldId world)
{
    std::vector<tson::Object> objects = layer.getObjects();

     for (const auto& object : layer.getObjects()) {
         const tson::Vector2i pos = object.getPosition();
         const tson::Vector2i size = object.getSize();

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
             return;
         }
     }
}

void loadTileLayer(
    tson::Layer& layer,
    const std::shared_ptr<RenderData>& renderData)
{
    std::vector<TileData> layerData;

    for (auto& tile : std::views::values(layer.getTileObjects())) {
        const tson::Tile* tilePtr = tile.getTile();
        const tson::Tileset* tileset = tilePtr->getTileset();

        auto it = renderData->texturePtrs.find(tileset);
        if (it == renderData->texturePtrs.end()) {
            logErr("Texture missing: loadMap(Args...)");
            break;
        }

        TileData tileData;
        tileData.position = toRayVec2(tile.getPosition());
        tileData.sourceRect = toRayRect(tile.getDrawingRect());
        tileData.texture = it->second;

        layerData.push_back(tileData);
    }

    renderData->layerRenderData[&layer] = std::move(layerData);
}