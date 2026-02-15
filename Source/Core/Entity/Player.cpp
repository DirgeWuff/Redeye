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
// Class definition for Player.h and definitions of its functions.

#include <iostream>
#include "raylib.h"
#include "box2d/box2d.h"
#include "Player.h"
#include "../../Core/Utility/Logging.h"
#include "../../Core/UI/RectButton.h"
#include "../../Core/Utility/Utils.h"
#include "../../Core/Backend/LayerManager.h"
#include "../../Application/Layers/DeathMenuLayer.h"
#include "../../Core/Utility/Globals.h"
#include "../../Core/Serialization/AnimationLoader.h"
#include "../../Core/Audio/AudioManager.h"

namespace RE::Core {
    void Player::moveRight(const b2WorldId& world) const {
        assert(b2Body_IsValid(m_body));
        assert(b2World_IsValid(world));

        // Calculate ground normals, adjust direction of force applied and amount of force applied based on ground angle
        // Maybe consolidate the following into a function since I use it twice?
        const float mass = b2Body_GetMass(m_body);
        float movementForce = mass * g_playerWalkMultiplier;
        b2Vec2 impulseNormals = {movementForce, 0.0f};

        if (isOnGround()) {
            const b2Vec2 groundNormals = getGroundNormals(world);

            b2Vec2 tangent = {groundNormals.y, -groundNormals.x};
            tangent = b2Normalize(tangent);
            const float slopeMagnitude = std::fabs(tangent.y);
            movementForce += slopeMagnitude * g_slopeForceMultiplier;

            impulseNormals = b2MulSV(-movementForce, tangent);
        }

        // Physically move player
        b2Body_ApplyLinearImpulse(
            m_body,
            impulseNormals,
            b2Body_GetWorldCenterOfMass(m_body),
            true);
    }

    void Player::moveLeft(const b2WorldId& world) const {
        assert(b2Body_IsValid(m_body));
        assert(b2World_IsValid(world));

        const float mass = b2Body_GetMass(m_body);
        float movementForce = mass * g_playerWalkMultiplier;
        b2Vec2 impulseNormals = {-movementForce, 0.0f};

        if (isOnGround()) {
            const b2Vec2 groundNormals = getGroundNormals(world);

            b2Vec2 tangent = {groundNormals.y, -groundNormals.x};
            tangent = b2Normalize(tangent);
            const float slopeMagnitude = std::fabs(tangent.y);
            movementForce += slopeMagnitude * g_slopeForceMultiplier;

            impulseNormals = b2MulSV(movementForce, tangent);
        }

        b2Body_ApplyLinearImpulse(
            m_body,
            impulseNormals,
            b2Body_GetWorldCenterOfMass(m_body),
            true);
    }

    void Player::jump() const {
        const float mass = b2Body_GetMass(m_body);

        b2Body_ApplyLinearImpulse(
        m_body,
        {0.0f, -(mass * g_playerJumpMultiplier)},
        b2Body_GetWorldCenterOfMass(m_body),
        true);
    }

    // Consider returning the full result if other raycast info is needed down the line...
    [[nodiscard]] b2Vec2 Player::getGroundNormals(const b2WorldId& world) const {
        assert(b2Body_IsValid(m_body));

        const b2Vec2 origin = b2Body_GetPosition(m_body);
        const b2Vec2 endPoint = {origin.x, origin.y + 1.0f};
        const b2Vec2 translation = b2Sub(endPoint, origin);

        b2QueryFilter filter = b2DefaultQueryFilter();
        filter.categoryBits = g_raycastCategoryBits;
        filter.maskBits = g_groundCategoryBits;

        assert(b2World_IsValid(world));
        const b2RayResult result = b2World_CastRayClosest(world, origin, translation, filter);

        return result.normal;
    }

