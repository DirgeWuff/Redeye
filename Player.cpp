//
// Created by DirgeWuff on 4/29/25.
//

#include "Player.h"
#include "Entity.h"
#include "raymath.h"

Player::Player(std::string spritePath) :
position{501.0f, 400.0f}, // here as a placeholder until I have terrain
numFrames(3),
frameIndex(0),
frameDelayAmount(5),
frameDelayClock(0),
moving(false),
onGround(true),
movementSpeed(4.5f),
lastDirection(0),
groundPos(510),
velocity{0, 0}
{
    walkSprites = LoadTexture(spritePath.c_str());
    spriteRect = {
        0.0f,
        static_cast<float>(walkSprites.height) / 2,
        static_cast<float>(walkSprites.width) / 3,
        static_cast<float>(walkSprites.height) / 2};

    // Probably redundant, don't feel like fixing now
    width = walkSprites.width / 3;
    height = walkSprites.height / 2;
}

Player::~Player() {
    UnloadTexture(walkSprites);
}

void Player::update() {

    // Update position
    position = Vector2Add(position, velocity);

    if (position.y >= groundPos) {
        onGround = true;
    }

    if (onGround) {
        position.y = groundPos;
    }
    else { // Make player fall down to earth if in air
        velocity.y += 1;
    }
}

void Player::draw() const {
    DrawTextureRec(walkSprites, spriteRect, position, WHITE);
}

void Player::moveRight() {
    moving = true;
    lastDirection = 1;
    velocity.x = movementSpeed;
    if (frameDelayClock >= frameDelayAmount) {
        frameIndex++;
        spriteRect.y = static_cast<float>(walkSprites.height) / 2;
        spriteRect.x = static_cast<float>(width) * static_cast<float>(frameIndex);
        frameDelayClock = 0;
    }
    else {
        frameDelayClock++;
    }
}

void Player::moveLeft() {
    moving = true;
    lastDirection = 0;
    velocity.x = -movementSpeed;
    if (frameDelayClock >= frameDelayAmount) {
        frameIndex++;
        spriteRect.y = 0.0f;
        spriteRect.x = static_cast<float>(width) * static_cast<float>(frameIndex);
        frameDelayClock = 0;
    }
    else {
        frameDelayClock++;
    }
}

void Player::jump() {
    if (onGround) {
        velocity.y -= 6.0f * movementSpeed;
        onGround = false;
    }
}

void Player::moveNowhere() {
    moving = false;
    velocity.x = 0.0f;

    if (lastDirection == 0) {
        spriteRect.x = static_cast<float>(walkSprites.width) / 3;
    }
    else {
        spriteRect.y = static_cast<float>(walkSprites.height) / 2;
        spriteRect.x = static_cast<float>(walkSprites.width) / 3;
    }
}

float Player::getX() const {
    const float output = position.x;
    return output;
}

float Player::getY() const {
    const float output = position.y;
    return output;
}

float Player::getWidth() const {
    const float output = width;
    return output;
}

float Player::getHeight() const {
    const float output = height;
    return output;
}
