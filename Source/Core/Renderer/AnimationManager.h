//
// Author: DirgeWuff
// Created on: 1/19/26
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Animation object manager class declaration. Defines a structure/interface
// for the loading, switching, and drawing of animation objects. Responsible
// for determining an animation's current state/frame based on states passed
// to it.

#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <map>
#include <string>
#include <memory>
#include "raylib.h"
#include "Animation.h"
#include "../Utility/Logging.h"
#include "../Audio/AudioManager.h"

namespace RE::Core {
    static animationId resolveAnimationId(
    const entityActionState& state,
    const direction& dir);

    class AnimationManager {
        std::map<animationId, Animation> m_anims{};
        std::map<std::string, std::shared_ptr<Texture2D>> m_animTexs{};
        animationId m_prevAnimId{};
        animationId m_curAnimId{};
    public:
        AnimationManager() {
            #ifdef DEBUG
                logDbg("Default AnimationManager created at address: ", this);
            #endif
        }

        AnimationManager(
        const std::string& spritePath,
        std::vector<animationDescriptor>& anims,
        const animationId& startingAnim);

        ~AnimationManager();

        AnimationManager(const AnimationManager&) = delete;
        AnimationManager(AnimationManager&&) noexcept = delete;
        AnimationManager& operator=(const AnimationManager&) = delete;
        AnimationManager& operator=(AnimationManager&&) noexcept = delete;

        void updateAnimation(
            const entityActionState& state,
            const direction& dir,
            AudioManager& audManager);

        void drawAnimation(Vector2 drawPos) const;

        [[nodiscard]] animationId getCurrentAnimId() const noexcept;
        [[nodiscard]] Vector2 getSpriteSize() const noexcept;
    };
}

#endif //ANIMATIONMANAGER_H
