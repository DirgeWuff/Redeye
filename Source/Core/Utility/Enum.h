//
// Created by DirgeWuff on 1/20/2026.
//

#ifndef ENUM_H
#define ENUM_H

#include <cstdint>
#include <string>

// State of a button as it relates to interaction from user
enum class buttonStates {
    BUTTON_IDLE,
    BUTTON_HOVERED,
    BUTTON_CLICKED
};

// First vs second render pass
enum class renderPassType {
    PRIMARY_PASS,
    DIFFERED_PASS
};

// Type of layer. Whether or not the layer can be drawn on top of another layer (partially transparent)
enum class layerType : std::uint8_t {
    PRIMARY_LAYER,
    OVERLAY_LAYER
};

// Names/keys of individual layers
enum class layerKey : std::uint8_t {
    START_MENU,
    GAME_LAYER,
    DEATH_MENU,
    CHECKPOINT_ALERT
};

// Basic directions
enum class direction : std::uint8_t {
    RIGHT,
    LEFT
};

// Animation playback "mode"
enum class playbackType : std::uint8_t {
    SINGLE_FRAME,
    NON_LOOPING,
    LOOP
};

// Entity state, can dictate correct animation along with direction
enum class entityActionState : std::uint8_t {
    IDLE,
    WALKING,
    JUMPING,
    FALLING
};

// Names of individual animations
enum class animationId : std::uint8_t {
    PLAYER_IDLE_RIGHT,
    PLAYER_IDLE_LEFT,
    PLAYER_WALK_RIGHT,
    PLAYER_WALK_LEFT,
    PLAYER_JUMP_RIGHT,
    PLAYER_JUMP_LEFT,
    PLAYER_FALL_RIGHT,
    PLAYER_FALL_LEFT
};

std::string layerKeyToStr(const layerKey& key);
std::string dirToStr(const direction& dir);
std::string stateToStr(const entityActionState& state);
std::string animIdToStr(const animationId& id);

#endif //ENUM_H
