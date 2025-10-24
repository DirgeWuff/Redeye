//
// Created by DirgeWuff on 10/24/2025.
//

#include "LayerTypes.h"

std::string keyToStr(const layerKey& key) {
    switch (key) {
        case layerKey::START_MENU: return "START_MENU";
        case layerKey::GAME_LAYER: return "GAME_LAYER";
        case layerKey::DEATH_MENU: return "DEATH_MENU";
        default: return "Unknown key type";
    }
}
