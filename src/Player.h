//
// Created by DirgeWuff on 4/29/25.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include "../external_libs/Raylib/include/raylib.h"
#include "CommandListener.h"
#include "Entity.h"
#include "Camera.h"

class SceneCamera;

struct saveData {
    b2Vec2 centerPosition{};
    // uint8_t health;
    // uint8_t phoneBattery;
    // Scene level; <- Maybe find a better way to do this later...
    // Any other data here
};

class Player final : public Entity, public CommandListener {
    Texture2D m_walkSprites{};          // Walk sprite sheet
    Rectangle m_spriteRect{};           // Rectangle defining the current sprite
    const uint8_t m_numFrames;          // Number of sprites in sheet width-wise
    uint8_t m_frameIndex;               // Currently selected sprite
    const uint8_t m_frameDelayAmount;   // Duration to delay next frame
    uint8_t m_frameDelayClock;          // Clock counting up to amount
    bool m_moving;                      // Whether or not the player is moving
    bool m_onGround;                    // Whether the player is on the ground or not
    uint16_t m_activeGroundContacts;    // How many b2BeginContactEvents are active on the player
    bool m_dead;                        // Whether the player is dead or not
    uint8_t m_lastDirection;            // Last direction the player was moving for animation purposes
    b2Polygon m_footpawSensorBox{};     // Footpaw sensor box polygon
    b2ShapeDef m_footpawSensorShape{};  // Footpaw sensor box shape
    b2ShapeId m_footpawSensorId{};      // Footpaw sensor box shape ID
    saveData m_currentCheckpoint;       // Current checkpoint data
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
    void murder(const std::unique_ptr<SceneCamera>& camera);
    void reform() const noexcept;
    [[nodiscard]] b2ShapeId getFootpawSenorId() const noexcept;
    [[nodiscard]] bool getFootpawSensorStatus() const noexcept;
    void addContactEvent() noexcept;
    void removeContactEvent() noexcept;
};

#endif //PLAYER_H