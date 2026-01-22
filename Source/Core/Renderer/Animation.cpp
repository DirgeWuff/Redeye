//
// Created by DirgeWuff on 1/5/2026.
//

#include <cassert>
#include <iostream>

#include "Animation.h"
#include "../utility/Logging.h"

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
        const spriteIndex startingIndex,
        const spriteIndex endingIndex,
        const std::shared_ptr<Texture2D> tex,
        const Vector2 spriteResolution,
        const float frameDur,
        const playbackType type,
        const animationId id
    ) :
        m_texture(tex),
        m_spriteRes(spriteResolution),
        m_frameDuration(frameDur),
        m_animType(type),
        m_animId(id)
{
    assert(m_texture);
    assert(IsTextureValid(*m_texture));

    const std::size_t numFrames = endingIndex.x - startingIndex.x + 1;
    m_frameIndices.resize(numFrames);
    m_frameCount = numFrames - 1;

    // Generate first frame...
    m_frameIndices[0] = {
        static_cast<float>(startingIndex.x - 1) * m_spriteRes.x,
        static_cast<float>(startingIndex.y - 1) * m_spriteRes.y
    };

    // Then all other frames
    for (std::size_t i = 1; i < numFrames; i++) {
        const std::size_t sX = startingIndex.x + i;
        const std::size_t sY = startingIndex.y;

        m_frameIndices[i] = {
            static_cast<float>(sX - 1) * m_spriteRes.x,
            static_cast<float>(sY - 1) * m_spriteRes.y
        };
    }

    if (m_frameIndices.empty()) {
        logFatal("Failed to generate animation frames. Animation::Animation(Args...)");
        return;
    }

    m_sourceRect = {
        m_frameIndices[0].x,
        m_frameIndices[0].y,
        m_spriteRes.x,
        m_spriteRes.y
    };

    assert(m_frameIndices[m_frameCount].x < m_texture->width);
    assert(m_frameIndices[0].y < m_texture->height);

    #ifdef DEBUG
        logDbg("Animation constructed at address: ", this);
    #endif
}

void Animation::resetAnimation() noexcept {
    m_elapsedFrameTime = 0.0f;
    m_curIdx = 0;
}

void Animation::update() noexcept {
    if (m_animationFinished) return;

    const float dt = GetFrameTime();
    m_elapsedFrameTime += dt;

    if (m_elapsedFrameTime >= m_frameDuration) {
        if (m_curIdx < m_frameCount && m_animType != playbackType::SINGLE_FRAME) {
            m_curIdx++;
            m_elapsedFrameTime = 0.0f;
        }
        else if (m_curIdx >= m_frameCount && m_animType == playbackType::LOOP) {
            m_curIdx = 0;
            m_elapsedFrameTime = 0.0f;
        }
        else if (m_curIdx >= m_frameCount && m_animType == playbackType::NON_LOOPING){
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