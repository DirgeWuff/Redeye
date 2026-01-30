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
// Utility functions used to load, manage and play audio files used
// by various other objects. Includes the ability to load and unload
// an std::vector<Sound> and play a random sound from the vector.

#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <filesystem>
#include <string>
#include "raylib.h"
#include "../Utility/Logging.h"

template<typename T>
[[nodiscard]] std::vector<Sound> loadSoundVector(const T& dirPath, const float volume) {
    std::vector<Sound> sounds{};

    try {
        for (const auto& file : std::filesystem::directory_iterator(dirPath)) {
            std::string soundPath = file.path().string();

            Sound sound = LoadSound(soundPath.c_str());
            if (!IsSoundValid(sound))
                logFatal(
                    std::string("Error loading sound: ") + soundPath +
                    std::string(": loadSoundVector(Args...)"));

            SetSoundVolume(sound, volume);

            sounds.push_back(sound);
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        logFatal(std::string("Unable to parse sounds: ") + dirPath +
            std::string(e.what()) + std::string("loadSoundVector(Args...)"));
        return {};
    }
    catch (...) {
        logFatal(
            std::string("An unknown error has occurred while loading sounds: ") + dirPath +
            std::string(": loadSoundVector(Args...)"));
        return{};
    }

    return sounds;
}

void unloadSoundVector(std::vector<Sound>& vec);
void playRandomSound(const std::vector<Sound>& vec);

#endif //AUDIO_H
