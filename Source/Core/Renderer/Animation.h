//
// Created by DirgeWuff on 1/5/2026.
//

#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <cstdint>

#include "raylib.h"

#include "../Utility/Logging.h"
#include "../Utility/Enum.h"

class AnimationManager;

struct spriteIndex {std::size_t x; std::size_t y;};

struct animationDescriptor {
    spriteIndex start;
    spriteIndex end;
    Vector2 spriteRes;
    float frameDuration;
    playbackType type;
    animationId id;
};

// TODO: Add support for animations that span multiple rows on the sprite sheet if that becomes a thing
class Animation {
    friend class AnimationManager;

    std::vector<Vector2> m_frameIndices{};
    std::shared_ptr<Texture2D> m_texture{};
    Vector2 m_spriteRes{};
    std::size_t m_frameCount{};
    Rectangle m_sourceRect{};
    float m_frameDuration{};
    float m_elapsedFrameTime{};
    std::uint8_t m_curIdx{};
    playbackType m_animType{};
    animationId m_animId{};
    bool m_animationFinished{};
public:
    Animation();
    ~Animation();

    // Bit of a mess of a constructor, but seems like the cleanest thing somehow...
    Animation(
        spriteIndex startingIndex,
        spriteIndex endingIndex,
        std::shared_ptr<Texture2D> tex,
        Vector2 spriteResolution,
        float frameDur,
        playbackType type,
        animationId id
    );

    Animation(const Animation&) = delete;
    Animation(Animation&) = delete;
    Animation& operator=(const Animation&) = delete;
    Animation& operator=(Animation&&) = delete;

    void resetAnimation() noexcept;
    void update() noexcept;
    void draw(Vector2 drawPos) const noexcept;
};


#endif //ANIMATION_H
