//
// Created by DirgeWuff on 4/29/25.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include "raylib.h"
#include "../../Core/Phys/BoxBody.h"
#include "../../Core/Audio/Audio.h"
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Utility/Utils.h"
#include "../../Core/Serialization/Save.h"

// Keeping these separate because it's easier
enum class directions : std::uint8_t {
    RIGHT,
    LEFT
};

enum class playerStates : std::uint8_t {
    ON_GROUND,
    IN_AIR,
    DEAD
};

class SceneCamera;

class Player final : public BoxBody, public std::enable_shared_from_this<Player> {
    b2Polygon m_footpawSensorBox{};
    b2ShapeDef m_footpawSensorShape{};
    std::string m_playerSpritePath{};
    std::vector<Sound> m_footstepSounds{};
    std::vector<Sound> m_landingSounds{};
    Texture2D m_walkSprites{};
    Rectangle m_spriteRect{};
    b2ShapeId m_footpawSensorId{};
    std::unique_ptr<sensorInfo> m_footpawSensorInfo{};
    uint16_t m_activeGroundContacts{};
    uint8_t m_numFrames{};
    uint8_t m_frameDelayAmount{};
    uint8_t m_soundDelayAmount{};
    uint8_t m_frameIndex{};
    uint8_t m_frameDelayClock{};
    uint8_t m_soundDelayClock{};
    directions m_lastDirection{};
    playerStates m_currentState{};
public:
    Player() = default;

    template<typename T>
    Player(
        const float centerX,
        const float centerY,
        const b2WorldId world,
        T&& spritePath) :
            m_playerSpritePath(std::string(std::forward<T>(spritePath))),
            m_numFrames(3),
            m_frameDelayAmount(20),
            m_soundDelayAmount(m_frameDelayAmount * 3),
            m_lastDirection(directions::RIGHT),
            m_currentState(playerStates::ON_GROUND)
{
        // Raylib/general stuff
        m_walkSprites = LoadTexture(m_playerSpritePath.c_str());
        if (!IsTextureValid(m_walkSprites))
            logErr(
                std::string("Cannot load player sprites: ") + std::string(spritePath) +
                std::string(". Player::Player()"));

        m_spriteRect = {
            0.0f,
            static_cast<float>(m_walkSprites.height) / 2,
            static_cast<float>(m_walkSprites.width) / 3,
            static_cast<float>(m_walkSprites.height) / 2
        };

        m_sizePx = {m_walkSprites.width / 3.0f, m_walkSprites.height / 2.0f};
        m_sizeMeters = pixelsToMetersVec(m_sizePx);
        m_centerPosition = {pixelsToMeters(centerX), pixelsToMeters(centerY)};
        m_cornerPosition ={centerX - m_sizePx.x / 2, centerY - m_sizePx.y / 2};

        m_footstepSounds = loadSoundVector("../assets/Player assets/Sounds/Walk", 0.2f);
        m_landingSounds = loadSoundVector("../assets/Player assets/Sounds/Jump", 0.2f);

        m_bodyDef = b2DefaultBodyDef();
        m_bodyDef.position = m_centerPosition;
        m_bodyDef.type = b2_dynamicBody;
        m_bodyDef.fixedRotation = true;
        m_bodyDef.linearDamping = 8.0f;
        m_body = b2CreateBody(world, &m_bodyDef);

        const b2Capsule boundingCapsule = {
            pixelsToMetersVec(Vector2(0.0f, -20.0f)),
            pixelsToMetersVec(Vector2(0.0f, 32.0f)),
            pixelsToMeters(28.0f)
        };

        m_shapeDef = b2DefaultShapeDef();
        m_shapeDef.material.friction = 0.50f;
        m_shapeDef.material.restitution = 0.0f;
        m_shapeDef.density = 8.0f;
        b2CreateCapsuleShape(m_body, &m_shapeDef, &boundingCapsule);

        // Footpaw sensor :3
        m_footpawSensorBox = b2MakeOffsetBox(
            pixelsToMeters(20.0f),
            pixelsToMeters(12.0f),
            {0.0f, m_sizeMeters.y / 3.0f},
            b2MakeRot(0.0f)
        );

        m_footpawSensorShape = b2DefaultShapeDef();
        m_footpawSensorShape.isSensor = true;
        m_footpawSensorShape.enableSensorEvents = true;

        // Set our userData for the EventDispatcher to use later
        m_footpawSensorInfo = std::make_unique<sensorInfo>("pawbs");
        m_footpawSensorShape.userData = static_cast<void*>(m_footpawSensorInfo.get());

        m_footpawSensorId = b2CreatePolygonShape(
            m_body,
            &m_footpawSensorShape,
            &m_footpawSensorBox);

        #ifdef DEBUG
            std::cout << "Player object created object at address: " << this << std::endl;
        #endif
}
    Player(const Player&) = delete;
    Player(Player&&) noexcept = delete;
    Player& operator=(const Player&) = delete;
    Player& operator=(Player&&) noexcept = delete;

    ~Player() override;

    void update() override;
    void draw() const override;
    void moveRight();
    void moveLeft();
    void jump() const;
    void moveNowhere();
    void murder();
    void reform(const saveData& save);
    [[nodiscard]] b2ShapeId getFootpawSenorId() const noexcept;
    [[nodiscard]] bool isOnGround() const noexcept;
    [[nodiscard]] bool isDead() const noexcept;
    [[nodiscard]] directions getPlayerDirection() const noexcept;
    void addContactEvent() noexcept;
    void removeContactEvent() noexcept;
};

#endif //PLAYER_H