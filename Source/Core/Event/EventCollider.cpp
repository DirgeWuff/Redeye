//
// Author: DirgeWuff
// Created on: 8/8/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class definition for EventCollider.h and its member functions.

#include "box2d/box2d.h"
#include "EventCollider.h"
#include <sys/stat.h>
#include "../Utility/Logging.h"

EventCollider::EventCollider (
        const float cornerX,
        const float cornerY,
        const float fullWidthPx,
        const float fullHeightPx,
        const sensorType type,
        const b2WorldId world) :
            m_bodyDef(b2DefaultBodyDef()),
            m_sizeMeters{pixelsToMeters(fullWidthPx), pixelsToMeters(fullHeightPx)},
            m_centerPosition{
                pixelsToMeters(cornerX) + m_sizeMeters.x / 2.0f,
                pixelsToMeters(cornerY) + m_sizeMeters.y / 2.0f},
            m_sizePx{fullWidthPx, fullHeightPx},
            m_cornerPosition{cornerX, cornerY}
{
    m_bodyDef.position = m_centerPosition;
    m_bodyDef.type = b2_staticBody;
    m_bodyDef.fixedRotation = false;
    m_body = b2CreateBody(world, &m_bodyDef);

    const b2Polygon boundingBox = b2MakeBox(m_sizeMeters.x / 2.0f, m_sizeMeters.y / 2.0f);
    m_shapeDef = b2DefaultShapeDef();
    m_shapeDef.isSensor = true;
    m_shapeDef.enableSensorEvents = true;

    m_sensorInfo = new sensorInfo;
    m_sensorInfo->id = generateGuid();
    m_sensorInfo->type = type;
    m_shapeDef.userData = static_cast<void*>(m_sensorInfo);

    m_shapeId = b2CreatePolygonShape(m_body, &m_shapeDef, &boundingBox);
}

EventCollider::EventCollider() {
    #ifdef DEBUG
        logDbg("Default EventCollider constructed at address: ", this);
    #endif
}

EventCollider::~EventCollider() {
#ifdef DEBUG
    logDbg("Default EventCollider destroyed at address: ", this);
#endif
}

void EventCollider::disableCollider() const noexcept {
    b2Body_Disable(m_body);
}

[[nodiscard]] Vector2 EventCollider::getSizePx() const noexcept {
    return m_sizePx;
}

[[nodiscard]] Vector2 EventCollider::getPosPixels() const noexcept {
    return m_cornerPosition;
}

sensorInfo EventCollider::getSensorInfo() const noexcept {
    assert(m_sensorInfo);

    return *m_sensorInfo;
}
