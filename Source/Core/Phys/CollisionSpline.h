//
// Author: DirgeWuff
// Created on: 7/15/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class of collision objects based on a Box2D chain shape. These objects
// are composed of multiple vertices that form a spline with collision.
// These objects are used to compose the collision geometry for the
// ground surface.

#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include <vector>
#include "box2d/types.h"

class CollisionSpline {
    b2BodyDef m_bodyDef{};
    b2ChainDef m_chainDef{};
    b2SurfaceMaterial m_chainMaterial{};
    b2Vec2* m_verts;
    std::size_t m_numVerts{};
    b2BodyId m_bodyId{};
    b2ChainId m_chainId{};
public:
    CollisionSpline();
    CollisionSpline(
        b2WorldId world,
        const std::vector<b2Vec2>& points);

    CollisionSpline(const CollisionSpline& other);
    CollisionSpline(CollisionSpline&& other) noexcept;
    CollisionSpline& operator=(const CollisionSpline& other);
    CollisionSpline& operator=(CollisionSpline&& other) noexcept;

    ~CollisionSpline();

    // TODO: Look into using std::span with C++ 20
    [[nodiscard]] b2Vec2* getObjectVerts() const noexcept;
    [[nodiscard]] std::size_t getVertCount() const noexcept;
};

#endif //COLLISIONOBJECT_H
