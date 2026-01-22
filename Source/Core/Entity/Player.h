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
#include "../../Core/Utility/Globals.h"
#include "../../Core/Renderer/Animation.h"
#include "../../Core/Renderer/AnimationManager.h"

class SceneCamera;

// TODO: Load all this from TOML cfg file instead of hard coding
static constexpr std::array<animationDescriptor, 8> playerAnimations =  {{
    {
        {2, 1},
        {8, 1},
        {108.0f, 144.0f},
        0.1f,
        playbackType::LOOP,
        animationId::PLAYER_WALK_LEFT
    },

    {
        {2, 2},
        {8, 2},
        {108.0f, 144.0f},
        0.1f,
        playbackType::LOOP,
        animationId::PLAYER_WALK_RIGHT
    },

    {
        {1, 1},
        {1, 1},
        {108.0f, 144.0f},
        0.1f,
        playbackType::SINGLE_FRAME,
        animationId::PLAYER_IDLE_LEFT
    },

    {
        {1, 2},
        {1, 2},
        {108.0f, 144.0f},
        0.1f,
        playbackType::SINGLE_FRAME,
        animationId::PLAYER_IDLE_RIGHT
    },

    {
        {1, 3},
        {2, 3},
        {108.0f, 144.0f},
        0.1f,
        playbackType::NON_LOOPING,
        animationId::PLAYER_JUMP_LEFT
    },

    {
        {3, 3},
        {4, 3},
        {108.0f, 144.0f},
        0.1f,
        playbackType::NON_LOOPING,
        animationId::PLAYER_FALL_LEFT
    },

    {
        {5, 3},
        {6, 3},
        {108.0f, 144.0f},
        0.1f,
        playbackType::NON_LOOPING,
        animationId::PLAYER_JUMP_RIGHT
    },

    {
        {7, 3},
        {8, 3},
        {108.0f, 144.0f},
        0.1f,
        playbackType::NON_LOOPING,
        animationId::PLAYER_FALL_RIGHT
    }
}};

class Player final : public BoxBody, public std::enable_shared_from_this<Player> {
    b2Polygon m_footpawSensorBox{};
    AnimationManager m_animationManager{};
    b2ShapeDef m_footpawSensorShape{};
    std::string m_playerSpritePath{};
    std::vector<Sound> m_footstepSounds{};
    std::vector<Sound> m_landingSounds{};
    b2ShapeId m_footpawSensorId{};
    std::unique_ptr<sensorInfo> m_footpawSensorInfo{};
    std::uint16_t m_activeGroundContacts{};
    std::uint8_t m_soundDelayClock{};
    std::int8_t m_movementIntent{};
    direction m_currentDirection{};
    entityActionState m_currentState{};
    animationId m_currentAnimId{};
    bool m_jumpIntent{};
    bool m_dead{};

    void moveRight(const b2WorldId& world) const;
    void moveLeft(const b2WorldId& world) const;
    void jump() const;
public:
    Player() = default;

    template<typename T>
    Player(
        const float centerX,
        const float centerY,
        const b2WorldId world,
        T&& spritePath) :
            m_animationManager(
                spritePath,
                playerAnimations,
                animationId::PLAYER_IDLE_RIGHT),
            m_playerSpritePath(std::string(std::forward<T>(spritePath))),
            m_currentDirection(direction::RIGHT),
            m_currentState(entityActionState::IDLE)
{
        m_sizePx = m_animationManager.getSpriteSize();
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
        m_shapeDef.filter.categoryBits = g_playerCategoryBits;
        m_shapeDef.filter.maskBits = g_universalMaskBits;   // Using this for now, may change later...
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
        m_footpawSensorShape.filter.categoryBits = g_footpawCategoryBits;
        m_footpawSensorShape.filter.maskBits = g_universalMaskBits;

        try {
            // Set our userData for the EventDispatcher to use later
            m_footpawSensorInfo = std::make_unique<sensorInfo>("pawbs");
            m_footpawSensorShape.userData = static_cast<void*>(m_footpawSensorInfo.get());
        }
        catch (const std::exception& e) {
            logFatal("Failed to allocate for m_footpawSensorInfo: " + std::string(e.what()) +
                std::string(". Player::Player(Args...)"));

            return;
        }
        catch (...) {
            logFatal("An unknown error occurred. Player::Player(Args...)");
            return;
        }

        m_footpawSensorId = b2CreatePolygonShape(
            m_body,
            &m_footpawSensorShape,
            &m_footpawSensorBox);

        m_currentAnimId = m_animationManager.getCurrentAnimId();

        #ifdef DEBUG
            logDbg("Class player constructed at address: ", this);
        #endif
}
    Player(const Player&) = delete;
    Player(Player&&) noexcept = delete;
    Player& operator=(const Player&) = delete;
    Player& operator=(Player&&) noexcept = delete;

    ~Player() override;

    void pollEvents();
    void update(const b2WorldId& world);
    void draw() const override;
    void murder();
    void reform(const saveData& save);
    void addContactEvent() noexcept;
    void removeContactEvent() noexcept;
    [[nodiscard]] b2Vec2 getGroundNormals(const b2WorldId& world) const;

    [[nodiscard]] b2ShapeId getFootpawSenorId() const noexcept;
    [[nodiscard]] bool isOnGround() const noexcept;
    [[nodiscard]] bool isDead() const noexcept;
    [[nodiscard]] direction getPlayerDirection() const noexcept;
    [[nodiscard]] animationId getCurrentAnimId() const noexcept;
    [[nodiscard]] entityActionState getCurrentActionState() const noexcept;
};

#endif //PLAYER_H