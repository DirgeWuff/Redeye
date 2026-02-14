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
// Class definition for AnimationManager.h and its member functions.

#include <cassert>
#include "ranges"
#include "AnimationManager.h"

namespace RE::Core {
    // Any new anims/sprites on the sprite sheet will have to be added here to function...
    static animationId resolveAnimationId(const entityActionState& state, const direction& dir) {
        static constexpr animationId animationTable[4][2] = {
            {animationId::PLAYER_IDLE_RIGHT, animationId::PLAYER_IDLE_LEFT},
            {animationId::PLAYER_WALK_RIGHT, animationId::PLAYER_WALK_LEFT},
            {animationId::PLAYER_JUMP_RIGHT, animationId::PLAYER_JUMP_LEFT},
            {animationId::PLAYER_FALL_RIGHT, animationId::PLAYER_FALL_LEFT}
        };

        return animationTable[static_cast<std::uint8_t>(state)][static_cast<std::uint8_t>(dir)];
    }

    AnimationManager::AnimationManager(
        const std::string& spritePath,
        std::vector<std::unique_ptr<animationDescriptor>>& anims,
        const animationId& startingAnim,
        std::shared_ptr<AudioManager> manager) :
            m_audioManager(std::move(manager))
    {
        for (auto& anim : anims) {
            try {
                std::shared_ptr<Texture2D> tex;
                const auto it = m_animTexs.find(spritePath); // NOLINT

                if (it != m_animTexs.end()) {
                    tex = it->second;
                }
                else {
                    tex = std::make_shared<Texture2D>(LoadTexture(spritePath.c_str()));
                    if (!IsTextureValid(*tex)) {
                        logFatal(std::string("Failed to load texture: ") + spritePath);
                        return;
                    }

                    m_animTexs.emplace(spritePath, tex);
                }

                // Try derived FIRST because dynamic_cast to base will always be valid...
                if (auto* keyframeDesc = dynamic_cast<keyframeSoundDescriptor*>(anim.get())) {
                    auto newKeyframeAnim = std::make_unique<KeyframeSoundAnim>(tex, *keyframeDesc, m_audioManager);

                    m_anims.emplace(
                        anim->id,
                        std::move(newKeyframeAnim));
                }
                else if (auto* transitionDesc = dynamic_cast<transitionSoundDescriptor*>(anim.get())) {
                    auto newTransitionAnim = std::make_unique<TransitionSoundAnim>(tex, *transitionDesc, m_audioManager);

                    m_anims.emplace(
                        anim->id,
                        std::move(newTransitionAnim));
                }
                else if (auto* animDesc = anim.get()) {
                    auto newAnim = std::make_unique<Animation>(tex, *animDesc);

                    m_anims.emplace(
                        anim->id,
                        std::move(newAnim));
                }
            }
            catch (const std::exception& e) {
                logFatal(std::string("Construction failed: ") + std::string(e.what()) +
                    std::string(". AnimationManager::AnimationManager(Args...)"));

                return;
            }
            catch (...) {
                logFatal("An unknown error has occurred: AnimationManager::AnimationManager(Args...)");
                return;
            }
        }

        const auto it = m_anims.find(startingAnim);

        if (it == m_anims.end()) {
            logFatal(std::string("Cannot find specified starting anim in provided animation array: ") +
                animIdToStr(startingAnim));

            return;
        }

        m_curAnimId = it->first;

        #ifdef DEBUG
            logDbg("AnimationManager constructed at address: ", this);
        #endif
    }

    AnimationManager::~AnimationManager() {
        for (const auto& texture : std::views::values(m_animTexs)) {
            assert(texture);
            assert(IsTextureValid(*texture));

            UnloadTexture(*texture);
        }

    #ifdef DEBUG
        logDbg("AnimationManager destroyed at address: ", this);
    #endif
    }

    AnimationManager::AnimationManager(AnimationManager&& other) noexcept :
        m_anims(std::move(other.m_anims)),
        m_animTexs(std::move(other.m_animTexs)),
        m_audioManager(std::move(other.m_audioManager)),
        m_prevAnimId(other.m_prevAnimId),
        m_curAnimId(other.m_curAnimId)
    {
        #ifdef DEBUG
            logDbg("Move called on AnimationManager, new address: ", this);
        #endif
    }

    AnimationManager& AnimationManager::operator=(AnimationManager&& other) noexcept {
        if (this != &other) {
            this->m_anims = std::move(other.m_anims);
            this->m_animTexs = std::move(other.m_animTexs);
            this->m_audioManager = std::move(other.m_audioManager);
            this->m_prevAnimId = other.m_prevAnimId;
            this->m_curAnimId = other.m_curAnimId;
        }

        #ifdef DEBUG
            logDbg("Move assignment called on AnimationManager, new address: ", this);
        #endif

        return *this;
    }

    void AnimationManager::updateAnimation(
        const entityActionState& state,
        const direction& dir)
    {
        const animationId id = resolveAnimationId(state, dir); // NOLINT

        // Animation switch occurs...
        if (id != m_curAnimId) {
            if (m_anims.at(m_curAnimId)->getType() == animType::TRANSITION_SOUND) {
                m_anims.at(m_curAnimId)->onEnd();
            }

            const auto it = m_anims.find(id);

            if (it == m_anims.end()) {
                logFatal(std::string("No such animation in animation stack: " + animIdToStr(id)));
                return;
            }

            it->second->resetAnimation();
            m_prevAnimId = m_curAnimId;
            m_curAnimId = it->first;

            m_anims.at(m_curAnimId)->update();
        }
        else {
            m_anims.at(m_curAnimId)->update();
        }
    }

    void AnimationManager::drawAnimation(const Vector2 drawPos) const {
        m_anims.at(m_curAnimId)->draw(drawPos);
    }

    [[nodiscard]] animationId AnimationManager::getCurrentAnimId() const noexcept {
        return m_curAnimId;
    }

    // Should be same for all sprites on sheet... Might be an unnecessarily complex way to do this.
    [[nodiscard]] Vector2 AnimationManager::getSpriteSize() const noexcept {
        const auto it = m_anims.find(m_curAnimId); // NOLINT

        if (it != m_anims.end()) {
           return it->second->m_spriteRes;
        }

        return{};
    }
}