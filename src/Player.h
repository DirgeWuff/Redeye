//
// Created by DirgeWuff on 4/29/25.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include "raylib.h"
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

    void update() override;                                         // Update player
    void draw() const override;                                     // Draw player
    void moveRight() override;                                      // Move the player right
    void moveLeft() override;                                       // Move the player left
    void jump() override;                                           // Make the player jump
    void moveNowhere() override;                                    // Used to control the player's idle sprite
    void murder(const std::unique_ptr<SceneCamera>& camera);        // Kill the player
    void reform() const noexcept;                                   // Respawn the player
    [[nodiscard]] b2ShapeId getFootpawSenorId() const noexcept;     // Get the footpaw sensor id
    [[nodiscard]] bool getFootpawSensorStatus() const noexcept;     // Get the footpaw sensor status
    void addContactEvent() noexcept;                                // Add a player contact event to counter
    void removeContactEvent() noexcept;                             // Remove a player contact event from counter
};

#endif //PLAYER_H