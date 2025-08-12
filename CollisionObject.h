//
// Created by DirgeWuff on 7/15/25.
//

#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include <vector>
#include "external_libs/Box2D/include/types.h"

class CollisionObject {
    b2Vec2* m_verts; // This makes everything entirely too complex...
    std::size_t m_numVerts{};
    b2BodyDef m_bodyDef{};
    b2BodyId m_bodyId{};
    b2ChainDef m_chainDef{};
    b2ChainId m_chainId{};
    b2SurfaceMaterial m_chainMaterial{};
public:
    CollisionObject(
        b2WorldId world,
        const std::vector<b2Vec2>& points);
    CollisionObject(const CollisionObject& other);
    CollisionObject& operator=(const CollisionObject& other);
    ~CollisionObject();

    // Doing this the old-fashioned way because clang 14.x doesn't support std::span
    // Consider changing that later if compiler changes
    b2Vec2* getObjectVerts() const;
    std::size_t getVertCount() const;
};

#endif //COLLISIONOBJECT_H
