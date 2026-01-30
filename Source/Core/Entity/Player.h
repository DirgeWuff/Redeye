//
// Author: DirgeWuff
// Created on: 4/29/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Main player class declaration for the game and player constructor definition.
// This class is one of the core classes in GameLayer, and defines an interface
// for the player to be controlled by the user, and queried by other modules
// that rely on it or its data.

#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <vector>
#include "raylib.h"
#include "../../Core/Phys/BoxBody.h"
#include "../../Core/Audio/Audio.h"
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Utility/Utils.h"
#include "../../Core/Serialization/Save.h"
#include "../../Core/Utility/Globals.h"
#include "../../Core/Renderer/Animation.h"
#include "../../Core/Renderer/AnimationManager.h"
#include "../Serialization/AnimationLoader.h"

class SceneCamera;

class Player final : public BoxBody, public std::enable_shared_from_this<Player> {
    b2Polygon m_footpawSensorBox{};
    std::vector<animationDescriptor> m_playerAnimations{};
    AnimationManager m_animationManager{};
    b2ShapeDef m_footpawSensorShape{};
    std::string m_playerSpritePath{};
    std::vector<Sound> m_footstepSounds{};
    std::vector<Sound> m_landingSounds{};
    std::unique_ptr<sensorInfo> m_footpawSensorInfo;
    b2ShapeId m_footpawSensorId{};
    std::uint16_t m_activeGroundContacts{};
    std::uint8_t m_soundDelayClock{};
    std::int8_t m_movementIntent{};
    direction m_currentDirection{};
    entityActionState m_currentState{};
    animationId m_currentAnimId{};
    bool m_jumpIntent{};
    bool m_dead{};

    void moveRight(const b2WorldId& world) const;
    void moveLeft(const b2WorldId& world) const;
    void jump() const;
public:
    Player() = default;

    template<typename T>
    Player(
        const float centerX,
        const float centerY,
        const b2WorldId world,
        T&& spritePath) :
            m_playerAnimations(loadAnimations(g_playerAnimPath)),
            m_animationManager(
                spritePath,
                m_playerAnimations,
                animationId::PLAYER_IDLE_RIGHT),
            m_playerSpritePath(std::string(std::forward<T>(spritePath))),
            m_currentDirection(direction::RIGHT),
            m_currentState(entityActionState::IDLE)
{
    m_sizePx = m_animationManager.getSpriteSize();
    m_sizeMeters = pixelsToMetersVec(m_sizePx);
    m_centerPosition = {pixelsToMeters(centerX), pixelsToMeters(centerY)};
    m_cornerPosition = {centerX - m_sizePx.x / 2.0f, centerY - m_sizePx.y / 2.0f};

    m_footstepSounds = loadSoundVector("../assets/Player assets/Sounds/Walk", 0.2f);
    m_landingSounds = loadSoundVector("../assets/Player assets/Sounds/Jump", 0.2f);

    m_bodyDef = b2DefaultBodyDef();
    m_bodyDef.position = m_centerPosition;
    m_bodyDef.type = b2_dynamicBody;
    m_bodyDef.fixedRotation = true;
    m_bodyDef.linearDamping = 8.0f;
    m_body = b2CreateBody(world, &m_bodyDef);

    const b2Capsule boundingCapsule = {
        pixelsToMetersVec(Vector2(0.0f, -20.0f)),
        pixelsToMetersVec(Vector2(0.0f, 32.0f)),
        pixelsToMeters(28.0f)
    };

    m_shapeDef = b2DefaultShapeDef();
    m_shapeDef.material.friction = 0.50f;
    m_shapeDef.material.restitution = 0.0f;
    m_shapeDef.density = 8.0f;
    m_shapeDef.filter.categoryBits = g_playerCategoryBits;
    m_shapeDef.filter.maskBits = g_universalMaskBits;   // Using this for now, may change later...
    b2CreateCapsuleShape(m_body, &m_shapeDef, &boundingCapsule);

    // Footpaw sensor :3
    m_footpawSensorBox = b2MakeOffsetBox(
        pixelsToMeters(20.0f),
        pixelsToMeters(12.0f),
        {0.0f, m_sizeMeters.y / 3.0f},
        b2MakeRot(0.0f)
    );

    m_footpawSensorShape = b2DefaultShapeDef();
    m_footpawSensorShape.isSensor = true;
    m_footpawSensorShape.enableSensorEvents = true;
    m_footpawSensorShape.filter.categoryBits = g_footpawCategoryBits;
    m_footpawSensorShape.filter.maskBits = g_universalMaskBits;

    // Set our userData for the EventDispatcher to use later
    try {
        m_footpawSensorInfo = std::make_unique<sensorInfo>(sensorType::PLAYER_FOOTPAW_SENSOR);
        m_footpawSensorShape.userData = static_cast<void*>(m_footpawSensorInfo.get());
    }
    catch (const std::exception& e) {
        logFatal(std::string("Failed to allocate for m_footpawSensorInfo: ") + std::string(e.what()) +
                std::string(". Player::Player(Args...)"));

        return;
    }
    catch (...) {
        logFatal("Failed to allocate for m_footpawSensorInfo: An unknown error has occurred."
                 "Player::Player(Args...)");

        return;
    }

    m_footpawSensorId = b2CreatePolygonShape(
        m_body,
        &m_footpawSensorShape,
        &m_footpawSensorBox);

    m_currentAnimId = m_animationManager.getCurrentAnimId();

    #ifdef DEBUG
        logDbg("Class player constructed at address: ", this);
    #endif
}
    Player(const Player&) = delete;
    Player(Player&&) noexcept = delete;
    Player& operator=(const Player&) = delete;
    Player& operator=(Player&&) noexcept = delete;

    ~Player() override;

    void pollEvents();
    void update(const b2WorldId& world);
    void draw() const override;
    void murder();
    void reform(const saveData& save);
    void addContactEvent() noexcept;
    void removeContactEvent() noexcept;
    [[nodiscard]] b2Vec2 getGroundNormals(const b2WorldId& world) const;

    [[nodiscard]] sensorInfo getFootpawSensorInfo() const noexcept;
    [[nodiscard]] b2ShapeId getFootpawSenorId() const noexcept; // Do we ever use this???
    [[nodiscard]] bool isOnGround() const noexcept;
    [[nodiscard]] bool isDead() const noexcept;
    [[nodiscard]] direction getPlayerDirection() const noexcept;
    [[nodiscard]] animationId getCurrentAnimId() const noexcept;
    [[nodiscard]] entityActionState getCurrentActionState() const noexcept;
};

#endif //PLAYER_H