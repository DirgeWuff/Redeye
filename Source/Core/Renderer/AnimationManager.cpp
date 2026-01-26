//
// Created by DirgeWuff on 1/19/2026.
//

#include <cassert>
#include "ranges"
#include "AnimationManager.h"

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
    const std::vector<animationDescriptor>& anims,
    const animationId& startingAnim)
{
    for (auto& anim : anims) {
        try {
            std::shared_ptr<Texture2D> tex;
            const auto it = m_animTexs.find(spritePath);

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

            m_anims.try_emplace(
               anim.id,

                anim.start,
                anim.end,
                tex,
                anim.spriteRes,
                anim.frameDuration,
                anim.type,
                anim.id);
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

void AnimationManager::updateAnimation(
    const entityActionState& state,
    const direction& dir) noexcept
{
    const animationId id = resolveAnimationId(state, dir);

    if (id != m_curAnimId) {
        const auto it = m_anims.find(id);

        if (it == m_anims.end()) {
            logFatal(std::string("No such animation in animation stack: " + animIdToStr(id)));
            return;
        }

        it->second.resetAnimation();
        m_prevAnimId = m_curAnimId;
        m_curAnimId = it->first;

        m_anims.at(m_curAnimId).update();
    }
    else {
        m_anims.at(m_curAnimId).update();
    }
}

void AnimationManager::drawAnimation(const Vector2 drawPos) const {
    m_anims.at(m_curAnimId).draw(drawPos);
}

[[nodiscard]] animationId AnimationManager::getCurrentAnimId() const noexcept {
    return m_curAnimId;
}

// Should be same for all sprites on sheet... Might be an unnecessarily complex way to do this.
[[nodiscard]] Vector2 AnimationManager::getSpriteSize() const noexcept {
    const auto it = m_anims.find(m_curAnimId);

    if (it != m_anims.end()) {
       return it->second.m_spriteRes;
    }

    return{};
}
