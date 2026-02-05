//
// Author: DirgeWuff
// Created on 1/30/2026
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Module purpose/description:
//
// Class definition for Music.h and its member functions

#include "Music.h"
#include "../Utility/Utils.h"

namespace RE::Core {
    Music::Music(const std::string& musicFilePath) :
        m_music(LoadMusicStream(musicFilePath.c_str()))
    {
        assert(IsMusicValid(m_music));

        m_volume = 1.0f;
        m_pan = 0.5f;

        #ifdef DEBUG
            logDbg("Music constructed at address: ", this);
        #endif
    }

    Music::~Music() {
        if (IsMusicStreamPlaying(m_music))
            StopMusicStream(m_music);

        UnloadMusicStream(m_music);

        #ifdef DEBUG
            logDbg("Music destroyed at address: ", this);
        #endif
    }

    Music::Music(Music&& other) noexcept :
        m_music(other.m_music),
        m_volume(other.m_volume),
        m_pan(other.m_pan),
        m_isActive(other.m_isActive)
    {
        other.m_music = {};
    }

    Music& Music::operator=(Music&& other) noexcept {
        if (this != &other) {
            UnloadMusicStream(this->m_music);

            this->m_music = other.m_music;
            this->m_volume = other.m_volume;
            this->m_pan = other.m_pan;
            this->m_isActive = other.m_isActive;

            other.m_music = {};
        }

        return *this;
    }

    void Music::play() const {
        assert(IsMusicValid(m_music));

        PlayMusicStream(m_music);
    }

    void Music::update() const {
        assert(IsMusicStreamPlaying(m_music));

        UpdateMusicStream(m_music);
    }

    void Music::stop() const {
        assert(IsMusicStreamPlaying(m_music));

        StopMusicStream(m_music);
    }

    void Music::setVolume(const float volume) {
        assert(volume <= 1.0f && volume >= 0.0f);

        m_volume = volume;

        SetMusicVolume(m_music, m_volume);
    }

    void Music::setPan(const float pan) {
        assert(pan <= 1.0f && pan >= 0.0f);

        m_pan = pan;

        SetMusicPan(m_music, m_pan);
    }

    [[nodiscard]] bool Music::isPlaying() const {
        return IsMusicStreamPlaying(m_music);
    }
}