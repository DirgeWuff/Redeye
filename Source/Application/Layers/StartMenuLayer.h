//
// Author: DirgeWuff
// Created on: 10/1/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class declaration for the StartMenuLayer. This layer is shown when
// the game is first started, giving the player the opportunity to start
// the game or quit

#ifndef STARTMENU_H
#define STARTMENU_H

#include "raylib.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/UI/UI.h"

namespace RE::Application {
    class StartMenuLayer final : public Core::Layer {
        Core::RectButton m_startButton;
        Core::RectButton m_quitButton;
        Font m_headerFont{};
    public:
        StartMenuLayer();
        StartMenuLayer(const StartMenuLayer&) = delete;
        StartMenuLayer(const StartMenuLayer&&) = delete;
        StartMenuLayer& operator=(const StartMenuLayer&) = delete;
        StartMenuLayer& operator=(const StartMenuLayer&&) = delete;
        ~StartMenuLayer() override;

        void update() override;
        void draw() override;
    };
}

#endif //STARTMENU_H
