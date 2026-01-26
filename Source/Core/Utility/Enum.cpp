//
// Created by DirgeWuff on 1/20/2026.
//

#include "Enum.h"

std::string layerKeyToStr(const layerKey& key) {
    switch (key) {
        case layerKey::START_MENU: return "START_MENU";
        case layerKey::GAME_LAYER: return "GAME_LAYER";
        case layerKey::DEATH_MENU: return "DEATH_MENU";
        default: return "Unknown key type";
    }
}

std::string dirToStr(direction& dir) {
    switch (dir) {
        case direction::RIGHT: return "RIGHT";
        case direction::LEFT: return "LEFT";
        default: return "No such direction exists";
    }
}

std::string stateToStr(const entityActionState& state) {
    switch (state) {
        case entityActionState::IDLE: return "IDLE";
        case entityActionState::WALKING: return "WALKING";
        case entityActionState::JUMPING: return "JUMPING";
        case entityActionState::FALLING: return "FALLING";
        default: return "No such animation state exists";
    }
}

std::string animIdToStr(const animationId& id) {
    switch (id) {
        case animationId::PLAYER_IDLE_RIGHT: return "PLAYER_IDLE_RIGHT";
        case animationId::PLAYER_IDLE_LEFT: return "PLAYER_IDLE_LEFT";
        case animationId::PLAYER_WALK_RIGHT: return "PLAYER_WALK_RIGHT";
        case animationId::PLAYER_WALK_LEFT: return "PLAYER_WALK_LEFT";
        case animationId::PLAYER_JUMP_RIGHT: return "PLAYER_JUMP_RIGHT";
        case animationId::PLAYER_JUMP_LEFT: return "PLAYER_JUMP_LEFT";
        case animationId::PLAYER_FALL_RIGHT: return "PLAYER_FALL_RIGHT";
        case animationId::PLAYER_FALL_LEFT: return "PLAYER_FALL_LEFT";
        default: return std::string("No such animation key exists");
    }
}
