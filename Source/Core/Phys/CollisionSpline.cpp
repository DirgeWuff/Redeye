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
// Class definition for CollisionSpline and all of its member functions, as well
// as move/copy constructors, and move/copy assignment constructors, required
// due to the nature of the vertex array.

#include <cstring>
#include "box2d/types.h"
#include "box2D/box2d.h"
#include "CollisionSpline.h"
#include "../Utility/Globals.h"
#include "../Utility/Logging.h"


CollisionSpline::CollisionSpline() {
    #ifdef DEBUG
        logDbg("Default CollisionSpline constructed at address: ", this);
    #endif
}

CollisionSpline::CollisionSpline(
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
    m_chainDef.filter.categoryBits = g_groundCategoryBits;
    m_chainDef.filter.maskBits = g_universalMaskBits;
    m_chainId = b2CreateChain(m_bodyId, &m_chainDef);

    #ifdef DEBUG
        logDbg("CollisionObjected constructed at address: ", this);
    #endif
}

CollisionSpline::CollisionSpline(const CollisionSpline& other) :
    m_bodyDef(other.m_bodyDef),
    m_chainDef(b2DefaultChainDef()),
    m_numVerts(other.m_numVerts),
    m_bodyId(other.m_bodyId)
{
    m_bodyDef.type = other.m_bodyDef.type;

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

CollisionSpline::CollisionSpline(CollisionSpline&& other) noexcept :
    m_bodyDef(std::move(other.m_bodyDef)),
    m_chainDef(b2DefaultChainDef()),
    m_chainMaterial(std::move(other.m_chainMaterial)),
    m_verts(other.m_verts),
    m_numVerts(other.m_numVerts),
    m_bodyId(other.m_bodyId)
{
    other.m_verts = nullptr;
    other.m_numVerts = 0;

    m_chainDef.count = m_numVerts;
    m_chainDef.points = m_verts;
    m_chainDef.materials = &m_chainMaterial;
    m_chainDef.materialCount = 1;
    m_chainDef.isLoop = false;

    m_chainId = b2CreateChain(m_bodyId, &m_chainDef);
}

CollisionSpline& CollisionSpline::operator=(const CollisionSpline& other) {
    if (this != &other) {
        delete[] m_verts;
        m_verts = nullptr;

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

CollisionSpline & CollisionSpline::operator=(CollisionSpline&& other) noexcept {
    if (this != &other) {
        delete[] m_verts;

        m_numVerts = other.m_numVerts;
        m_bodyId = other.m_bodyId;
        m_bodyDef = std::move(other.m_bodyDef);
        m_chainMaterial = std::move(other.m_chainMaterial);

        m_verts = other.m_verts;
        other.m_verts = nullptr;
        other.m_numVerts = 0;

        m_chainDef.count = m_numVerts;
        m_chainDef.points = m_verts;
        m_chainDef.materials = &m_chainMaterial;
        m_chainDef.materialCount = 1;
        m_chainDef.isLoop = false;

        m_chainId = b2CreateChain(m_bodyId, &m_chainDef);
    }

    return *this;
}

CollisionSpline::~CollisionSpline() {
    delete[] m_verts;
    m_verts = nullptr;

    #ifdef DEBUG
        logDbg("Collision object destroyed at address: ", this);
    #endif
}

[[nodiscard]] b2Vec2* CollisionSpline::getObjectVerts() const noexcept {
    return m_verts;
}

[[nodiscard]] std::size_t CollisionSpline::getVertCount() const noexcept {
    return m_numVerts;
}
