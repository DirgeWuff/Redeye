//
// Created by DirgeWuff on 4/29/25.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <string>
#include "Entity.h"

class Player final : public Entity {
    Vector2 position;               // Position of player sprite
    Texture2D walkSprites{};        // Walk sprite sheet
    Rectangle spriteRect{};         // Rectangle defining the current sprite
    const int numFrames;            // Number of sprites in sheet width-wise
    int frameIndex;                 // Currently selected sprite
    const int frameDelayAmount;     // Duration to delay next frame
    int frameDelayClock;            // Clock counting up to amount
    bool moving;                    // Whether or not the player is moving
    bool onGround;                  // Whether the player is on the ground or not
    const float movementSpeed;      // Speed at which the player moves
    int lastDirection;              // Last direction the player was moving for animation purposes
    const float groundPos;          // Temp placeholder value
    Vector2 velocity;               // X/Y movement speed of the player
    int height;                     // Height of sprite
    int width;                      // Width of sprite
public:
    explicit Player(std::string spritePath);
    ~Player() override;
    void update();
    void draw() const;
    void moveRight() override;
    void moveLeft() override;
    void jump() override;
    void moveNowhere() override;
    float getX() const override;
    float getY() const override;
    float getWidth() const override;
    float getHeight() const override;
};

#endif //PLAYER_H
