//
// Created by DirgeWuff on 10/1/25.
//

#ifndef STARTMENU_H
#define STARTMENU_H

#include "raylib.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/UI/UI.h"

class StartMenuLayer final : public Layer {
    RectButton m_startButton;
    RectButton m_quitButton;
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

#endif //STARTMENU_H
