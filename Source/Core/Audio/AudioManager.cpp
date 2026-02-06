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

#include <cassert>
#include "ranges"
#include "AudioManager.h"
#include "../Utility/Logging.h"
#include "../Utility/Enum.h"

namespace RE::Core {
    AudioManager::AudioManager() {
        #ifdef DEBUG
            logDbg("AudioManager created at address: ", this);
        #endif
    }

    AudioManager::~AudioManager() {
        #ifdef DEBUG
            logDbg("AudioManager Destroyed at address: ", this);
        #endif
    }

    AudioManager::AudioManager(AudioManager&& other) noexcept :
        m_sounds(std::move(other.m_sounds)),
        m_musicTracks(std::move(other.m_musicTracks))
    {
        #ifdef DEBUG
                logDbg("Move called on AudioManager, new address: ", this);
        #endif
    }

    AudioManager& AudioManager::operator=(AudioManager&& other) noexcept {
        if (this != &other) {
            this->m_sounds = std::move(other.m_sounds);
            this->m_musicTracks = std::move(other.m_musicTracks);

            #ifdef DEBUG
                        logDbg("Move called on AudioManager, new address: ", this);
            #endif
        }

        return *this;
    }

    // Sound control/management
    // =================================================================================================================
    void AudioManager::pushSound(soundId id, std::unique_ptr<Sound> newSound) {
        m_sounds.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(id),
            std::forward_as_tuple(std::move(newSound)));
    }

    void AudioManager::popSound(const soundId& id) {
        const auto it = m_sounds.find(id);
        assert(it != m_sounds.end());

        m_sounds.erase(id);
    }

    void AudioManager::playSound(const soundId& id) {
        const auto it = m_sounds.find(id);
        assert(it != m_sounds.end());
        assert(it->second);

        it->second->play();
    }

    void AudioManager::setSoundVolume(const soundId& id, const float volume) {
        const auto it = m_sounds.find(id);
        assert(it != m_sounds.end());
        assert(it->second);

        it->second->setVolume(volume);
    }

    void AudioManager::setSoundPan(const soundId& id, const float pan) {
        const auto it = m_sounds.find(id);
        assert(it != m_sounds.end());
        assert(it->second);

        it->second->setPan(pan);
    }

    // Music control/management
    // =================================================================================================================
    void AudioManager::pushMusic(musicId id, Music&& music) {
        m_musicTracks.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(id),
            std::forward_as_tuple(std::move(music)));
    }

    void AudioManager::popMusic(const musicId& id) {
        const auto it = m_musicTracks.find(id);
        assert(it != m_musicTracks.end());

        m_musicTracks.erase(id);
    }

    void AudioManager::startMusic(const musicId& id) {
        const auto it = m_musicTracks.find(id);
        assert(it != m_musicTracks.end());

        it->second.play();
    }

    void AudioManager::updateMusic() {
        for (const auto& track : std::views::values(m_musicTracks)) {
            if (!track.isPlaying()) continue;
            track.update();
        }
    }

    void AudioManager::stopMusic(const musicId& id) {
        const auto it = m_musicTracks.find(id);
        assert(it != m_musicTracks.end());

        it->second.stop();
    }

    void AudioManager::setMusicVolume(const musicId& id, const float volume) {
        const auto it = m_musicTracks.find(id);
        assert(it != m_musicTracks.end());

        it->second.setVolume(volume);
    }

    void AudioManager::setMusicPan(const musicId& id, const float pan) {
        const auto it = m_musicTracks.find(id);
        assert(it != m_musicTracks.end());

        it->second.setPan(pan);
    }
}