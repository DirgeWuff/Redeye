//
// Created by DirgeWuff on 10/24/2025.
//

#ifndef LAYERTYPES_H
#define LAYERTYPES_H

#include <string>

// Type of layer. Whether or not the layer can be drawn on top of another layer (partially transparent)
enum class layerType {
    PRIMARY_LAYER,
    OVERLAY_LAYER
};

// Names/keys of individual layers
enum class layerKey {
    START_MENU,
    GAME_LAYER,
    DEATH_MENU,
    CHECKPOINT_ALERT
};

std::string keyToStr(const layerKey& key);

#endif //LAYERTYPES_H
