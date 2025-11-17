//
// Created by DirgeWuff on 6/27/25.
//

// Note: A bunch of these functions have a bit of visual 'jitter' to them due to the
// loss of precision from doing a static_cast from float to int.

#define RAYGUI_IMPLEMENTATION

#include <cassert>
#include <iostream>
#include <cstdint>
#include "box2d/box2d.h"
#include "raylib.h"
#include "Tson/tileson.hpp"
#include "Raygui/raygui.h"
#include "Debug.h"
#include "Utils.h"
#include "../Entity/Player.h"
#include "../../Application/Layers/GameLayer.h"
#include "../Utility/Globals.h"

// TODO: Create functions to disable camera lock and move camera on demand
void drawDebugBodyShapes(const Player& player) {
    if (!g_drawPlayerShapes) return;

    assert(b2Body_GetShapeCount(player.getBodyID()) != 0);

    const uint8_t maxShapes = b2Body_GetShapeCount(player.getBodyID());

    b2ShapeId shapes[maxShapes];
    b2Body_GetShapes(player.getBodyID(), shapes, maxShapes);

    b2Transform tf;
    tf.q = b2Body_GetRotation(player.getBodyID());
    tf.p = b2Body_GetPosition(player.getBodyID());

    // Probably really slow, but fuck it, this is a debug function
    for (const auto& shape : shapes) {
        const b2ShapeType st = b2Shape_GetType(shape); // Leaving this outside switch for debugging
        switch (st) {
            case b2_polygonShape: {
                const b2Polygon polygon = b2Shape_GetPolygon(shape);
                const std::size_t numVerts = polygon.count;

                b2Vec2 tfdVerts[numVerts];
                for (std::size_t i = 0; i < numVerts; i++) {
                    tfdVerts[i] = b2TransformPoint(tf, polygon.vertices[i]);
                }

                for (std::size_t i = 0; i < numVerts; i++) {
                    if (i + 1 == numVerts) {
                        DrawLineEx(
                     metersToPixelsVec(tfdVerts[i]),
                      metersToPixelsVec(tfdVerts[0]),
                     1.0f,
                     g_debugBodyColor);
                    }
                    else {
                        DrawLineEx(
                         metersToPixelsVec(tfdVerts[i]),
                          metersToPixelsVec(tfdVerts[i + 1]),
                          1.0f,
                          g_debugBodyColor);
                    }
                }

                break;
            }
            case b2_capsuleShape: {
                const auto [center1, center2, radius] = b2Shape_GetCapsule(shape);

                const b2Vec2 tfedP1 = b2TransformPoint(tf, center1);
                const b2Vec2 tfedP2 = b2TransformPoint(tf, center2);
                const float rad = radius;

                const b2Vec2 axis = tfedP2 - tfedP1;
                const auto [x, y] = b2Normalize(axis);
                const b2Vec2 normals = {-y, x};

                const b2Vec2 quads[4] = {
                    tfedP1 + rad * normals,
                    tfedP2 + rad * normals,
                    tfedP2 - rad * normals,
                    tfedP1 - rad * normals
                };

                DrawLineEx(metersToPixelsVec(quads[0]), metersToPixelsVec(quads[1]), 0.5f, g_debugBodyColor);
                DrawLineEx(metersToPixelsVec(quads[1]), metersToPixelsVec(quads[2]), 0.5f, g_debugBodyColor);
                DrawLineEx(metersToPixelsVec(quads[2]), metersToPixelsVec(quads[3]), 0.5f, g_debugBodyColor);
                DrawLineEx(metersToPixelsVec(quads[3]), metersToPixelsVec(quads[0]), 0.5f, g_debugBodyColor);

                const float capRadius = atan2f(y, x) * RAD2DEG;

                DrawCircleSectorLines(
                    metersToPixelsVec(tfedP2),
                     metersToPixels(rad),
                    capRadius - 90.0f,
                    capRadius + 90.0f,
                    20, g_debugBodyColor);
                DrawCircleSectorLines(
                    metersToPixelsVec(tfedP1),
                    metersToPixels(rad),
                    capRadius + 90.0f,
                    capRadius - 90.0f + 360.0f,
                    20,
                    g_debugBodyColor);
                break;
            }
            default: {
                std::cout << "Unsupported shape type." << std::endl;
                break;
            }
        }
    }
}

void drawDebugBodyCenter(const Player& player) {
    if (!g_drawPlayerCenter) return;

    const auto [x, y] = b2Body_GetPosition(player.getBodyID());

    DrawCircleLines(
        static_cast<int>(metersToPixels(x)),
        static_cast<int>(metersToPixels(y)),
        5.0f,
        g_debugBodyColor
    );
}

void drawDebugCameraCrosshair(const SceneCamera& camera) {
    if (!g_drawCameraCrosshair) return;

    const Rectangle cameraRect = camera.getCameraRect();

    DrawLine(
        static_cast<int>(cameraRect.x) + static_cast<int>(cameraRect.width) / 2,
        static_cast<int>(cameraRect.y),
        static_cast<int>(cameraRect.x) + static_cast<int>(cameraRect.width) / 2,
        static_cast<int>(cameraRect.y) + static_cast<int>(cameraRect.height),
        RED);

    DrawLine(
        static_cast<int>(cameraRect.x),
        static_cast<int>(cameraRect.y) + static_cast<int>(cameraRect.height) / 2,
        static_cast<int>(cameraRect.x) + static_cast<int>(cameraRect.width),
        static_cast<int>(cameraRect.y) + static_cast<int>(cameraRect.height) / 2,
        RED);
}

