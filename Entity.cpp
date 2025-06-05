//
// Created by DirgeWuff on 5/9/25.
//

#include "Entity.h"
#include "raylib.h"

Entity::Entity() = default;

Entity::~Entity() = default;

bool Entity::update(const bool &onGround) {
    return true;
}

// This is kinda fucked and I should fix it at some point
void Entity::draw() {
    DrawText("Entity::draw() override", 0, 0, 25, RED);
}

float Entity::getX() const {
    constexpr float output = 0.0f;
    return output;
}

float Entity::getY() const {
    constexpr float output = 0.0f;
    return output;
}
