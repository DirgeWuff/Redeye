//
// Created by DirgeWuff on 1/19/2026.
//

#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <map>
#include <string>
#include <memory>

#include "raylib.h"

#include "Animation.h"
#include "../Utility/Logging.h"

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
    const std::vector<animationDescriptor>& anims,
    const animationId& startingAnim);

    ~AnimationManager();


    AnimationManager(const AnimationManager&) = delete;
    AnimationManager(AnimationManager&&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;
    AnimationManager& operator=(AnimationManager&&) = delete;

    void updateAnimation(
        const entityActionState& state,
        const direction& dir) noexcept;

    void drawAnimation(Vector2 drawPos) const;

    [[nodiscard]] animationId getCurrentAnimId() const noexcept;
    [[nodiscard]] Vector2 getSpriteSize() const noexcept;
};

#endif //ANIMATIONMANAGER_H
