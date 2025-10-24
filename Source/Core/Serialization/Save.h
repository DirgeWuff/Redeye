//
// Created by DirgeWuff on 10/19/2025.
//

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
