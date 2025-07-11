//
// Created by DirgeWuff on 6/27/25.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "BoxBody.h"
#include "box2d/types.h"

// Base class from which player/enemy bodies are created from in Box2D.
// May want to convert this to have most of the logic contained within the player class later

class Entity : public BoxBody {
public:
    Entity() = default;

    Entity(
        float centerX,
        float centerY,
        float fullWidth,
        float fullHeight,
        bodyConfig config,
        b2WorldId world);
};

#endif //ENTITY_H
