//
// Author: DirgeWuff
// Created on: 8/28/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class declarations for sounds and music. These objects are used to load and
// store sounds and music, and related metadata and define an interface for
// AudioManager to control them.

#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <filesystem>
#include <string>
#include <cstdint>
#include "raylib.h"

namespace fs = std::filesystem;

namespace RE::Core {
    enum class soundType : std::uint8_t {
        SINGLE_SOUND_EFFECT,
        SOUND_ARRAY,
        MUSIC
    };

    // Virtual/Base
    // =====================================================================================================================
    class Sound {
    protected:
        soundType m_type{};

        // These are here because there may be conditions in which I want to return them later...
        float m_volume{};
        float m_pan{};
    public:
        Sound() = default;
        virtual ~Sound() = default;

        virtual void play() const {}
        virtual void setVolume(float volume) {}
        virtual void setPan(float pan) {}

        [[nodiscard]] soundType getType() const noexcept { return m_type; }
    };

    // Single sound file/effect
    // =====================================================================================================================
    class SingleSound final : public Sound {
        ::Sound m_sound{};
    public:
        SingleSound() = default;
        explicit SingleSound(const std::string& soundFilePath);
        ~SingleSound() override;

        SingleSound(const SingleSound&) = delete;
        SingleSound(SingleSound&& other) noexcept;
        SingleSound& operator=(const SingleSound&) = delete;
        SingleSound& operator=(SingleSound&& other) noexcept;

        void play() const override;
        void setVolume(float volume) override;
        void setPan(float pan) override;
    };

    // Array/Vector of sounds, played at random for variety
    // =====================================================================================================================
    class SoundArray final : public Sound {
        std::vector<::Sound> m_sounds{};
    public:
        SoundArray() = default;
        explicit SoundArray(const std::string& soundDirPath);
        ~SoundArray() override;

        SoundArray(const SoundArray&) = delete;
        SoundArray(SoundArray&& other) noexcept;
        SoundArray& operator=(const SoundArray&) = delete;
        SoundArray& operator=(SoundArray&& other) noexcept;

        void play() const override;
        void setVolume(float volume) override;
        void setPan(float pan) override;
    };
}

#endif //AUDIO_H
