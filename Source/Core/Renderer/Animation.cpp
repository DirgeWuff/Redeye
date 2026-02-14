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
#include <filesystem>

namespace fs = std::filesystem;

namespace RE::Core {
    // Virtual/Base
    //==================================================================================================================
    void Animation::initBase(
        std::shared_ptr<Texture2D> tex,
        const spriteIndex startingFrame,
        const spriteIndex endingFrame,
        const Vector2 res,
        const float duration,
        const animPlaybackMode playbackType,
        const animationId animId,
        const animType type)
    {
        assert(tex);
        assert(IsTextureValid(*tex));

        m_texture = tex;
        m_spriteRes = res;
        m_frameDuration = duration;
        m_playbackType = playbackType;
        m_animId = animId;
        m_type = type;

        const std::size_t numFrames = endingFrame.x - startingFrame.x + 1;

        m_frameIndices.resize(numFrames);
        m_frameCount = numFrames - 1;

        // Generate first frame...
        m_frameIndices[0] = {
            static_cast<float>(startingFrame.x - 1) * m_spriteRes.x,
            static_cast<float>(startingFrame.y - 1) * m_spriteRes.y
        };

        // Then all other frames
        for (std::size_t i = 1; i < numFrames; i++) {
            const std::size_t sX = startingFrame.x + i;
            const std::size_t sY = startingFrame.y;

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
    }

    Animation::Animation() {
        #ifdef DEBUG
            logDbg("Default Animation constructed at address: ", this);
        #endif
    }

    Animation::Animation(
        const std::shared_ptr<Texture2D> tex,
        const animationDescriptor& desc)
    {
       initBase(
           tex,
           desc.start,
           desc.end,
           desc.spriteRes,
           desc.frameDuration,
           desc.playbackMode,
           desc.id,
           desc.type);

        #ifdef DEBUG
            logDbg("Animation constructed at address: ", this);
        #endif
    }

    Animation::~Animation() {
        #ifdef DEBUG
                logDbg("Animation destroyed at address: ", this);
        #endif
    }

    Animation::Animation(Animation&& other) noexcept :
        m_frameIndices(std::move(other.m_frameIndices)),
        m_texture(std::move(other.m_texture)),
        m_sourceRect(other.m_sourceRect),
        m_spriteRes(other.m_spriteRes),
        m_frameCount(other.m_frameCount),
        m_frameDuration(other.m_frameDuration),
        m_elapsedFrameTime(other.m_elapsedFrameTime),
        m_curIdx(other.m_curIdx),
        m_playbackType(other.m_playbackType),
        m_animId(other.m_animId),
        m_animationFinished(other.m_animationFinished)
    {
        #ifdef DEBUG
            logDbg("Move called on Animation, new address: ", this);
        #endif
    }

    Animation& Animation::operator=(Animation&& other) noexcept {
        if (this != &other) {
            this->m_frameIndices = std::move(other.m_frameIndices);
            this->m_texture = std::move(other.m_texture);
            this->m_sourceRect = other.m_sourceRect;
            this->m_spriteRes = other.m_spriteRes;
            this->m_frameCount = other.m_frameCount;
            this->m_frameDuration = other.m_frameDuration;
            this->m_elapsedFrameTime = other.m_elapsedFrameTime;
            this->m_curIdx = other.m_curIdx;
            this->m_playbackType = other.m_playbackType;
            this->m_animId = other.m_animId;
            this->m_animationFinished = other.m_animationFinished;
        }

        #ifdef DEBUG
                logDbg("Move called on Animation, new address: ", this);
        #endif

        return *this;
    }

    void Animation::resetAnimation() noexcept {
        m_elapsedFrameTime = 0.0f;
        m_curIdx = 0;
    }

    void Animation::update() {
        if (m_animationFinished) return;

        const float dt = GetFrameTime();
        m_elapsedFrameTime += dt;

        if (m_elapsedFrameTime >= m_frameDuration) {
            if (m_curIdx < m_frameCount && m_playbackType != animPlaybackMode::SINGLE_FRAME) {
                m_curIdx++;
                m_elapsedFrameTime = 0.0f;
            }
            else if (m_curIdx >= m_frameCount && m_playbackType == animPlaybackMode::LOOP) {
                m_curIdx = 0;
                m_elapsedFrameTime = 0.0f;
            }
            else if (m_curIdx >= m_frameCount && m_playbackType == animPlaybackMode::NON_LOOPING){
                m_animationFinished = true;
            }
        }

        m_sourceRect = {
            m_frameIndices.at(m_curIdx).x,
            m_frameIndices.at(m_curIdx).y,
            m_spriteRes.x,
            m_spriteRes.y};

        std::ofstream f;

        if (fs::exists("sourceRectLog.txt")) {
            f.open("sourceRectLog.txt", std::ios::app);

            f << "Animation::Update()\n";
            f << "X: " << m_sourceRect.x << " Y: " << m_sourceRect.y;
            f << " " << animIdToStr(m_animId) << "\n";
            f.close();
        }
        else {
            f.open("sourceRectLog.txt");

            f << "Animation::Update()\n";
            f << "X: " << m_sourceRect.x << " Y: " << m_sourceRect.y;
            f << " " << animIdToStr(m_animId) << "\n";
            f.close();
        }
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

    [[nodiscard]] animType Animation::getType() const noexcept {
        return m_type;
    }

    // KeyframeSoundAnim
    // =================================================================================================================
    KeyframeSoundAnim::KeyframeSoundAnim() {
        #ifdef DEBUG
            logDbg("Default KeyframeSoundAnim created at address: ", this);
        #endif
    }

    KeyframeSoundAnim::KeyframeSoundAnim(
        std::shared_ptr<Texture2D> tex,
        const keyframeSoundDescriptor& desc,
        std::shared_ptr<AudioManager> manager) :
            m_soundFrames(desc.soundFrames),
            m_audioManager(std::move(manager)),
            m_soundId(desc.soundFrameSoundId)
    {
        initBase(
            std::move(tex),
            desc.start,
            desc.end,
            desc.spriteRes,
            desc.frameDuration,
            desc.playbackMode,
            desc.id,
            desc.type);

        #ifdef DEBUG
            logDbg("KeyframeSoundAnim constructed at address: ", this);
        #endif
    }

    KeyframeSoundAnim::~KeyframeSoundAnim() {
        #ifdef DEBUG
            logDbg("KeyframeSoundAnim constructed at address: ", this);
        #endif
    }

    KeyframeSoundAnim::KeyframeSoundAnim(KeyframeSoundAnim&& other) noexcept :
        m_soundFrames(std::move(other.m_soundFrames)),
        m_audioManager(std::move(other.m_audioManager)),
        m_soundId(other.m_soundId)
    {
        this->m_frameIndices = std::move(other.m_frameIndices);
        this->m_texture = std::move(other.m_texture);
        this->m_sourceRect = other.m_sourceRect;
        this->m_spriteRes = other.m_spriteRes;
        this->m_frameCount = other.m_frameCount;
        this->m_frameDuration = other.m_frameDuration;
        this->m_elapsedFrameTime = other.m_elapsedFrameTime;
        this->m_curIdx = other.m_curIdx;
        this->m_playbackType = other.m_playbackType;
        this->m_animId = other.m_animId;
        this->m_animationFinished = other.m_animationFinished;

        #ifdef DEBUG
            logDbg("Move called on KeyframeSoundAnim, new address: ", this);
        #endif
    }

    KeyframeSoundAnim& KeyframeSoundAnim::operator=(KeyframeSoundAnim&& other) noexcept {
        if (this != &other) {
            this->m_audioManager = std::move(other.m_audioManager);
            this->m_soundFrames = std::move(other.m_soundFrames);
            this->m_frameIndices = std::move(other.m_frameIndices);
            this->m_texture = std::move(other.m_texture);
            this->m_sourceRect = other.m_sourceRect;
            this->m_spriteRes = other.m_spriteRes;
            this->m_frameCount = other.m_frameCount;
            this->m_frameDuration = other.m_frameDuration;
            this->m_elapsedFrameTime = other.m_elapsedFrameTime;
            this->m_curIdx = other.m_curIdx;
            this->m_playbackType = other.m_playbackType;
            this->m_animId = other.m_animId;
            this->m_soundId = other.m_soundId;
            this->m_animationFinished = other.m_animationFinished;
        }

        #ifdef DEBUG
            logDbg("Move assignment called on KeyframeSoundAnim, new address: ", this);
        #endif

        return *this;
    }

    void KeyframeSoundAnim::update() noexcept {
        if (m_animationFinished) return;

        const float dt = GetFrameTime();
        m_elapsedFrameTime += dt;

        if (m_elapsedFrameTime >= m_frameDuration) {
            if (m_curIdx < m_frameCount && m_playbackType != animPlaybackMode::SINGLE_FRAME) {
                m_curIdx++;
                m_elapsedFrameTime = 0.0f;

                const auto it =
                        std::ranges::find(m_soundFrames, m_curIdx);

                if (it != m_soundFrames.end()) {
                    m_audioManager->playSound(m_soundId);
                }
            }
            else if (m_curIdx >= m_frameCount && m_playbackType == animPlaybackMode::LOOP) {
                m_curIdx = 0;
                m_elapsedFrameTime = 0.0f;

                const auto it =
                        std::ranges::find(m_soundFrames, m_curIdx);

                if (it != m_soundFrames.end()) {
                    m_audioManager->playSound(m_soundId);
                }
            }
            else if (m_curIdx >= m_frameCount && m_playbackType == animPlaybackMode::NON_LOOPING){
                m_animationFinished = true;
            }
        }

        m_sourceRect = {
            m_frameIndices.at(m_curIdx).x,
            m_frameIndices.at(m_curIdx).y,
            m_spriteRes.x,
            m_spriteRes.y};

        std::ofstream f;

        if (fs::exists("sourceRectLog.txt")) {
            f.open("sourceRectLog.txt", std::ios::app);

            f << "KeyframeSoundAnim::Update()\n";
            f << "X: " << m_sourceRect.x << " Y: " << m_sourceRect.y;
            f << " " << animIdToStr(m_animId) << "\n";
            f.close();
        }
        else {
            f.open("sourceRectLog.txt");

            f << "KeyframeSoundAnim::Update()\n";
            f << "X: " << m_sourceRect.x << " Y: " << m_sourceRect.y;
            f << " " << animIdToStr(m_animId) << "\n";
            f.close();
        }
    }

    // TransitionSoundAnim
    // =================================================================================================================
    TransitionSoundAnim::TransitionSoundAnim() {
        #ifdef DEBUG
            logDbg("Default TransitionSoundAnimation created at address: ", this);
        #endif
    }

    TransitionSoundAnim::TransitionSoundAnim(
        std::shared_ptr<Texture2D> tex,
        const transitionSoundDescriptor& desc,
        std::shared_ptr<AudioManager> manager) :
            m_audioManager(std::move(manager)),
            m_soundId(desc.transitionFrameSoundId)
    {
        assert(m_audioManager);

        initBase(
            std::move(tex),
            desc.start,
            desc.end,
            desc.spriteRes,
            desc.frameDuration,
            desc.playbackMode,
            desc.id,
            desc.type);

        #ifdef DEBUG
            logDbg("KeyframeSoundAnim constructed at address: ", this);
        #endif
    }

    TransitionSoundAnim::~TransitionSoundAnim() {
        #ifdef DEBUG
            logDbg("TransitionSoundAnim destroyed at address: ", this);
        #endif
    }

    TransitionSoundAnim::TransitionSoundAnim(TransitionSoundAnim&& other) noexcept :
        m_soundId(other.m_soundId)
    {
        this->m_frameIndices = std::move(other.m_frameIndices);
        this->m_texture = std::move(other.m_texture);
        this->m_sourceRect = other.m_sourceRect;
        this->m_spriteRes = other.m_spriteRes;
        this->m_frameCount = other.m_frameCount;
        this->m_frameDuration = other.m_frameDuration;
        this->m_elapsedFrameTime = other.m_elapsedFrameTime;
        this->m_curIdx = other.m_curIdx;
        this->m_playbackType = other.m_playbackType;
        this->m_animId = other.m_animId;
        this->m_animationFinished = other.m_animationFinished;

        #ifdef DEBUG
            logDbg("Move called on TransitionSoundAnim, new address: ", this);
        #endif
    }

    TransitionSoundAnim& TransitionSoundAnim::operator=(TransitionSoundAnim&& other) noexcept {
        if (this != &other) {
            this->m_frameIndices = std::move(other.m_frameIndices);
            this->m_texture = std::move(other.m_texture);
            this->m_sourceRect = other.m_sourceRect;
            this->m_spriteRes = other.m_spriteRes;
            this->m_frameCount = other.m_frameCount;
            this->m_frameDuration = other.m_frameDuration;
            this->m_elapsedFrameTime = other.m_elapsedFrameTime;
            this->m_curIdx = other.m_curIdx;
            this->m_playbackType = other.m_playbackType;
            this->m_animId = other.m_animId;
            this->m_animationFinished = other.m_animationFinished;
            this->m_soundId = other.m_soundId;
        }

        #ifdef DEBUG
            logDbg("Move assignment called on TransitionSoundAnim, new address: ", this);
        #endif

        return *this;
    }

    void TransitionSoundAnim::onEnd() {
        assert(m_audioManager);

        m_audioManager->playSound(m_soundId);
    }

}