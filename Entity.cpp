//
// Created by DirgeWuff on 5/9/25.
//

#include "Entity.h"

Entity::Entity() = default;

Entity::~Entity() = default;

bool Entity::update(const bool &onGround) {
    return true;
}

float Entity::getX() const {
    constexpr float output = 0.0f;
    return output;
}

float Entity::getY() const {
    constexpr float output = 0.0f;
    return output;
}
