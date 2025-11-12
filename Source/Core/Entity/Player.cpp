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
#include "../../Core/Utility/Error.h"
#include "../../Core/UI/UI.h"
#include "../../Core/Audio/Audio.h"
#include "../../Core/Utility/Utils.h"
#include "../../Core/Backend/LayerManager.h"
#include "../../Application/Layers/DeathMenuLayer.h"
#include "../../Core/Utility/Globals.h"

Player::~Player() {
#ifdef DEBUG
    std::cout << "Player object destroyed at address: " << this << std::endl;
#endif

    if (b2Body_IsValid(m_body))
        b2DestroyBody(m_body);

    UnloadTexture(m_walkSprites);
    unloadSoundVector(m_footstepSounds);
    unloadSoundVector(m_landingSounds);
}

void Player::update() {
    assert(b2Body_IsValid(m_body));

    m_centerPosition = b2Body_GetPosition(m_body);
    m_cornerPosition = {
        metersToPixels(m_centerPosition.x) - m_sizePx.x / 2,
        metersToPixels(m_centerPosition.y) - m_sizePx.y / 2
    };
    if (m_activeGroundContacts > 0) {
        if (m_currentState != playerStates::ON_GROUND) {
            m_currentState = playerStates::ON_GROUND;
            playRandomSound(m_landingSounds);
        }
    }
    else {
         m_currentState = playerStates::IN_AIR;
    }
}

void Player::draw() const {
    DrawTextureRec(m_walkSprites, m_spriteRect, m_cornerPosition, WHITE);
}

void Player::moveRight(const b2WorldId& world) {
    assert(b2Body_IsValid(m_body));
    assert(b2World_IsValid(world));

    // Calculate ground normals, adjust direction of force applied and amount of force applied based on ground angle
    const float mass = b2Body_GetMass(m_body);
    float movementForce = mass * 0.30f;
    b2Vec2 impulseNormals = {movementForce, 0.0f};

    if (m_currentState == playerStates::ON_GROUND) {
        const b2Vec2 groundNormals = getGroundNormals(world);

        b2Vec2 tangent = {groundNormals.y, -groundNormals.x};
        tangent = b2Normalize(tangent);
        const float slopeMagnitude = std::fabs(tangent.y);
        movementForce += slopeMagnitude;

        impulseNormals = b2MulSV(-movementForce, tangent);
    }

    m_lastDirection = directions::RIGHT;

    // Physically move player
    b2Body_ApplyLinearImpulse(
        m_body,
        impulseNormals,
        b2Body_GetWorldCenterOfMass(m_body),
        true);

    // Switch sprites and play footstep sounds
    if (m_currentState == playerStates::ON_GROUND) {
        if (m_frameDelayClock >= m_frameDelayAmount) {
            m_frameIndex++;
            m_spriteRect.y = static_cast<float>(m_walkSprites.height) / 2;
            m_spriteRect.x = m_sizePx.x * static_cast<float>(m_frameIndex);
            m_frameDelayClock = 0;
        }
        else {
            m_frameDelayClock++;
        }

        if (m_soundDelayClock >= m_soundDelayAmount) {
            playRandomSound(m_footstepSounds);
            m_soundDelayClock = 0;
        }
        else {
            m_soundDelayClock++;
        }
    }
    else {
        m_spriteRect.y = static_cast<float>(m_walkSprites.height) / 2;
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
}

void Player::moveLeft(const b2WorldId& world) {
    assert(b2Body_IsValid(m_body));
    assert(b2World_IsValid(world));

    const float mass = b2Body_GetMass(m_body);
    float movementForce = mass * 0.30f;
    b2Vec2 impulseNormals = {-movementForce, 0.0f};

    if (m_currentState == playerStates::ON_GROUND) {
        const b2Vec2 groundNormals = getGroundNormals(world);

        b2Vec2 tangent = {groundNormals.y, -groundNormals.x};
        tangent = b2Normalize(tangent);
        const float slopeMagnitude = std::fabs(tangent.y);
        movementForce += slopeMagnitude;

        impulseNormals = b2MulSV(movementForce, tangent);
    }

    m_lastDirection = directions::LEFT;

    b2Body_ApplyLinearImpulse(
        m_body,
        impulseNormals,
        b2Body_GetWorldCenterOfMass(m_body),
        true);

    if (m_currentState == playerStates::ON_GROUND) {
        if (m_frameDelayClock >= m_frameDelayAmount) {
            m_frameIndex++;
            m_spriteRect.y = 0.0f;
            m_spriteRect.x = m_sizePx.x * static_cast<float>(m_frameIndex);
            m_frameDelayClock = 0;
        }
        else {
            m_frameDelayClock++;
        }

        if (m_soundDelayClock >= m_soundDelayAmount) {
            playRandomSound(m_footstepSounds);
            m_soundDelayClock = 0;
        }
        else {
            m_soundDelayClock++;
        }
    }
    else {
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
}

void Player::jump() const {
    if (!b2Body_IsValid(m_body)) {
        logErr("m_body invalid: Player::jump()");
        return;
    }

    if (m_currentState == playerStates::ON_GROUND) {
        const float mass = b2Body_GetMass(m_body);

        b2Body_ApplyLinearImpulse(
        m_body,
        {0.0f, -(mass * 8.0f)},
        b2Body_GetWorldCenterOfMass(m_body),
        true);
    }
}

// Control player idle sprite
void Player::moveNowhere() {
    if (m_lastDirection == directions::RIGHT) {
        m_spriteRect.y = static_cast<float>(m_walkSprites.height) / 2;
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
    else {
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
}

void Player::murder() {
    LayerManager::getInstance().suspendLayer(layerKey::GAME_LAYER);
    LayerManager::getInstance().suspendOverlays();

    // Pretty bad created unique_ptr<T> with shared_ptr<T> as argument, but it works better than anything else...
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
        logErr(std::string("Error using shared from this: ") +
            std::string(e.what()) + std::string(". Player::murder()"));
        return;
    }
    catch (...) {
        logErr("Unknown error occurred using shared_from_this(). Player::murder()");
    }

    m_currentState = playerStates::DEAD;
}

void Player::reform(const saveData& save) {
    if (!b2Body_IsValid(m_body)) {
        logErr("m_body invalid: Player::reform()");
        return;
    }

    LayerManager::getInstance().resumeLayer(layerKey::GAME_LAYER);
    LayerManager::getInstance().resumeOverlays();
    LayerManager::getInstance().requestLayerPop(layerKey::DEATH_MENU);

    b2Body_SetTransform(
        m_body,
        save.centerPosition,
        b2MakeRot(0.0f));

    m_currentState = playerStates::ON_GROUND;
}

[[nodiscard]] b2ShapeId Player::getFootpawSenorId() const noexcept {
    return m_footpawSensorId;
}

[[nodiscard]] bool Player::isOnGround() const noexcept {
    return m_currentState == playerStates::ON_GROUND;
}

[[nodiscard]] bool Player::isDead() const noexcept {
    return m_currentState == playerStates::DEAD;
}

[[nodiscard]] directions Player::getPlayerDirection() const noexcept {
    return m_lastDirection;
}

void Player::addContactEvent() noexcept {
    m_activeGroundContacts++;
}

void Player::removeContactEvent() noexcept {
    m_activeGroundContacts--;
}

// Consider returning the full result if other raycast info is needed down the line...
b2Vec2 Player::getGroundNormals(const b2WorldId& world) const {
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
