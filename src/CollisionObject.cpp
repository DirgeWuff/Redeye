//
// Created by DirgeWuff on 7/15/25.
//

#include <cstring>
#include "box2d/types.h"
#include "box2d/box2d.h"
#include "CollisionObject.h"


// Base class for a CollisionObject, generated from vertices in a tiled map's object layer.
// These are used to generate collisions with the terrain in a world.
CollisionObject::CollisionObject(
    const b2WorldId world,
    const std::vector<b2Vec2>& points) :
m_bodyDef(b2DefaultBodyDef()),
m_chainDef(b2DefaultChainDef())
{
    m_bodyDef.type = b2_staticBody;
    m_bodyId = b2CreateBody(world, &m_bodyDef);

    m_numVerts = points.size();
    m_verts = new b2Vec2[m_numVerts];

    memcpy(m_verts, points.data(), m_numVerts * sizeof(b2Vec2));

    m_chainMaterial = b2DefaultSurfaceMaterial();
    m_chainMaterial.friction = 0.2f;
    m_chainMaterial.restitution = 0.01f;

    m_chainDef.count = m_numVerts;
    m_chainDef.points = m_verts;
    m_chainDef.materials = &m_chainMaterial;
    m_chainDef.materialCount = 1;
    m_chainDef.isLoop = false;
    m_chainDef.enableSensorEvents = true;
    m_chainId = b2CreateChain(m_bodyId, &m_chainDef);
}

CollisionObject::CollisionObject(const CollisionObject& other) :
m_numVerts(other.m_numVerts),
m_bodyDef(other.m_bodyDef),
m_chainDef(other.m_chainDef)
{
    m_bodyDef.type = other.m_bodyDef.type;
    m_bodyId = other.m_bodyId;

    m_verts = new b2Vec2[m_numVerts];

    memcpy(m_verts, other.m_verts, m_numVerts * sizeof(b2Vec2));

    m_chainMaterial = other.m_chainMaterial;
    m_chainMaterial.friction = other.m_chainMaterial.friction;
    m_chainMaterial.restitution = other.m_chainMaterial.restitution;

    m_chainDef.count = m_numVerts;
    m_chainDef.points = m_verts;
    m_chainDef.materials = &m_chainMaterial;
    m_chainDef.materialCount = 1;
    m_chainDef.isLoop = false;
    m_chainId = b2CreateChain(m_bodyId, &m_chainDef);
}


CollisionObject& CollisionObject::operator=(const CollisionObject& other) {
    if (this != &other) {
        delete[] m_verts;

        m_numVerts = other.m_numVerts;
        m_bodyDef = other.m_bodyDef;
        m_chainDef = other.m_chainDef;

        m_verts = new b2Vec2[m_numVerts];
        memcpy(m_verts, other.m_verts, m_numVerts * sizeof(b2Vec2));

        m_chainMaterial = other.m_chainMaterial;
        m_chainMaterial.friction = other.m_chainMaterial.friction;
        m_chainMaterial.restitution = other.m_chainMaterial.restitution;

        m_chainDef.count = m_numVerts;
        m_chainDef.points = m_verts;
        m_chainDef.materials = &m_chainMaterial;
        m_chainDef.materialCount = 1;
        m_chainDef.isLoop = false;
        m_chainId = b2CreateChain(m_bodyId, &m_chainDef);
    }

    return *this;
}

CollisionObject::~CollisionObject() {
    delete[] m_verts;
}

[[nodiscard]] b2Vec2* CollisionObject::getObjectVerts() const noexcept {
    return m_verts;
}

[[nodiscard]] std::size_t CollisionObject::getVertCount() const noexcept {
    return m_numVerts;
}

