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
