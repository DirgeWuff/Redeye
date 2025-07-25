//
// Created by DirgeWuff on 6/27/25.
//

#ifndef B2UTILS_H
#define B2UTILS_H

#include "tileson.hpp"
#include <box2d/types.h>
#include <raylib.h>

bool isShapeIdEqual(const b2ShapeId& idOne, const b2ShapeId& idTwo);    // Check if 2 b2ShapeId's are equal

Vector2 metersToPixelsVec(const b2Vec2& vec);                           // Convert b2 meters to rlpixels in vector2
b2Vec2 pixelsToMetersVec(const Vector2& vec);                           // Convert rl pixels to b2 meters in vector2
float metersToPixels(float meters);                                     // Convert b2 meters to rl pixels
float pixelsToMeters(float pixels);                                     // Convert rl pixels to b2 meters

Vector2 toRayVec2(const tson::Vector2f& vec);                           // Convert tson::Vector2i to Vector2
b2Vec2 toB2Vec2(const tson::Vector2i& vec);                             // Convert tson::Vector2i to b2Vec2
Rectangle toRayRect(const tson::Rect& rect);                            // Convert tson::Rect to Rectangle


tson::Vector2i v2iAdd(
    const tson::Vector2i& vecOne,
    const tson::Vector2i& vecTwo);                                      // Perform addition with tson::Vector2i

#endif //B2UTILS_H