void drawDebugCameraRect(const SceneCamera& camera) {
    if (!g_drawCameraRect) return;

    const Rectangle rect = camera.getCameraRect();

    // Add offset of 2px/edge so we can see the rectangle around the edges of camera
    DrawRectangleLines(
            static_cast<int>(rect.x) + 2,
            static_cast<int>(rect.y) + 2,
            static_cast<int>(rect.width) - 4,
            static_cast<int>(rect.height) - 4,
            RED);
}

void drawDebugPlayerPosition(const Player& player) {
    if (!g_drawPlayerPos) return;

    DrawText(
        TextFormat("Position X: %f", player.getPositionCornerPx().x),
        10,
        10,
        20,
        RED);
    DrawText(
        TextFormat("Position Y: %f", player.getPositionCornerPx().y),
        10,
        30,
        20,
        RED);
}

void drawDebugFootpawSensorStatus(const Player& player) {
    if (!g_drawPlayerSensorStatus) return;

    if (player.isOnGround()) {
        DrawText(
            "Footpaw sensor in contact with object",
            10,
            g_drawPlayerPos ? 50 : 10,
            20,
            RED);
    }
    else {
        DrawText(
            "Footpaw sensor not in contact with object",
            10,
            g_drawPlayerPos ? 50 : 10,
            20,
            RED);
    }
}

void drawDebugCollisionShapes(const MapData& map) {
    if (!g_drawTerrainShapes) return;

    const std::vector<CollisionObject>& shapes = map.collisionObjects;

    for (const auto& shape : shapes) {
        const b2Vec2* points = shape.getObjectVerts();
        const std::size_t numVerts = shape.getVertCount();

        std::vector<Vector2> tfedVerts;

        for (std::size_t i = 0; i < numVerts; i++) {
            tfedVerts.push_back(metersToPixelsVec(points[i]));
        }

        for (std::size_t i = 0; i < numVerts - 1; i++) {
            DrawLineEx(
                tfedVerts[i],
                tfedVerts[i + 1],
                1.0f,
                g_debugCollisionColor);
        }
    }
}

// TODO: Improve speed on this, will tank framerate when used
void drawDebugCollisionVerts(const MapData& map) {
    if (!g_drawTerrainVerts) return;

    const std::vector<CollisionObject>& shapes = map.collisionObjects;

    for (const auto& shape : shapes) {
        const b2Vec2* points = shape.getObjectVerts();

        for (std::size_t i = 0; i < shape.getVertCount(); i++) {
            Vector2 v = metersToPixelsVec(points[i]);

            DrawCircleV(
                v,
                1.0f,
                g_debugVertColor);
        }
    }
}

void drawDebugEventColliders(const MapData& map) {
    if (!g_drawEventColliders) return;

    const std::unordered_map<std::string, EventCollider>& colliders = map.eventColliders;

    for (const auto& [id, collider] : colliders) {
        const Vector2 pos = collider.getPosPixels();
        const Vector2 size = collider.getSizePx();

        DrawRectangleLinesEx(
            {pos.x, pos.y, size.x, size.y},
            1.0f,
            g_debugColliderColor);

        const Vector2 idLen = MeasureTextEx(g_debugFont, id.c_str(), 6.0f, 0.50f);
        const Vector2 textPos = {
            pos.x + idLen.x / 2.0f,
            pos.y - 6.0f};

        // Still draws default font and I've spent way too much time trying to figure out why...
        DrawTextEx(g_debugFont, id.c_str(), textPos, 6.0f, 0.50f, g_debugColliderColor);
    }
}

void drawControlsWindow() {
        g_debugWindowBoxActive = IsKeyDown(KEY_M);

    if (g_debugWindowBoxActive) {

        g_debugWindowBoxActive = !GuiWindowBox(Rectangle{ 8, 496, 240, 280 }, "Debug drawing controls");

        GuiCheckBox(Rectangle{ 16, 528, 12, 12 }, "Draw player shapes", &g_drawPlayerShapes);
        GuiCheckBox(Rectangle{ 16, 552, 12, 12 }, "Draw player sensor status", &g_drawPlayerSensorStatus);
        GuiCheckBox(Rectangle{ 16, 576, 12, 12 }, "Draw player position", &g_drawPlayerPos);
        GuiCheckBox(Rectangle{16, 600, 12, 12}, "Draw player body center", &g_drawPlayerCenter);
        GuiCheckBox(Rectangle{ 16, 624, 12, 12 }, "Draw terrain shapes", &g_drawTerrainShapes);
        GuiCheckBox(Rectangle{ 16, 648, 12, 12 }, "Draw terrain vertices", &g_drawTerrainVerts);
        GuiCheckBox(Rectangle{ 16, 672, 12, 12 }, "Draw camera center crosshair", &g_drawCameraCrosshair);
        GuiCheckBox(Rectangle{ 16, 696, 12, 12}, "Draw camera edge rectangle", &g_drawCameraRect);
        GuiCheckBox(Rectangle{16, 720, 12,12}, "Draw event colliders", &g_drawEventColliders);
    }
}