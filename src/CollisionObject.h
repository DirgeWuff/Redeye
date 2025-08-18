//
// Created by DirgeWuff on 7/15/25.
//

#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include <vector>
#include "box2d/types.h"

class CollisionObject {
    b2Vec2* m_verts;                            // Array of raw pointers to the vertex information
    std::size_t m_numVerts{};                   // Number of vertices in the array
    b2BodyDef m_bodyDef{};                      // The CollisionObject's b2BodyDef
    b2BodyId m_bodyId{};                        // The CollisionObject's b2BodyId
    b2ChainDef m_chainDef{};                    // The CollisionObject's b2ChainDef
    b2ChainId m_chainId{};                      // The CollisionObject's b2ChainId
    b2SurfaceMaterial m_chainMaterial{};        // The CollisionObject's b2SurfaceMaterial
public:
    CollisionObject(
        b2WorldId world,
        const std::vector<b2Vec2>& points);
    CollisionObject(const CollisionObject& other);              // Copy constructor, required due to C-style interface
    CollisionObject& operator=(const CollisionObject& other);   // Move constructor, required due to C-style interface
    ~CollisionObject();

    // TODO: Look into using std::span with C++ 20
    [[nodiscard]] b2Vec2* getObjectVerts() const noexcept;          // Get pointer to the raw vertex array. NOT MEMORY SAFE!
    [[nodiscard]] std::size_t getVertCount() const noexcept;        // Get the number of vertices in the array. Must be used in conjunction with getObjectVerts()
};

#endif //COLLISIONOBJECT_H
