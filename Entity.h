//
// Created by DirgeWuff on 5/9/25.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <raylib.h>

#include "CommandListener.h"

class Entity : public CommandListener {
    Vector2 position{};
    Vector2 velocity{};
public:
    Entity();
    ~Entity() override;
    virtual bool update(const bool& onGround);
    virtual void draw() const = 0;
    virtual float getX() const;
    virtual float getY() const;
    virtual float getWidth() const = 0;
    virtual float getHeight() const = 0;
};

#endif //ENTITY_H
