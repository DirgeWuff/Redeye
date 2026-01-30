//
// Author: DirgeWuff
// Created on: 10/19/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Declares a data structure and functions to read/write a TOML-based save file
// from disk, as well as create a default save file that places the player
// at the beginning of the game.

#ifndef SAVE_H
#define SAVE_H

#include <filesystem>
#include "box2d/types.h"

namespace fs = std::filesystem;

struct saveData {
    fs::path currentMapPath;
    b2Vec2 centerPosition;
};

void createDefaultSave();
void saveGame(const saveData& data);
saveData loadGame();

#endif //SAVE_H