    Player::Player(
        const float centerX,
        const float centerY,
        const b2WorldId world,
        std::shared_ptr<AudioManager> manager) :
            m_playerAnimations(loadAnimations(g_playerAnimPath)),
            m_animationManager(
                g_playerSpritePath,
                m_playerAnimations,
                animationId::PLAYER_IDLE_RIGHT,
                std::move(manager)),
            m_playerSpritePath(g_playerSpritePath),
            m_currentDirection(direction::RIGHT),
            m_currentState(entityActionState::IDLE)
    {
        m_sizePx = m_animationManager.getSpriteSize();
        m_sizeMeters = pixelsToMetersVec(m_sizePx);
        m_centerPosition = {pixelsToMeters(centerX), pixelsToMeters(centerY)};
        m_cornerPosition = {centerX - m_sizePx.x / 2.0f, centerY - m_sizePx.y / 2.0f};

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

    Player::~Player() {
        #ifdef DEBUG
            logDbg("Player destroyed at address: ", this);
        #endif

        if (b2Body_IsValid(m_body))
            b2DestroyBody(m_body);
    }

    void Player::pollEvents() {
        if (IsKeyDown(KEY_D))
            m_movementIntent = 1;
        else if (IsKeyDown(KEY_A))
            m_movementIntent = -1;
        else
            m_movementIntent = 0;

        if (IsKeyDown(KEY_SPACE))
            m_jumpIntent = true;
    }

    void Player::update(const b2WorldId& world) {
        assert(b2Body_IsValid(m_body));

        const float velocityY = b2Body_GetLinearVelocity(m_body).y;

        if (isOnGround() && m_jumpIntent)
            jump();

        m_jumpIntent = false;

        if (m_movementIntent > 0) {
            m_currentDirection = direction::RIGHT;
            moveRight(world);
        }
        else if (m_movementIntent < 0) {
            m_currentDirection = direction::LEFT;
            moveLeft(world);
        }

        if (!isOnGround()) {
            m_currentState = velocityY < 0.0f ?
                entityActionState::JUMPING :
                entityActionState::FALLING;
        }
        else {
            m_currentState = m_movementIntent != 0 ?
                entityActionState::WALKING :
                entityActionState::IDLE;
        }

        m_movementIntent = 0;

        m_centerPosition = b2Body_GetPosition(m_body);
        m_cornerPosition = {
            metersToPixels(m_centerPosition.x) - m_sizePx.x / 2,
            metersToPixels(m_centerPosition.y) - m_sizePx.y / 2
        };

        m_animationManager.updateAnimation(m_currentState, m_currentDirection);
        m_currentAnimId = m_animationManager.getCurrentAnimId();
    }

    void Player::draw() const {
        m_animationManager.drawAnimation(m_cornerPosition);
    }

    void Player::murder() {
        LayerManager::getInstance().suspendLayer(layerKey::GAME_LAYER);
        LayerManager::getInstance().suspendOverlays();

        // "How the fuck does this shit even compile?!" -Some loser on Reddit, 2025
        try {
            const auto p = shared_from_this();

            if (!LayerManager::getInstance().stackContains(layerKey::DEATH_MENU)) {
                LayerManager::getInstance().pushLayer(
               layerKey::DEATH_MENU,
               std::make_unique<Application::DeathMenuLayer>(p));
            }
            else {
                LayerManager::getInstance().resumeLayer(layerKey::DEATH_MENU);
            }
        }
        catch (std::bad_weak_ptr& e) {
            logFatal(std::string("Error using shared from this: ") +
                std::string(e.what()) + std::string(". Player::murder()"));

            return;
        }
        catch (...) {
            logFatal("Unknown error occurred using shared_from_this(). Player::murder()");
            return;
        }

        m_dead = true;
    }

    void Player::reform(const saveData& save) {
        assert(b2Body_IsValid(m_body));

        LayerManager::getInstance().resumeLayer(layerKey::GAME_LAYER);
        LayerManager::getInstance().resumeOverlays();
        LayerManager::getInstance().requestLayerPop(layerKey::DEATH_MENU);

        b2Body_SetTransform(
            m_body,
            save.centerPosition,
            b2MakeRot(0.0f));

        m_dead = false;
    }

    void Player::addContactEvent() noexcept {
        m_activeGroundContacts++;
    }

    void Player::removeContactEvent() noexcept {
        m_activeGroundContacts--;
    }

    [[nodiscard]] sensorInfo Player::getFootpawSensorInfo() const noexcept {
        assert(m_footpawSensorInfo);

        return *m_footpawSensorInfo;
    }

    [[nodiscard]] b2ShapeId Player::getFootpawSenorId() const noexcept {
        return m_footpawSensorId;
    }

    [[nodiscard]] bool Player::isOnGround() const noexcept {
        return m_activeGroundContacts > 0;
    }

    [[nodiscard]] bool Player::isDead() const noexcept {
        return m_dead;
    }

    [[nodiscard]] direction Player::getPlayerDirection() const noexcept {
        return m_currentDirection;
    }

    [[nodiscard]] animationId Player::getCurrentAnimId() const noexcept {
        return m_currentAnimId;
    }

    [[nodiscard]] entityActionState Player::getCurrentActionState() const noexcept {
        return m_currentState;
    }
}