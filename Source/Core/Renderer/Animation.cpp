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
// Class definition of Animation.H

#include <cassert>
#include <iostream>
#include "Animation.h"
#include "../Audio/AudioManager.h"
#include "../Utility/Logging.h"

namespace RE::Core {
    Animation::Animation() {
        #ifdef DEBUG
            logDbg("Default Animation constructed at address: ", this);
        #endif
    }

    Animation::~Animation() {
        #ifdef DEBUG
            logDbg("Animation destroyed at address: ", this);
        #endif
    }

    Animation::Animation(
        const std::shared_ptr<Texture2D> tex, // NOLINT
        const animationDescriptor& desc
        ) :
            m_texture(tex),
            m_spriteRes(desc.spriteRes),
            m_frameDuration(desc.frameDuration),
            m_animType(desc.type),
            m_animId(desc.id)
    {
        assert(m_texture);
        assert(IsTextureValid(*m_texture));

        const std::size_t numFrames = desc.end.x - desc.start.x + 1;
        m_frameIndices.resize(numFrames);
        m_frameCount = numFrames - 1;

        // Generate first frame...
        m_frameIndices[0] = {
            static_cast<float>(desc.start.x - 1) * m_spriteRes.x,
            static_cast<float>(desc.start.y - 1) * m_spriteRes.y
        };

        // Then all other frames
        for (std::size_t i = 1; i < numFrames; i++) {
            const std::size_t sX = desc.start.x + i;
            const std::size_t sY = desc.start.y;

            m_frameIndices[i] = {
                static_cast<float>(sX - 1) * m_spriteRes.x,
                static_cast<float>(sY - 1) * m_spriteRes.y
            };
        }

        assert(!m_frameIndices.empty());

        m_sourceRect = {
            m_frameIndices[0].x,
            m_frameIndices[0].y,
            m_spriteRes.x,
            m_spriteRes.y
        };

        assert(m_frameIndices[m_frameCount].x < m_texture->width);
        assert(m_frameIndices[0].y < m_texture->height);

        if (desc.soundFrames.has_value() && desc.soundId.has_value()) {
            m_soundFrames = desc.soundFrames;
            m_soundId = toEnum<soundId>(desc.soundId.value());
        }
        else {
            m_soundFrames = std::nullopt;
            m_soundId = std::nullopt;
        }

        #ifdef DEBUG
            logDbg("Animation constructed at address: ", this);
        #endif
    }

    void Animation::resetAnimation() noexcept {
        m_elapsedFrameTime = 0.0f;
        m_curIdx = 0;
    }

    void Animation::update(AudioManager& audManager) {
        if (m_animationFinished) return;

        const float dt = GetFrameTime();
        m_elapsedFrameTime += dt;

        if (m_elapsedFrameTime >= m_frameDuration) {
            if (m_curIdx < m_frameCount && m_animType != animPlaybackType::SINGLE_FRAME) {
                m_curIdx++;
                m_elapsedFrameTime = 0.0f;

                if (m_soundFrames.has_value() && m_soundId.has_value()) {
                    const auto it =
                        std::ranges::find(m_soundFrames.value(), m_curIdx);

                    if (it != m_soundFrames->end()) {
                        audManager.playSound(m_soundId.value());
                    }
                }
            }
            else if (m_curIdx >= m_frameCount && m_animType == animPlaybackType::LOOP) {
                m_curIdx = 0;
                m_elapsedFrameTime = 0.0f;

                if (m_soundFrames.has_value() && m_soundId.has_value()) {
                    const auto it =
                        std::ranges::find(m_soundFrames.value(), m_curIdx);

                    if (it != m_soundFrames->end()) {
                        audManager.playSound(m_soundId.value());
                    }
                }
            }
            else if (m_curIdx >= m_frameCount && m_animType == animPlaybackType::NON_LOOPING){
                m_animationFinished = true;
            }
        }

        m_sourceRect = {
            m_frameIndices.at(m_curIdx).x,
            m_frameIndices.at(m_curIdx).y,
            m_spriteRes.x,
            m_spriteRes.y};
    }

    void Animation::draw(const Vector2 drawPos) const noexcept {
        assert(m_texture);
        assert(IsTextureValid(*m_texture));

        DrawTextureRec(
            *m_texture,
            m_sourceRect,
            drawPos,
            WHITE);
    }
}