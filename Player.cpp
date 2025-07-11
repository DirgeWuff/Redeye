//
// Created by DirgeWuff on 4/29/25.
//

#include <box2d/box2d.h>
#include <iostream>
#include "Player.h"
#include "Utils.h"
#include "Debug.h"

enum directions {RIGHT, LEFT};

Player::Player(
    const float centerX,
    const float centerY,
    const b2WorldId world,
    const std::string& spritePath) :
m_numFrames(3),
m_frameIndex(0),
m_frameDelayAmount(10),
m_frameDelayClock(0),
m_moving(false),
m_onGround(true),
m_lastDirection(RIGHT)
{
    // Raylib/general stuff
    m_walkSprites = LoadTexture(spritePath.c_str());
    m_spriteRect = {
        0.0f,
        static_cast<float>(m_walkSprites.height) / 2,
        static_cast<float>(m_walkSprites.width) / 3,
        static_cast<float>(m_walkSprites.height) / 2
    };

    m_sizePx = {m_walkSprites.width / 3.0f, m_walkSprites.height / 2.0f};
    m_sizeMeters = pixelsToMetersVec(m_sizePx);
    m_centerPosition = {pixelsToMeters(centerX), pixelsToMeters(centerY)};
    m_cornerPosition ={centerX - m_sizePx.x / 2, centerY - m_sizePx.y / 2};

    // B2 body stuff
    m_bodyDef = b2DefaultBodyDef();
    m_bodyDef.position = m_centerPosition;
    m_bodyDef.type = b2_dynamicBody;
    m_bodyDef.fixedRotation = true;
    m_bodyDef.linearDamping = 8.0f;
    m_body = b2CreateBody(world, &m_bodyDef);

    const b2Capsule boundingCapsule = {
        pixelsToMetersVec({Vector2{0.0f, -20.0f}}),
        pixelsToMetersVec(Vector2{0.0f, 32.0f}),
        pixelsToMeters(28.0f)
    };

    m_shapeDef = b2DefaultShapeDef();
    m_shapeDef.material.friction = 0.50f;
    m_shapeDef.material.restitution = 0.0f;
    m_shapeDef.density = 8.0f;
    b2CreateCapsuleShape(m_body, &m_shapeDef, &boundingCapsule);

    // Footpaw sensor :3
    m_footpawSensorBox = b2MakeOffsetBox(
        pixelsToMeters(12.0f),
        pixelsToMeters(12.0f),
        {0.0f, m_sizeMeters.y / 2.5f},
        b2MakeRot(0.0f)
    );
    m_footpawSensorShape = b2DefaultShapeDef();
    m_footpawSensorShape.isSensor = true;
    m_footpawSensorShape.enableSensorEvents = true;
    m_footpawSensorId = b2CreatePolygonShape(
        m_body,
        &m_footpawSensorShape,
        &m_footpawSensorBox);
}

Player::~Player() {
    UnloadTexture(m_walkSprites);
    b2DestroyBody(m_body);
}

void Player::update() {
    m_centerPosition = b2Body_GetPosition(m_body);
    m_cornerPosition = {
        metersToPixels(m_centerPosition.x) - m_sizePx.x / 2,
        metersToPixels(m_centerPosition.y) - m_sizePx.y / 2
    };
}

void Player::draw() const {
    DrawTextureRec(m_walkSprites, m_spriteRect, m_cornerPosition, WHITE);

    #ifdef DEBUG_DRAW_PLAYER_SHAPES
        drawDebugBodyShapes(this);
    #endif
}

void Player::moveRight() {
    const float mass = b2Body_GetMass(m_body);
    m_lastDirection = RIGHT;

    b2Body_ApplyLinearImpulse(
        m_body,
        {mass * .50f, 0.0f},
        b2Body_GetWorldCenterOfMass(m_body),
        true);

    if (m_frameDelayClock >= m_frameDelayAmount) {
        m_frameIndex++;
        m_spriteRect.y = static_cast<float>(m_walkSprites.height) / 2;
        m_spriteRect.x = m_sizePx.x * static_cast<float>(m_frameIndex);
        m_frameDelayClock = 0;
    }
    else {
        m_frameDelayClock++;
    }
}

void Player::moveLeft() {
    const float mass = b2Body_GetMass(m_body);
    m_lastDirection = LEFT;

    b2Body_ApplyLinearImpulse(
        m_body,
        {-(mass * .50f), 0.0f},
        b2Body_GetWorldCenterOfMass(m_body),
        true);

    if (m_frameDelayClock >= m_frameDelayAmount) {
        m_frameIndex++;
        m_spriteRect.y = 0.0f;
        m_spriteRect.x = m_sizePx.x * static_cast<float>(m_frameIndex);
        m_frameDelayClock = 0;
    }
    else {
        m_frameDelayClock++;
    }
}

void Player::jump() {
    if (m_onGround) {
        const float mass = b2Body_GetMass(m_body);

        b2Body_ApplyLinearImpulse(
        m_body,
        {0.0f, -(mass * 10.0f)},
        b2Body_GetWorldCenterOfMass(m_body),
        true);
    }
}

// Control player idle sprite
void Player::moveNowhere() {
    if (m_lastDirection == RIGHT) {
        m_spriteRect.y = static_cast<float>(m_walkSprites.height) / 2;
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
    else {
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
}

b2ShapeId Player::getFootpawSenorId() const {
    return m_footpawSensorId;
}

bool Player::getFootpawSensorStatus() const {
    return m_onGround;
}

void Player::setFootpawStatus(const bool status) {
    if (m_onGround != status) {
        m_onGround = status;
    }
}
