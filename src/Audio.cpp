//
// Created by DirgeWuff on 8/28/2025.
//

#include "Audio.h"
#include "Utils.h"

void playRandomSound(const std::vector<Sound>& vec) {
    try {
        PlaySound(vec.at(getRandInt(0, vec.size() - 1)));
    }
    catch (std::out_of_range& e) {
        logErr(std::string("Unable to play sound, value out of range: " + std::string(e.what())));
    }
    catch (...) {
        logErr("Unknown exception thrown while playing sound. Ln 17, Audio.cpp");
    }
}
