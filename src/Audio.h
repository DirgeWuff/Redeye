//
// Created by DirgeWuff on 8/28/2025.
//

#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <filesystem>
#include "raylib.h"
#include "Error.h"

// Load all sounds from a specified folder into a vector
template<typename T>
[[nodiscard]] std::vector<Sound> loadSoundVector(const T& dirPath, const float volume) {
    std::vector<Sound> sounds{};
    try {
        for (const auto& file : std::filesystem::directory_iterator(dirPath)) {
            std::string soundPath = file.path().string();

            Sound sound = LoadSound(soundPath.c_str());
            if (!IsSoundValid(sound))
                logErr(std::string("Error loading sound: ") + soundPath);

            SetSoundVolume(sound, volume);

            sounds.push_back(sound);
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        logErr(std::string("Unable to parse sounds: ") + dirPath + std::string(e.what()));
        return {};
    }
    catch (...) {
        logErr(std::string("An unknown error has occurred while loading sounds: ") + dirPath);
    }

    return sounds;
}

void playRandomSound(const std::vector<Sound>& vec); // Play a sound at a randomly generated index within a vector.

#endif //AUDIO_H
