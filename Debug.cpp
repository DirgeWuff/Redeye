//
// Created by DirgeWuff on 6/27/25.
//

// Note: A bunch of these functions have a bit of jitter to them due to the
// loss of precision in doing a static_cast to int.

#include "Debug.h"
#include "Utils.h"
#include "box2d/box2d.h"
#include "raylib.h"
#include <cassert>
#include <iostream>

#include "Error.h"

constexpr Color DEBUG_COLOR{0, 0, 255, 255};

void drawDebugBodyShapes(const Entity* targetEntity) {
    assert(b2Body_GetShapeCount(targetEntity->getBodyID()) != 0 && "Assertion failed. Body contains no shapes.");

    const int maxShapes = b2Body_GetShapeCount(targetEntity->getBodyID());

    b2ShapeId shapes[maxShapes];
    b2Body_GetShapes(targetEntity->getBodyID(), shapes, maxShapes);

    b2Transform tf;
    tf.q = b2Body_GetRotation(targetEntity->getBodyID());
    tf.p = b2Body_GetPosition(targetEntity->getBodyID());

    // Probably really slow, but fuck it, this is a debug function
    for (const auto& shape : shapes) {
        const b2ShapeType st = b2Shape_GetType(shape); // Leaving this outside switch for debugging
        switch (st) {
            case b2_polygonShape: {
                const b2Polygon polygon = b2Shape_GetPolygon(shape);
                const int numVerts = polygon.count;

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
                     DEBUG_COLOR);
                    }
                    else {
                        DrawLineEx(
                         metersToPixelsVec(tfdVerts[i]),
                          metersToPixelsVec(tfdVerts[i + 1]),
                          1.0f,
                          DEBUG_COLOR);
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

                DrawLineEx(metersToPixelsVec(quads[0]), metersToPixelsVec(quads[1]), 0.5f, DEBUG_COLOR);
                DrawLineEx(metersToPixelsVec(quads[1]), metersToPixelsVec(quads[2]), 0.5f, DEBUG_COLOR);
                DrawLineEx(metersToPixelsVec(quads[2]), metersToPixelsVec(quads[3]), 0.5f, DEBUG_COLOR);
                DrawLineEx(metersToPixelsVec(quads[3]), metersToPixelsVec(quads[0]), 0.5f, DEBUG_COLOR);

                const float capRadius = atan2f(y, x) * RAD2DEG;

                DrawCircleSectorLines(
                    metersToPixelsVec(tfedP2),
                     metersToPixels(rad),
                    capRadius - 90.0f,
                    capRadius + 90.0f,
                    20, DEBUG_COLOR);
                DrawCircleSectorLines(
                    metersToPixelsVec(tfedP1),
                    metersToPixels(rad),
                    capRadius + 90.0f,
                    capRadius - 90.0f + 360.0f,
                    20,
                    DEBUG_COLOR);
                break;
            }
            default: {
                std::cout << "Unsupported shape type." << std::endl;
                break;
            }
        }
    }
}

void drawDebugBodyCenter(const Entity* targetEntity) {
    const auto [x, y] = b2Body_GetPosition(targetEntity->getBodyID());

    DrawCircleLines(
        static_cast<int>(metersToPixels(x)),
        static_cast<int>(metersToPixels(y)),
        5.0f,
        DEBUG_COLOR
    );
}

void drawDebugCameraCrosshair(const std::unique_ptr<SceneCamera>& camera) {
    if (camera == nullptr) {
        logErr("camera == nullptr. Ln 123, Debug.cpp");
        return;
    }

    const Rectangle cameraRect = camera->getCameraRect();

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

void drawDebugCameraRect(const std::unique_ptr<SceneCamera>& camera) {
    if (camera == nullptr) {
        logErr("camera == nullptr. Ln 146, Debug.cpp");
        return;
    }
    const Rectangle rect = camera->getCameraRect();

    // Add offset of 2px/edge so we can see the rectangle around the edges of camera
    DrawRectangleLines(
            static_cast<int>(rect.x) + 2,
            static_cast<int>(rect.y) + 2,
            static_cast<int>(rect.width) - 4,
            static_cast<int>(rect.height) - 4,
            RED);
}

void drawDebugPlayerPosition(
    const std::shared_ptr<Player>& player,
    const std::unique_ptr<SceneCamera>& camera) {
    if (player == nullptr || camera == nullptr) {
        logErr("camera || player == nullptr. Ln 164, Debug.cpp");
        return;
    }

    Vector2 transPosOne = GetScreenToWorld2D({10.0f, 10.0f}, *camera->getCameraPtr());
    Vector2 transPosTwo = GetScreenToWorld2D({10.0f, 40.0f}, *camera->getCameraPtr());

    DrawText(
        TextFormat("Position X: %f", player->getPositionCornerPx().x),
        static_cast<int>(transPosOne.x),
        static_cast<int>(transPosOne.y),
        20,
        RED);
    DrawText(
        TextFormat("Position Y: %f", player->getPositionCornerPx().y),
        static_cast<int>(transPosTwo.x),
        static_cast<int>(transPosTwo.y),
        20,
        RED);
}

void drawDebugFootpawSensorStatus(
    const std::shared_ptr<Player> &player,
    const std::unique_ptr<SceneCamera> &camera) {
    if (player == nullptr || camera == nullptr) {
        logErr("camera || player == nullptr. Ln 189, Debug.cpp");
        return;
    }

    Vector2 transPos = GetScreenToWorld2D({10.0f, 70.0f}, *camera->getCameraPtr());

    if (player->getFootpawSensorStatus() == true) {
        DrawText(
            "Footpaw sensor in contact with object",
            static_cast<int>(transPos.x),
            static_cast<int>(transPos.y),
            20,
            RED);
    }
    else {
        DrawText(
            "Footpaw sensor not in contact with object",
            static_cast<int>(transPos.x),
            static_cast<int>(transPos.y),
            20,
            RED);
    }
}
