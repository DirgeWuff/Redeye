//
// Created by DirgeWuff on 8/8/2025.
//

#ifndef EVENTCOLLIDER_H
#define EVENTCOLLIDER_H

#include "BoxBody.h"
#include "EventDispatcher.h"
#include "Player.h"

struct playerContactEvent {
    bool contactBegan;
    b2ShapeId visitorShape;

};

// This shit is only in a struct for type safety reasons, since we cast to void* and back
struct sensorInfo {
    std::string typeId;
};

// Subclass for extra clarity here, should be used for any type of
// Collider object that triggers an event when entered
class EventCollider {
protected:
    b2Vec2 m_sizeMeters{};
    Vector2 m_sizePx{};
    b2Vec2 m_centerPosition{};
    Vector2 m_cornerPosition{};
    b2BodyDef m_bodyDef{};
    b2BodyId m_body{};
    b2ShapeDef m_shapeDef{};
    std::string m_typeId{};
    b2ShapeId m_shapeId{};
public:
    EventCollider() = default;

    EventCollider (
        float cornerX,
        float cornerY,
        float fullWidthPx,
        float fullHeightPx,
        const std::string& id,
        b2WorldId world);

    ~EventCollider() = default;
};

#endif //EVENTCOLLIDER_H
