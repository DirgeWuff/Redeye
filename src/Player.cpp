//
// Created by DirgeWuff on 4/29/25.
//

#include <iostream>
#include "box2d/box2d.h"

#include "Player.h"
#include "Utils.h"
#include "Debug.h"
#include "EventCollider.h"
#include "UI.h"

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
m_activeGroundContacts(0),
m_dead(false),
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

    m_currentCheckpoint.centerPosition = m_centerPosition;

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
        pixelsToMeters(20.0f),
        pixelsToMeters(12.0f),
        {0.0f, m_sizeMeters.y / 2.3f},
        b2MakeRot(0.0f)
    );
    m_footpawSensorShape = b2DefaultShapeDef();
    m_footpawSensorShape.isSensor = true;
    m_footpawSensorShape.enableSensorEvents = true;

    // Set our userData for the EventDispatcher to use later
    auto* ud = new sensorInfo{"pawbs"};
    m_footpawSensorShape.userData = static_cast<void*>(ud);
    ud = nullptr;
    delete ud;

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
    if (m_activeGroundContacts > 0) {
        m_onGround = true;
    }
    else {
        m_onGround = false;
    }
}

void Player::draw() const {
    DrawTextureRec(m_walkSprites, m_spriteRect, m_cornerPosition, WHITE);
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
        {0.0f, -(mass * 20.0f)},
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

void Player::murder(const std::unique_ptr<SceneCamera>& camera) {
    assert(camera != nullptr && "Camera == nullptr. Ln 170, Player.cpp");

    unsigned char alpha = 0;
    m_dead = true;
    const Rectangle rect = camera->getCameraRect();
    const Texture2D deathText = LoadTexture("../assets/UI and graphics/Death screen text.png");
    const Vector2 deathTextPos = GetScreenToWorld2D(
        {
            static_cast<float>(1000 / 2 - deathText.width / 2),
            static_cast<float>(400 / 2 - deathText.height / 2)},
            *camera->getCameraPtr());

    auto loadCheckpointBtn = RectButton(
        625.0f,
        500.0f,
        150.0f,
        30.0f,
        "Continue from last checkpoint",
        camera);
    loadCheckpointBtn.setClickEvent([this, &alpha, &rect, &camera] {
        alpha = 0;
        m_dead = false;

        // Fade in red
        while (alpha < 255) {
            ClearBackground({105, 7, 0, alpha});
            BeginDrawing();

            camera->cameraBegin();
            DrawRectangleRec(rect, {10, 1, 0, alpha});
            camera->cameraEnd();

            EndDrawing();

            alpha += 3;
        }

        reform();
    });
    loadCheckpointBtn.setColor(
        {9, 59,36, 255},
        {10,92,54, 255});

    // Fade in black when continue button is clicked
    // NOTE: Static code analyzers will say m_dead is always true, this is incorrect.
    while (m_dead == true && !WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);

        camera->cameraBegin();

        DrawRectangleRec(rect, {105, 7, 0, alpha});
        loadCheckpointBtn.draw(camera);
        DrawTextureEx(deathText, deathTextPos, 0.0f, 2.0f, BLACK);

        camera->cameraEnd();

        EndDrawing();
        if (alpha < 255) alpha += 3;
    }
}

void Player::reform() const noexcept {
    b2Body_SetTransform(
       m_body,
       m_currentCheckpoint.centerPosition,
       b2MakeRot(0.0f));
}

[[nodiscard]] b2ShapeId Player::getFootpawSenorId() const noexcept {
    return m_footpawSensorId;
}

[[nodiscard]] bool Player::getFootpawSensorStatus() const noexcept { // May be able to depreccate
    return m_onGround;
}

void Player::addContactEvent() noexcept {
    m_activeGroundContacts++;
}

void Player::removeContactEvent() noexcept {
    m_activeGroundContacts--;
}

void Player::setCurrentCheckpoint() noexcept {
    m_currentCheckpoint.centerPosition = m_centerPosition;
}
