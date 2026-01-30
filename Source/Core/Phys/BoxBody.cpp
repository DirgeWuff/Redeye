//
// Author: DirgeWuff
// Created on: 7/8/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class definition for BoxBody.h and its member functions


#include "box2d/box2d.h"
#include "BoxBody.h"
#include "../Utility/Utils.h"

BoxBody::BoxBody() {
    #ifdef DEBUG
        logDbg("Default BoxBody constructed at address: ", this);
    #endif
}

BoxBody::BoxBody(
    const float centerX,
    const float centerY,
    const float fullWidth,
    const float fullHeight,
    const bodyConfig config,
    const b2WorldId world) :
        m_bodyDef(b2DefaultBodyDef()),
        m_world(world),
        m_sizeMeters{pixelsToMeters(fullWidth), pixelsToMeters(fullHeight)},
        m_centerPosition{pixelsToMeters(centerX), pixelsToMeters(centerY)},
        m_sizePx{fullWidth, fullHeight},
        m_cornerPosition{centerX - m_sizePx.x / 2.0f, centerY - m_sizePx.y / 2.0f}
{
    m_bodyDef.position = m_centerPosition;
    m_bodyDef.type = config.bodyType;
    m_bodyDef.fixedRotation = config.rotationEnabled;
    m_bodyDef.linearDamping = config.damping;
    m_body = b2CreateBody(m_world, &m_bodyDef);

    b2Polygon boundingBox = b2MakeBox(m_sizeMeters.x / 2.0f, m_sizeMeters.y / 2.0f);
    m_shapeDef = b2DefaultShapeDef();
    m_shapeDef.material.friction = config.friction;
    m_shapeDef.enableSensorEvents = true;
    b2CreatePolygonShape(m_body, &m_shapeDef, &boundingBox);

    #ifdef DEBUG
        logDbg("BoxBody constructed at address: ", this);
    #endif
}

BoxBody::~BoxBody() {
    #ifdef DEBUG
        logDbg("BoxBody destroyed at address: ", this);
    #endif
}

void BoxBody::update() {
    m_centerPosition = b2Body_GetPosition(m_body);
    m_cornerPosition.x = metersToPixels(m_centerPosition.x - m_sizeMeters.x / 2);
    m_cornerPosition.y = metersToPixels(m_centerPosition.y - m_sizeMeters.y / 2);
}

void BoxBody::draw() const {
    DrawRectangle(
            static_cast<int>(metersToPixels(m_centerPosition.x)) - static_cast<int>(m_sizePx.x) / 2,
            static_cast<int>(metersToPixels(m_centerPosition.y)) - static_cast<int>(m_sizePx.y) / 2,
            m_sizePx.x,
            m_sizePx.y,
            RED);
}

[[nodiscard]] b2Vec2 BoxBody::getSizeMeters() const noexcept {
    return m_sizeMeters;
}

[[nodiscard]] Vector2 BoxBody::getSizePx() const noexcept {
    return m_sizePx;
}

[[nodiscard]] b2Vec2 BoxBody::getPositionCenterMeters() const noexcept {
    return m_centerPosition;
}

[[nodiscard]] Vector2 BoxBody::getPositionCornerPx() const noexcept {
    return m_cornerPosition;
}

[[nodiscard]] b2BodyId BoxBody::getBodyID() const noexcept {
    return m_body;
}
