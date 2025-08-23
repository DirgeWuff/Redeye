//
// Created by DirgeWuff on 8/8/2025.
//

#ifndef EVENTCOLLIDER_H
#define EVENTCOLLIDER_H

#include "Player.h"

struct playerContactEvent {
    bool contactBegan;
    b2ShapeId visitorShape;
};

// This shit is only in a struct for type safety reasons, since we cast to void* and back
struct sensorInfo {
    std::string typeId;
};

// An invisible collider for use in triggering events in game, such as a cutscene or death
class EventCollider {
protected:
    b2Vec2 m_sizeMeters{};          // EventCollider size in meters
    Vector2 m_sizePx{};             // EventCollider size in pixels
    b2Vec2 m_centerPosition{};      // Center position of the EventCollider in meters
    Vector2 m_cornerPosition{};     // Corner position of the EventCollider in pixels
    b2BodyDef m_bodyDef{};          // b2BodyDef of the EventCollider
    b2BodyId m_body{};              // b2BodyId of the EventCollider
    b2ShapeDef m_shapeDef{};        // b2ShapeDef of the EventCollider
    std::string m_typeId{};         // typeId of the EventCollider, used by EventDispatcher for parsing purposes
    b2ShapeId m_shapeId{};          // b2ShapeId of the EventCollider
public:
    EventCollider() = default;

    EventCollider (
        float cornerX,
        float cornerY,
        float fullWidthPx,
        float fullHeightPx,
        const std::string&& id,
        b2WorldId world);

    ~EventCollider() = default;

    void disableCollider() const noexcept;
    [[nodiscard]] Vector2 getSizePx() const noexcept;
    [[nodiscard]] Vector2 getPosPixels() const noexcept;
};

#endif //EVENTCOLLIDER_H
