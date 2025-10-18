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
    if (!b2Body_IsValid(m_body)) {
        logErr("m_body invalid: Player::update()");
        return;
    }

    m_centerPosition = b2Body_GetPosition(m_body);
    m_cornerPosition = {
        metersToPixels(m_centerPosition.x) - m_sizePx.x / 2,
        metersToPixels(m_centerPosition.y) - m_sizePx.y / 2
    };
    if (m_activeGroundContacts > 0) {
        if (!m_onGround) {
            m_onGround = true;
            playRandomSound(m_landingSounds);
        }
    }
    else {
        m_onGround = false;
    }
}

void Player::draw() const {
    DrawTextureRec(m_walkSprites, m_spriteRect, m_cornerPosition, WHITE);
}

void Player::moveRight() {
    if (!b2Body_IsValid(m_body)) {
        logErr("m_body invalid: Player::moveRight()");
        return;
    }

    const float mass = b2Body_GetMass(m_body);
    m_lastDirection = RIGHT;

    // Physically move player
    b2Body_ApplyLinearImpulse(
        m_body,
        {mass * .30f, 0.0f},
        b2Body_GetWorldCenterOfMass(m_body),
        true);

    // Switch sprites and play footstep sounds
    if (m_onGround) {
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

void Player::moveLeft() {
    if (!b2Body_IsValid(m_body)) {
        logErr("m_body invalid: Player::moveLeft()");
        return;
    }

    const float mass = b2Body_GetMass(m_body);
    m_lastDirection = LEFT;

    b2Body_ApplyLinearImpulse(
        m_body,
        {-(mass * .30f), 0.0f},
        b2Body_GetWorldCenterOfMass(m_body),
        true);

    if (m_onGround) {
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

    if (m_onGround) {
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
    if (m_lastDirection == RIGHT) {
        m_spriteRect.y = static_cast<float>(m_walkSprites.height) / 2;
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
    else {
        m_spriteRect.x = static_cast<float>(m_walkSprites.width) / 3;
    }
}

void Player::murder() {
    LayerManager::getInstance().suspendLayer(std::string("GameLayer"));
    LayerManager::getInstance().suspendOverlays();

    // Pretty bad created unique_ptr<T> with shared_ptr<T> as argument, but it works better than anything else...
    try {
        const auto p = shared_from_this();

        if (!LayerManager::getInstance().stackContains("DeathMenuLayer")) {
            LayerManager::getInstance().pushLayer(
           std::string("DeathMenuLayer"),
           std::make_unique<DeathMenuLayer>(p));
        }
        else {
            LayerManager::getInstance().resumeLayer("DeathMenuLayer");
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

    m_dead = true;
}

void Player::reform() {
    if (!b2Body_IsValid(m_body)) {
        logErr("m_body invalid: Player::reform()");
        return;
    }

    LayerManager::getInstance().resumeLayer("GameLayer");
    LayerManager::getInstance().resumeOverlays();
    LayerManager::getInstance().requestLayerPop("DeathMenuLayer");

    b2Body_SetTransform(
        m_body,
        m_currentCheckpoint.centerPosition,
        b2MakeRot(0.0f));

    m_dead = false;
}

[[nodiscard]] b2ShapeId Player::getFootpawSenorId() const noexcept {
    return m_footpawSensorId;
}

[[nodiscard]] bool Player::getFootpawSensorStatus() const noexcept { // May be able to deprecate
    return m_onGround;
}

[[nodiscard]] bool Player::isDead() const noexcept {
    return m_dead;
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
