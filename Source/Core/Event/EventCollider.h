//
// Created by DirgeWuff on 8/8/2025.
//

#ifndef EVENTCOLLIDER_H
#define EVENTCOLLIDER_H

#include "../Utility/Error.h"
#include "../Utility/Utils.h"

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
    b2BodyDef m_bodyDef{};
    b2ShapeDef m_shapeDef{};
    std::string m_typeId{};
    b2BodyId m_body{};
    b2ShapeId m_shapeId{};
    b2Vec2 m_sizeMeters{};
    b2Vec2 m_centerPosition{};
    Vector2 m_sizePx{};
    Vector2 m_cornerPosition{};
    std::unique_ptr<sensorInfo> m_sensorInfo{};
public:
    EventCollider() = default;

    template<typename T>
    EventCollider (
        float cornerX,
        float cornerY,
        float fullWidthPx,
        float fullHeightPx,
        T&& id,
        b2WorldId world) :
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

        b2Polygon boundingBox = b2MakeBox(m_sizeMeters.x / 2.0f, m_sizeMeters.y / 2.0f);
        m_shapeDef = b2DefaultShapeDef();
        m_shapeDef.isSensor = true;
        m_shapeDef.enableSensorEvents = true;

        try {
            m_sensorInfo = std::make_unique<sensorInfo>(std::string(std::forward<T>(id)));
            m_shapeDef.userData = static_cast<void*>(m_sensorInfo.get());
        }
        catch (const std::bad_alloc& e) {
            logErr(
                std::string("EventCollider::EventCollider() failed, std::bad_alloc thrown: " +
                    std::string(e.what())));

            return;
        }
        catch (...) {
            logErr("EventCollider::EventCollider() failed, an unknown error has occurred.");
            return;
        }

        m_shapeId = b2CreatePolygonShape(m_body, &m_shapeDef, &boundingBox);
    }
    ~EventCollider() = default;

    void disableCollider() const noexcept;
    [[nodiscard]] Vector2 getSizePx() const noexcept;
    [[nodiscard]] Vector2 getPosPixels() const noexcept;
};

#endif //EVENTCOLLIDER_H
