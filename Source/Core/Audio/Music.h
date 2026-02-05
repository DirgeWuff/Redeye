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
// Class declaration for Music, a very similar class to Sound and its children.
// This is a separate class due to the fact that music has different properties
// and control requirements than sound effects, and therefore should be distinct
// from Sound.

#ifndef MUSIC_H
#define MUSIC_H

#include <string>
#include "raylib.h"
#include "../Utility/Enum.h"

namespace RE::Core {
    class Music final {
        ::Music m_music{};
        float m_volume{};
        float m_pan{};
        bool m_isActive{};
    public:
        Music() = default;
        explicit Music(const std::string& musicFilePath);
        ~Music();

        Music(const Music&) = delete;
        Music(Music&& other) noexcept;
        Music& operator=(const Music&) = delete;
        Music& operator=(Music&& other) noexcept;

        void play() const;
        void update() const;
        void stop() const;
        void setVolume(float volume);
        void setPan(float pan);

        [[nodiscard]] bool isPlaying() const;


    };
}

#endif //MUSIC_H
