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
// Class definition and constructor for EventCollider, a custom class of Box2D
// object used to trigger callback functions/game behavior. These objects do not have
// collision.

#ifndef EVENTCOLLIDER_H
#define EVENTCOLLIDER_H

#include "../Utility/Utils.h"
#include "../Utility/Enum.h"

namespace RE::Core {
    struct sensorInfo {
        sensorType type{};
        guid id{};

        explicit sensorInfo(const sensorType t) :
            type(t),
            id(generateGuid())
        {

        }

        sensorInfo() = default;
        ~sensorInfo() = default;
    };

    class EventCollider {
    protected:
        b2BodyDef m_bodyDef{};
        b2ShapeDef m_shapeDef{};
        std::string m_typeId{};
        sensorInfo* m_sensorInfo{};
        b2BodyId m_body{};
        b2ShapeId m_shapeId{};
        b2Vec2 m_sizeMeters{};
        b2Vec2 m_centerPosition{};
        Vector2 m_sizePx{};
        Vector2 m_cornerPosition{};
    public:
        EventCollider();
        EventCollider (
            float cornerX,
            float cornerY,
            float fullWidthPx,
            float fullHeightPx,
            sensorType type,
            b2WorldId world);

        ~EventCollider();

        EventCollider(const EventCollider&) = default;
        EventCollider(EventCollider&& other) noexcept = delete;
        EventCollider& operator=(const EventCollider&) = delete;
        EventCollider& operator=(EventCollider&& other) noexcept = delete;

        void disableCollider() const noexcept;
        [[nodiscard]] Vector2 getSizePx() const noexcept;
        [[nodiscard]] Vector2 getPosPixels() const noexcept;
        [[nodiscard]] sensorInfo getSensorInfo() const noexcept;
    };

}

#endif //EVENTCOLLIDER_H
