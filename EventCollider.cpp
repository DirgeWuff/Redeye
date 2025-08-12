//
// Created by DirgeWuff on 8/8/2025.
//

#include "EventCollider.h"
#include "Utils.h"

EventCollider::EventCollider (
    const float cornerX,
    const float cornerY,
    const float fullWidthPx,
    const float fullHeightPx,
    const std::string& id,
    const b2WorldId world) :
        m_sizeMeters{pixelsToMeters(fullWidthPx), pixelsToMeters(fullHeightPx)},
        m_sizePx{fullWidthPx, fullHeightPx},
        m_centerPosition{
            pixelsToMeters(cornerX) + m_sizeMeters.x / 2.0f,
            pixelsToMeters(cornerY) + m_sizeMeters.y / 2.0f},
        m_cornerPosition{cornerX, cornerY},
        m_bodyDef(b2DefaultBodyDef())
{
    m_bodyDef.position = m_centerPosition;
    m_bodyDef.type = b2_staticBody;
    m_bodyDef.fixedRotation = false;
    m_body = b2CreateBody(world, &m_bodyDef);

    b2Polygon boundingBox = b2MakeBox(m_sizeMeters.x / 2.0f, m_sizeMeters.y / 2.0f);
    m_shapeDef = b2DefaultShapeDef();
    m_shapeDef.isSensor = true;
    m_shapeDef.enableSensorEvents = true;

    // Yucky hack, not a fan of this
    auto* ud = new sensorInfo{id};
    m_shapeDef.userData = static_cast<void*>(ud);
    ud = nullptr;
    delete ud;

    m_shapeId = b2CreatePolygonShape(m_body, &m_shapeDef, &boundingBox);
}
