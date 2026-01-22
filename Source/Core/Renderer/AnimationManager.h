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

    ~AnimationManager();

    // Templating here to get around the fact the value of N must be declared for arrays...
    template<std::size_t N>
    AnimationManager(
        const std::string& spritePath,
        const std::array<animationDescriptor, N>& anims,
        const animationId startingAnim)
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
