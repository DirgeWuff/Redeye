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
// Management class for sounds. Capable of loading, and playing sounds.

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <unordered_map>
#include "Sound.h"
#include "Music.h"
#include "../Utility/Enum.h"

namespace RE::Core {
    class AudioManager final {
        std::unordered_map<soundId, std::unique_ptr<Sound>> m_sounds;
        std::unordered_map<musicId, Music> m_musicTracks;
    public:
        AudioManager();
        ~AudioManager();

        AudioManager(const AudioManager&) = delete;
        AudioManager(AudioManager&& other) noexcept;
        AudioManager& operator=(const AudioManager&) = delete;
        AudioManager& operator=(AudioManager&& other) noexcept;

        void pushSound(soundId id, std::unique_ptr<Sound> newSound);
        void popSound(const soundId& id);
        void playSound(const soundId& id);
        void setSoundVolume(const soundId& id, float volume);
        void setSoundPan(const soundId& id, float pan);

        void pushMusic(musicId id, Music&& music);
        void popMusic(const musicId& id);
        void startMusic(const musicId& id);
        void updateMusic();
        void stopMusic(const musicId& id);
        void setMusicVolume(const musicId& id, float volume);
        void setMusicPan(const musicId& id, float pan);
    };
}

#endif //AUDIOMANAGER_H
