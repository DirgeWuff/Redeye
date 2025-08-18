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
    const bool rotationEnabled;
    const bool isSensor;
    const bool sensorEventsEnabled;
    const float damping;
    const float friction;
};

class BoxBody {
protected:
    b2Vec2 m_sizeMeters{};          // Body size in meters
    Vector2 m_sizePx{};             // Body size in pixels
    b2Vec2 m_centerPosition{};      // Center position of the body
    Vector2 m_cornerPosition{};     // Top left corner position of the body
    b2BodyDef m_bodyDef{};          // Box2D Body Def
    b2BodyId m_body{};              // Box2D Body ID
    b2ShapeDef m_shapeDef{};        // Box2D Shape Def
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

    virtual void update();                                              // Update the BoxBody
    virtual void draw() const;                                          // Draw the BoxBody
    [[nodiscard]] b2Vec2 getSizeMeters() const noexcept;                // Get BoxBody size in meters
    [[nodiscard]] Vector2 getSizePx() const noexcept;                   // Get BoxBody size in Pixels
    [[nodiscard]] b2Vec2 getPositionCenterMeters() const noexcept;      // Get the center position of the BoxBody
    [[nodiscard]] Vector2 getPositionCornerPx() const noexcept;         // Get the corner position of the BoxBody
    [[nodiscard]] b2BodyId getBodyID() const noexcept;                  // Get the b2BodyId of the BoxBody
};


#endif //BOXBODY_H
