//
// Author: DirgeWuff
// Created on: 1/5/26
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Enums defining multiple aspects of the game, with related functions for
// translating enums into strings and safely converting an enum's underlying type
// back into it's original type.

#ifndef ENUM_H
#define ENUM_H

#include <cstdint>
#include <string>
#include <type_traits>
#include <optional>
#include "Logging.h"

// State of a button as it relates to interaction from user
enum class buttonStates : std::uint8_t {
    BUTTON_IDLE,
    BUTTON_HOVERED,
    BUTTON_CLICKED,
    COUNT // Serves as marker for enum end
};

// First vs second render pass
enum class renderPassType : std::uint8_t {
    PRIMARY_PASS,
    DIFFERED_PASS,
    COUNT
};

// Type of layer. Whether or not the layer can be drawn on top of another layer (partially transparent)
enum class layerType : std::uint8_t {
    PRIMARY_LAYER,
    OVERLAY_LAYER,
    COUNT
};

// Names/keys of individual layers
enum class layerKey : std::uint8_t {
    START_MENU,
    GAME_LAYER,
    DEATH_MENU,
    CHECKPOINT_ALERT,
    COUNT
};

// Basic directions
enum class direction : std::uint8_t {
    RIGHT,
    LEFT,
    COUNT
};

// Animation playback "mode"
enum class playbackType : std::uint8_t {
    SINGLE_FRAME,
    NON_LOOPING,
    LOOP,
    COUNT
};

// IDs of subscription objects, used by EventDispatcher<T>
enum class subId : std::uint8_t {
    PLAYER_FOOTPAW_GROUND_CONTACT,
    PLAYER_DEATH_COLLIDER_CONTACT,
    PLAYER_CHECKPOINT_COLLIDER_CONTACT,
    COUNT
};

// Types of event colliders/sensor objects
enum class sensorType : std::uint8_t {
    PLAYER_FOOTPAW_SENSOR,
    MURDER_BOX,
    CHECKPOINT,
    COUNT
};

// Explicitly defining these for my own sanity since they appear in the animation toml...

// Entity state, can dictate correct animation along with direction
enum class entityActionState : std::uint8_t {
    IDLE = 0,
    WALKING = 1,
    JUMPING = 2,
    FALLING = 3,
    COUNT = 4
};

// Names of individual animations
enum class animationId : std::uint8_t {
    PLAYER_IDLE_RIGHT = 0,
    PLAYER_IDLE_LEFT = 1,
    PLAYER_WALK_RIGHT = 2,
    PLAYER_WALK_LEFT = 3,
    PLAYER_JUMP_RIGHT = 4,
    PLAYER_JUMP_LEFT = 5,
    PLAYER_FALL_RIGHT = 6,
    PLAYER_FALL_LEFT = 7,
    COUNT = 8
};


std::string layerKeyToStr(const layerKey& key);
std::string dirToStr(const direction& dir);
std::string stateToStr(const entityActionState& state);
std::string animIdToStr(const animationId& id);
std::string sensorToStr(const sensorType& type);
animationId strToAnimId(const std::string& str);


template<typename E>
constexpr std::optional<E> toEnum(std::underlying_type_t<E> v) {
    // Make sure it's an enum...
    static_assert(std::is_enum_v<E>);

    // Make sure we're in bounds, get underlying enum type, cast back to enum
    using UT = std::underlying_type_t<E>;
    if (v < static_cast<UT>(E::COUNT))
        return static_cast<E>(v);

    logDbg("v > E::COUNT. toEnum(Args...)");
    return std::nullopt;
}

#endif //ENUM_H
