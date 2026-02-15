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
#include "EntityAnimation.h"
#include "../Utility/Logging.h"
#include "../Audio/AudioManager.h"

namespace RE::Core {
    static animationId resolveAnimationId(
    const entityActionState& state,
    const direction& dir);

    class EntityAnimationManager {
        std::map<animationId, std::unique_ptr<EntityAnimation>> m_anims{};
        std::map<std::string, std::shared_ptr<Texture2D>> m_animTexs{};
        std::shared_ptr<AudioManager> m_audioManager{};
        animationId m_prevAnimId{};
        animationId m_curAnimId{};
    public:
        EntityAnimationManager() {
            #ifdef DEBUG
                logDbg("Default AnimationManager created at address: ", this);
            #endif
        }

        // TODO: Rethink how all this is constructed, should probably have push/pop functions
        EntityAnimationManager(
        const std::string& spritePath,
        std::vector<std::unique_ptr<animationDescriptor>>& anims,
        const animationId& startingAnim,
        std::shared_ptr<AudioManager> manager);

        ~EntityAnimationManager();

        EntityAnimationManager(const EntityAnimationManager&) = delete;
        EntityAnimationManager(EntityAnimationManager&& other) noexcept;
        EntityAnimationManager& operator=(const EntityAnimationManager&) = delete;
        EntityAnimationManager& operator=(EntityAnimationManager&& other) noexcept;

        void updateAnimation(
            const entityActionState& state,
            const direction& dir);

        void drawAnimation(Vector2 drawPos) const;

        [[nodiscard]] animationId getCurrentAnimId() const noexcept;
        [[nodiscard]] Vector2 getSpriteSize() const noexcept;
    };
}

#endif //ANIMATIONMANAGER_H
