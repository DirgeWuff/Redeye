//
// Created by DirgeWuff on 4/29/25.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "CommandListener.h"
#include "Entity.h"

class Player final : public Entity, public CommandListener {
    Texture2D m_walkSprites{};          // Walk sprite sheet
    Rectangle m_spriteRect{};           // Rectangle defining the current sprite
    const int m_numFrames;              // Number of sprites in sheet width-wise
    int m_frameIndex;                   // Currently selected sprite
    const int m_frameDelayAmount;       // Duration to delay next frame
    int m_frameDelayClock;              // Clock counting up to amount
    bool m_moving;                      // Whether or not the player is moving
    bool m_onGround;                    // Whether the player is on the ground or not
    int m_lastDirection;                // Last direction the player was moving for animation purposes
    b2Polygon m_footpawSensorBox{};     // Footpaw sensor box polygon
    b2ShapeDef m_footpawSensorShape{};  // Footpaw sensor box shape
    b2ShapeId m_footpawSensorId{};      // Footpaw sensor box shape ID
public:
    Player(
        float centerX,
        float centerY,
        b2WorldId world,
        const std::string& spritePath);

    ~Player() override;

    void update() override;
    void draw() const override;
    void moveRight() override;
    void moveLeft() override;
    void jump() override;
    void moveNowhere() override;
    b2ShapeId getFootpawSenorId() const;
    bool getFootpawSensorStatus() const;
    void setFootpawStatus(bool status);
};

#endif //PLAYER_H