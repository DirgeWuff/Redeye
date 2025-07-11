//
// Created by DirgeWuff on 6/27/25.
//

#include "Utils.h"

constexpr int PPM = 100; // 100 px/meter

bool isShapeIdEqual(const b2ShapeId &idOne, const b2ShapeId &idTwo) {
    return idOne.generation == idTwo.generation &&
           idOne.index1 == idTwo.index1 &&
           idOne.world0 == idTwo.world0;
}

Vector2 metersToPixelsVec(const b2Vec2 &vec) {
    return Vector2{vec.x * PPM, vec.y * PPM};
}

b2Vec2 pixelsToMetersVec(const Vector2 &vec) {
    return b2Vec2{vec.x / PPM, vec.y / PPM};
}

float metersToPixels(float meters) {
    return meters * PPM;
}

float pixelsToMeters(float pixels) {
    return pixels / PPM;
}
