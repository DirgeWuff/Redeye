//
// Created by DirgeWuff on 7/8/25.
//

#include "external_libs/Box2D/include/box2d.h"
#include "BoxBody.h"
#include "Utils.h"

// Entirely possible this should be made into a pure virtual class or removed all together.
// Don't really see a need for this object in the final game.

BoxBody::BoxBody(
    const float centerX,
    const float centerY,
    const float fullWidth,
    const float fullHeight,
    const bodyConfig config,
    const b2WorldId world) :
        m_sizeMeters{pixelsToMeters(fullWidth), pixelsToMeters(fullHeight)},
        m_sizePx{fullWidth, fullHeight},
        m_centerPosition{pixelsToMeters(centerX), pixelsToMeters(centerY)},
        m_cornerPosition{centerX - m_sizePx.x / 2.0f, centerY - m_sizePx.y / 2.0f},
        m_bodyDef(b2DefaultBodyDef())
{
    m_bodyDef.position = m_centerPosition;
    m_bodyDef.type = config.bodyType;
    m_bodyDef.fixedRotation = config.rotationEnabled;
    m_bodyDef.linearDamping = config.damping;
    m_body = b2CreateBody(world, &m_bodyDef);

    b2Polygon boundingBox = b2MakeBox(m_sizeMeters.x / 2.0f, m_sizeMeters.y / 2.0f);
    m_shapeDef = b2DefaultShapeDef();
    m_shapeDef.material.friction = config.friction;
    m_shapeDef.enableSensorEvents = true;
    b2CreatePolygonShape(m_body, &m_shapeDef, &boundingBox);
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

b2Vec2 BoxBody::getSizeMeters() const {
    return m_sizeMeters;
}

Vector2 BoxBody::getSizePx() const {
    return m_sizePx;
}

b2Vec2 BoxBody::getPositionCenterMeters() const {
    return m_centerPosition;
}

Vector2 BoxBody::getPositionCornerPx() const {
    return m_cornerPosition;
}

b2BodyId BoxBody::getBodyID() const {
    return m_body;
}
