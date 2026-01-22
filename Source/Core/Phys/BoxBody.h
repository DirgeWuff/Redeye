//
// Created by DirgeWuff on 7/8/25.
//

// Base class for any box2d 'box object'

#ifndef BOXBODY_H
#define BOXBODY_H

#include "raylib.h"
#include "box2d/types.h"

// For use in configuring custom bodies of this type...
struct bodyConfig {
    b2BodyType bodyType;
    const float damping;
    const float friction;
    const bool rotationEnabled;
    const bool isSensor;
    const bool sensorEventsEnabled;
};

// Base class for dynamic box-shaped collision bodies
class BoxBody {
protected:
    b2BodyDef m_bodyDef{};
    b2ShapeDef m_shapeDef{};
    b2WorldId m_world{};
    b2BodyId m_body{};
    b2Vec2 m_sizeMeters{};
    b2Vec2 m_centerPosition{};
    Vector2 m_sizePx{};
    Vector2 m_cornerPosition{};
public:
    BoxBody();

    BoxBody(
        float centerX,
        float centerY,
        float fullWidth,
        float fullHeight,
        bodyConfig config,
        b2WorldId world);

    BoxBody(const BoxBody&) = default;
    BoxBody(BoxBody&&) noexcept = delete;
    BoxBody& operator=(const BoxBody&) = default;
    BoxBody& operator=(BoxBody&&) noexcept = delete;

    virtual ~BoxBody();

    virtual void update();
    virtual void draw() const;
    [[nodiscard]] b2Vec2 getSizeMeters() const noexcept;
    [[nodiscard]] Vector2 getSizePx() const noexcept;
    [[nodiscard]] b2Vec2 getPositionCenterMeters() const noexcept;
    [[nodiscard]] Vector2 getPositionCornerPx() const noexcept;
    [[nodiscard]] b2BodyId getBodyID() const noexcept;
};

#endif //BOXBODY_H
