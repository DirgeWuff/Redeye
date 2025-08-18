//
// Created by DirgeWuff on 7/8/25.
//

// Base class for any box2d 'box object'

#ifndef BOXBODY_H
#define BOXBODY_H

#include "external_libs/Raylib/include/raylib.h"
#include "external_libs/Box2D/include/types.h"

struct bodyConfig {
    b2BodyType bodyType;
    const bool rotationEnabled;
    const bool isSensor;
    const bool sensorEventsEnabled;
    const float damping;
    const float friction;
};

class BoxBody {
protected:
    b2Vec2 m_sizeMeters{};
    Vector2 m_sizePx{};
    b2Vec2 m_centerPosition{};
    Vector2 m_cornerPosition{};
    b2BodyDef m_bodyDef{};
    b2BodyId m_body{};
    b2ShapeDef m_shapeDef{};
public:
    BoxBody() = default;

    BoxBody(
        float centerX,
        float centerY,
        float fullWidth,
        float fullHeight,
        bodyConfig config,
        b2WorldId world);

    virtual ~BoxBody() = default;

    virtual void update();
    virtual void draw() const;
    [[nodiscard]] b2Vec2 getSizeMeters() const noexcept;
    [[nodiscard]] Vector2 getSizePx() const noexcept;
    [[nodiscard]] b2Vec2 getPositionCenterMeters() const noexcept;
    [[nodiscard]] Vector2 getPositionCornerPx() const noexcept;
    [[nodiscard]] b2BodyId getBodyID() const noexcept;
};


#endif //BOXBODY_H
