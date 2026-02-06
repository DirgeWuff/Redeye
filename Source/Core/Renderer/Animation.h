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
// Animation class declaration, defines a data structure and functions to create an
// animated sprite, which is loaded from a sprite sheet.

#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <cstdint>
#include "raylib.h"
#include "../Utility/Enum.h"

namespace RE::Core {
    class AudioManager;
    class AnimationManager;

    struct spriteIndex {std::size_t x; std::size_t y;};

    struct animationDescriptor {
        std::optional<std::vector<std::uint8_t>> soundFrames;
        spriteIndex start;
        spriteIndex end;
        Vector2 spriteRes;
        float frameDuration;
        std::optional<std::uint8_t> soundId;
        animPlaybackType type;
        animationId id;
    };

    // TODO: Add support for animations that span multiple rows on the sprite sheet if that becomes a thing
    class Animation {
        friend class AnimationManager;

        std::optional<std::vector<std::uint8_t>> m_soundFrames{};
        std::vector<Vector2> m_frameIndices{};
        std::shared_ptr<Texture2D> m_texture{};
        Rectangle m_sourceRect{};
        Vector2 m_spriteRes{};
        std::size_t m_frameCount{};
        float m_frameDuration{};
        float m_elapsedFrameTime{};
        std::optional<soundId> m_soundId{};
        std::uint8_t m_curIdx{};
        animPlaybackType m_animType{};
        animationId m_animId{};
        bool m_animationFinished{};
    public:
        Animation();
        Animation(
            std::shared_ptr<Texture2D> tex,
            const animationDescriptor& desc);

        ~Animation();

        Animation(const Animation&) = delete;
        Animation(Animation&& other) noexcept;
        Animation& operator=(const Animation&) = delete;
        Animation& operator=(Animation&& other) noexcept;

        void resetAnimation() noexcept;
        void update(AudioManager& audManager);
        void draw(Vector2 drawPos) const noexcept;
    };
}

#endif //ANIMATION_H
