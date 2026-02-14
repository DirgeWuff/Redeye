//
// Author: DirgeWuff
// Created on: 4/29/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Main player class declaration for the game and player constructor definition.
// This class is one of the core classes in GameLayer, and defines an interface
// for the player to be controlled by the user, and queried by other modules
// that rely on it or its data.

#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <vector>
#include "../../Core/Phys/BoxBody.h"
#include "../../Core/Event/EventCollider.h"
#include "../../Core/Utility/Utils.h"
#include "../../Core/Serialization/Save.h"
#include "../../Core/Renderer/Animation.h"
#include "../../Core/Renderer/AnimationManager.h"

namespace RE::Core {
    class SceneCamera;

    class Player final : public BoxBody, public std::enable_shared_from_this<Player> {
        b2Polygon m_footpawSensorBox{};
        std::vector<std::unique_ptr<animationDescriptor>> m_playerAnimations{};
        AnimationManager m_animationManager{};
        b2ShapeDef m_footpawSensorShape{};
        std::string m_playerSpritePath{};
        std::unique_ptr<sensorInfo> m_footpawSensorInfo{};
        b2ShapeId m_footpawSensorId{};
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
        [[nodiscard]] b2Vec2 getGroundNormals(const b2WorldId& world) const;
    public:
        Player() = default;
        Player(
            float centerX,
            float centerY,
            b2WorldId world,
            std::shared_ptr<AudioManager> manager);

        ~Player() override;

        Player(const Player&) = delete;
        Player(Player&&) noexcept = delete;
        Player& operator=(const Player&) = delete;
        Player& operator=(Player&&) noexcept = delete;

        void pollEvents();
        void update(const b2WorldId& world);
        void draw() const override;
        void murder();
        void reform(const saveData& save);
        void addContactEvent() noexcept;
        void removeContactEvent() noexcept;

        [[nodiscard]] sensorInfo getFootpawSensorInfo() const noexcept;
        [[nodiscard]] b2ShapeId getFootpawSenorId() const noexcept; // Do we ever use this???
        [[nodiscard]] bool isOnGround() const noexcept;
        [[nodiscard]] bool isDead() const noexcept;
        [[nodiscard]] direction getPlayerDirection() const noexcept;
        [[nodiscard]] animationId getCurrentAnimId() const noexcept;
        [[nodiscard]] entityActionState getCurrentActionState() const noexcept;
    };
}

#endif //PLAYER_H