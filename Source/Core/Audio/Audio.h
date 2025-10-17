//
// Created by DirgeWuff on 8/28/2025.
//

#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <filesystem>
#include <string>
#include "raylib.h"
#include "../Utility/Error.h"

template<typename T>
[[nodiscard]] std::vector<Sound> loadSoundVector(const T& dirPath, const float volume) {
    std::vector<Sound> sounds{};

    try {
        for (const auto& file : std::filesystem::directory_iterator(dirPath)) {
            std::string soundPath = file.path().string();

            Sound sound = LoadSound(soundPath.c_str());
            if (!IsSoundValid(sound))
                logErr(
                    std::string("Error loading sound: ") + soundPath +
                    std::string(": loadSoundVector(Args...)"));

            SetSoundVolume(sound, volume);

            sounds.push_back(sound);
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        logErr(std::string("Unable to parse sounds: ") + dirPath +
            std::string(e.what()) + std::string("loadSoundVector(Args...)"));
        return {};
    }
    catch (...) {
        logErr(
            std::string("An unknown error has occurred while loading sounds: ") + dirPath +
            std::string(": loadSoundVector(Args...)"));
        return{};
    }

    return sounds;
}

void unloadSoundVector(std::vector<Sound>& vec);
void playRandomSound(const std::vector<Sound>& vec);

#endif //AUDIO_H
