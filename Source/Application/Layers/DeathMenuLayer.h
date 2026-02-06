//
// Author: DirgeWuff
// Created on: 10/2/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class declaration for the death menu. Menu will appear when player is
// killed, allowing the player to either restart from the last checkpoint
// or quit the game

#ifndef DEATHLAYER_H
#define DEATHLAYER_H

#include "raylib.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/UI/RectButton.h"
#include "../../Core/Entity/Player.h"

namespace RE::Application {
    class DeathMenuLayer final : public Core::Layer {
        Core::RectButton m_continueBtn;
        Core::RectButton m_quitBtn;
        Font m_headerFont;
        unsigned char m_backgroundOpacity;
    public:
        DeathMenuLayer() = delete;
        explicit DeathMenuLayer(const std::shared_ptr<Core::Player>& player);
        ~DeathMenuLayer() override;

        DeathMenuLayer(const DeathMenuLayer&) = delete;
        DeathMenuLayer(DeathMenuLayer&&) noexcept = delete;
        DeathMenuLayer& operator=(const DeathMenuLayer&) = delete;
        DeathMenuLayer& operator=(DeathMenuLayer&&) noexcept = delete;

        void update() override;
        void draw() override;
    };
}

#endif //DEATHLAYER_H
