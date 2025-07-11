//
// Created by DirgeWuff on 6/27/25.
//


#ifndef UTILS_H
#define UTILS_H

#include <box2d/types.h>
#include <raylib.h>

bool isShapeIdEqual(const b2ShapeId& idOne, const b2ShapeId& idTwo);    // Check if 2 b2ShapeId's are equal

// NOTE: In this game, 100 pixels is defined as a meter
Vector2 metersToPixelsVec(const b2Vec2& vec);                           // Convert b2 meters to rl pixels in vector
b2Vec2 pixelsToMetersVec(const Vector2& vec);                           // Convert rl pixels to b2 meters in vector
float metersToPixels(float meters);                                     // Convert b2 meters to rl pixels
float pixelsToMeters(float pixels);                                     // Convert rl pixels to b2 meters

#endif //UTILS_H
