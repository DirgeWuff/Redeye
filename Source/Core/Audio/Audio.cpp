//
// Created by DirgeWuff on 8/28/2025.
//

#include "Audio.h"
#include "../Utility/Utils.h"

void playRandomSound(const std::vector<Sound>& vec) {
    assert(!vec.empty());

    try {
        PlaySound(vec.at(getRandInt(0, vec.size() - 1)));
    }
    catch (std::out_of_range& e) {
        logErr(
            std::string("Unable to play sound, value out of range: "
                + std::string(e.what())) + std::string(": playRandomSound(Args...)"));
    }
    catch (...) {
        logErr("Unknown exception thrown while playing sound: playRandomSound(Args...)");
    }
}

void unloadSoundVector(std::vector<Sound>& vec) {
    for (const auto& sound : vec) {
        UnloadSound(sound);
    }
    vec.clear();
}