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
// Function definitions for non-templated functions for Audio.h

#include <cassert>
#include "Sound.h"
#include "../Utility/Utils.h"
#include "../Utility/Logging.h"

namespace RE::Core {
    // SingleSound
    // =====================================================================================================================
    SingleSound::SingleSound(const std::string& soundFilePath) :
        m_sound(LoadSound(soundFilePath.c_str()))
    {
        if (!IsSoundValid(m_sound)) {
            logDbg("Failed to load sound file: ", soundFilePath, ". SingleSound::SingleSound(Args...)");
            return;
        }

        m_type = soundType::SINGLE_SOUND_EFFECT;
        m_volume = 1.0f;
        m_pan = 0.5f;

        #ifdef DEBUG
            logDbg("SingleSound constructed at address: ", this);
        #endif
    }

    SingleSound::~SingleSound() {
        assert(IsSoundValid(m_sound));

        UnloadSound(m_sound);

        #ifdef DEBUG
            logDbg("SingleSound destroyed at address: ", this);
        #endif
    }

    void SingleSound::play() const {
        assert(IsSoundValid(m_sound));

        PlaySound(m_sound);
    }

    void SingleSound::setVolume(const float volume) {
        assert(IsSoundValid(m_sound));
        assert(volume <= 1.0f && volume >= 0.0f);

        m_volume = volume;

        SetSoundVolume(m_sound, volume);
    }

    void SingleSound::setPan(const float pan) {
        assert(IsSoundValid(m_sound));
        assert(pan <= 1.0f && pan >= 0.0f);

        m_pan = pan;

        SetSoundPan(m_sound, m_pan);
    }

    // SoundArray
    // =====================================================================================================================
    SoundArray::SoundArray(const std::string& soundDirPath) {
        try {
            for (const auto& file : fs::directory_iterator(fs::path(soundDirPath))) {
                std::string soundFilePath = file.path().string();

                ::Sound sound = LoadSound(soundFilePath.c_str());
                if (!IsSoundValid(sound)) {
                    logDbg("One or more sounds failed to load from: ", soundFilePath,
                        "SoundArray::SoundArray(Args...");

                    return;
                }

                m_sounds.push_back(sound);
            }
        }
        catch (const std::exception& e){
            logFatal(std::string("Construction failed: ") + std::string(e.what()) +
                std::string(". SoundArray::SoundArray(Args...)"));

            return;
        }
        catch (...) {
            logFatal("Construction failed: An unknown error has occurred. SoundArray::SoundArray(Args...)");
            return;
        }

        m_type = soundType::SOUND_ARRAY;
        m_volume = 1.0f;
        m_pan = 0.5f;

        #ifdef DEBUG
            logDbg("SoundArray constructed at address: ", this);
        #endif
    }

    SoundArray::~SoundArray() {
        assert(!m_sounds.empty());

        for (const auto& sound : m_sounds) {
            assert(IsSoundValid(sound));

            UnloadSound(sound);

        #ifdef DEBUG
            logDbg("SoundArray destroyed at address: ", this);
        #endif
        }
    }

    void SoundArray::play() const {
        assert(!m_sounds.empty());

        try {
            PlaySound(m_sounds.at(getRandInt(0, m_sounds.size() - 1)));
        }
        catch (const std::exception& e) {
            logFatal(std::string("SoundArray::play() failed:") + std::string(e.what()));
        }
        catch (...) {
            logFatal("SoundArray::play() failed: An unknown error has occurred.");
        }
    }

    void SoundArray::setVolume(const float volume) {
        assert(volume <= 1.0f && volume >= 0.0f);
        m_volume = volume;

        for (const auto& sound : m_sounds) {
            assert(IsSoundValid(sound));
            SetSoundVolume(sound, m_volume);
        }
    }

    void SoundArray::setPan(const float pan) {
        assert(pan <= 1.0f && pan >= 0.0f);
        m_pan = pan;

        for (const auto& sound : m_sounds) {
            assert(IsSoundValid(sound));
            SetSoundPan(sound, m_pan);
        }
    }
}