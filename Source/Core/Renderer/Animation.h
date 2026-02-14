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
// Class declarations for several animation types. These objects are used
// AnimationManager, and contain sprite and sound data that is used to
// display animations, and play any synced sounds they may own.
// animationDescriptor and its children are used to load relevant
// data from disk and pass it to the constructor for a given animation type.
// NOTE: For the base/virtual class (Animation), some of the virtual
// functions are not implemented in either base or child classes, but
// are there for future expansion as animations grow and become more
// advanced.

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
        animationDescriptor() = default;
        animationDescriptor(
            const spriteIndex startingFrame,
            const spriteIndex endingFrame,
            const Vector2 res,
            const float duration,
            const animPlaybackMode mode,
            const animationId animId,
            const animType animType) :
                start(startingFrame),
                end(endingFrame),
                spriteRes(res),
                frameDuration(duration),
                playbackMode(mode),
                id(animId),
                type(animType)
        {
        }

        // Mostly here to just let me dynamic_cast<T> later in the manager...
        virtual ~animationDescriptor() = default;

        spriteIndex start{};
        spriteIndex end{};
        Vector2 spriteRes{};
        float frameDuration{};
        animPlaybackMode playbackMode{};
        animationId id{};
        animType type{};
    };

    struct keyframeSoundDescriptor final : animationDescriptor {
        std::vector<std::uint8_t> soundFrames{};
        soundId soundFrameSoundId{};

        keyframeSoundDescriptor() = default;
        keyframeSoundDescriptor(
            const animationDescriptor& baseDesc,
            std::vector<std::uint8_t> sFrames,
            const soundId sId) :
                soundFrames(std::move(sFrames)),
                soundFrameSoundId(sId)
        {
            this->start = baseDesc.start;
            this->end = baseDesc.end;
            this->spriteRes = baseDesc.spriteRes;
            this->frameDuration = baseDesc.frameDuration;
            this->playbackMode = baseDesc.playbackMode;
            this->id = baseDesc.id;
            this->type = baseDesc.type;
        }

        ~keyframeSoundDescriptor() override = default;
    };

    struct transitionSoundDescriptor final : animationDescriptor {
        soundId transitionFrameSoundId{};

        transitionSoundDescriptor() = default;
        transitionSoundDescriptor(
            const animationDescriptor& baseDesc,
            const soundId tfSId) :
                transitionFrameSoundId(tfSId)
        {
            this->start = baseDesc.start;
            this->end = baseDesc.end;
            this->spriteRes = baseDesc.spriteRes;
            this->frameDuration = baseDesc.frameDuration;
            this->playbackMode = baseDesc.playbackMode;
            this->id = baseDesc.id;
            this->type = baseDesc.type;
        }

        ~transitionSoundDescriptor() override = default;
    };

    // Add another struct with both key and transition sounds if needed later...

    // TODO: Add support for animations that span multiple rows on the sprite sheet if that becomes a thing

    // Silent/base animation class
    class Animation {
        friend class AnimationManager;
    protected:
        std::vector<Vector2> m_frameIndices{};
        std::shared_ptr<Texture2D> m_texture{};
        Rectangle m_sourceRect{};
        Vector2 m_spriteRes{};
        std::size_t m_frameCount{};
        float m_frameDuration{};
        float m_elapsedFrameTime{};
        std::uint8_t m_curIdx{};
        animPlaybackMode m_playbackType{};
        animationId m_animId{};
        animType m_type{};
        bool m_animationFinished{};

        // Sort of a "universal constructor" that will do most of the work to create
        // a base Animation class, avoiding code duplication
        void initBase(
            std::shared_ptr<Texture2D> tex,
            spriteIndex startingFrame,
            spriteIndex endingFrame,
            Vector2 res,
            float duration,
            animPlaybackMode playbackType,
            animationId animId,
            animType type);
    public:
        Animation();
        Animation(
            std::shared_ptr<Texture2D> tex,
            const animationDescriptor& desc);

        virtual ~Animation();

        Animation(const Animation&) = delete;
        Animation(Animation&& other) noexcept;
        Animation& operator=(const Animation&) = delete;
        Animation& operator=(Animation&& other) noexcept;

        virtual void onBegin() {}
        virtual void onEnd() {}
        virtual void resetAnimation() noexcept;
        virtual void update() noexcept;
        virtual void draw(Vector2 drawPos) const noexcept;

        [[nodiscard]] animType getType() const noexcept;
    };

    class KeyframeSoundAnim final : public Animation {
        std::vector<std::uint8_t> m_soundFrames{};
        std::shared_ptr<AudioManager> m_audioManager{};
        soundId m_soundId{};
    public:
        KeyframeSoundAnim();
        KeyframeSoundAnim(
            std::shared_ptr<Texture2D> tex,
            const keyframeSoundDescriptor& desc,
            std::shared_ptr<AudioManager> manager);

        ~KeyframeSoundAnim() noexcept override;

        KeyframeSoundAnim(const KeyframeSoundAnim&) = delete;
        KeyframeSoundAnim(KeyframeSoundAnim&& other) noexcept;
        KeyframeSoundAnim& operator=(const KeyframeSoundAnim&) = delete;
        KeyframeSoundAnim& operator=(KeyframeSoundAnim&& other) noexcept;

        void update() noexcept override;
    };

    class TransitionSoundAnim final : public Animation {
        // If the animation manager detects that we're transitioning
        // from this animation into m_transitionToId, a sound will be
        // the sound with m_soundId will be played by onEnd().
        std::shared_ptr<AudioManager> m_audioManager{};
        soundId m_soundId{};
    public:
        TransitionSoundAnim();
        TransitionSoundAnim(
            std::shared_ptr<Texture2D> tex,
            const transitionSoundDescriptor& desc,
            std::shared_ptr<AudioManager> manager);

        ~TransitionSoundAnim() noexcept override;

        TransitionSoundAnim(const TransitionSoundAnim&) = delete;
        TransitionSoundAnim(TransitionSoundAnim&& other) noexcept;
        TransitionSoundAnim& operator=(const TransitionSoundAnim&) = delete;
        TransitionSoundAnim& operator=(TransitionSoundAnim&& other) noexcept;

        void onEnd() override;
    };
}

#endif //ANIMATION_H
