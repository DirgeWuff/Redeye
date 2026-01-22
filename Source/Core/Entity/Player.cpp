//
// Created by DirgeWuff on 4/29/25.
//

// NOTE: m_frameDelayAmount, and linear impulse forces reduced by
// around half with new layer system because player was suddenly
// moving too fast. Hopefully this isn't covering up a deeper bug
// that I am unaware of...

#include <iostream>
#include "box2d/box2d.h"
#include "Player.h"
#include "../../Core/Utility/Logging.h"
#include "../../Core/UI/UI.h"
#include "../../Core/Audio/Audio.h"
#include "../../Core/Utility/Utils.h"
#include "../../Core/Backend/LayerManager.h"
#include "../../Application/Layers/DeathMenuLayer.h"
#include "../../Core/Utility/Globals.h"

Player::~Player() {
    #ifdef DEBUG
        logDbg("Player destroyed at address: ", this);
    #endif

    if (b2Body_IsValid(m_body))
        b2DestroyBody(m_body);

    unloadSoundVector(m_footstepSounds);
    unloadSoundVector(m_landingSounds);
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

void Player::murder() {
    LayerManager::getInstance().suspendLayer(layerKey::GAME_LAYER);
    LayerManager::getInstance().suspendOverlays();

    // Pretty bad creating unique_ptr<T> with shared_ptr<T> as argument, but it works better than anything else...
    try {
        const auto p = shared_from_this();

        if (!LayerManager::getInstance().stackContains(layerKey::DEATH_MENU)) {
            LayerManager::getInstance().pushLayer(
           layerKey::DEATH_MENU,
           std::make_unique<DeathMenuLayer>(p));
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

void Player::addContactEvent() noexcept {
    m_activeGroundContacts++;
}

void Player::removeContactEvent() noexcept {
    m_activeGroundContacts--;
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
