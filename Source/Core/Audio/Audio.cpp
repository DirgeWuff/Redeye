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

#include "Audio.h"
#include "../Utility/Utils.h"

void playRandomSound(const std::vector<Sound>& vec) {
    assert(!vec.empty());

    try {
        PlaySound(vec.at(getRandInt(0, vec.size() - 1)));
    }
    catch (std::out_of_range& e) {
        logFatal(
            std::string("Unable to play sound, value out of range: "
                + std::string(e.what())) + std::string(": playRandomSound(Args...)"));
    }
    catch (...) {
        logFatal("Unknown exception thrown while playing sound: playRandomSound(Args...)");
    }
}

void unloadSoundVector(std::vector<Sound>& vec) {
    for (const auto& sound : vec) {
        UnloadSound(sound);
    }
    vec.clear();
}