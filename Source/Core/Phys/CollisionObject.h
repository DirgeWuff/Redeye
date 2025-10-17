//
// Created by DirgeWuff on 7/15/25.
//

#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include <vector>
#include "box2d/types.h"

class CollisionObject {
    b2BodyDef m_bodyDef{};
    b2ChainDef m_chainDef{};
    b2SurfaceMaterial m_chainMaterial{};
    b2Vec2* m_verts;
    std::size_t m_numVerts{};
    b2BodyId m_bodyId{};
    b2ChainId m_chainId{};
public:
    CollisionObject(
        b2WorldId world,
        const std::vector<b2Vec2>& points);

    CollisionObject() = default;
    CollisionObject(const CollisionObject& other);
    CollisionObject(CollisionObject&& other) noexcept;
    CollisionObject& operator=(const CollisionObject& other);
    CollisionObject& operator=(CollisionObject&& other) noexcept;

    ~CollisionObject();

    // TODO: Look into using std::span with C++ 20
    [[nodiscard]] b2Vec2* getObjectVerts() const noexcept;
    [[nodiscard]] std::size_t getVertCount() const noexcept;
};

#endif //COLLISIONOBJECT_H
