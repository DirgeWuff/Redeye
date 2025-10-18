//
// Created by DirgeWuff on 10/2/25.
//

#ifndef DEATHLAYER_H
#define DEATHLAYER_H

#include "raylib.h"
#include "../../Core/Backend/Layer.h"
#include "../../Core/UI/UI.h"
#include "../../Core/Entity/Player.h"

class DeathMenuLayer final : public Layer {
    RectButton m_continueBtn;
    RectButton m_quitBtn;
    Font m_headerFont;
    unsigned char m_backgroundOpacity;
public:
    DeathMenuLayer() = delete;
    explicit DeathMenuLayer(const std::shared_ptr<Player>& player);
    ~DeathMenuLayer() override;

    void update() override;
    void draw() override;
};

#endif //DEATHLAYER_H
