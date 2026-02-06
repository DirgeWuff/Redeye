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
// Class definition for DeathMenuLayer and its member functions.

#include "DeathMenuLayer.h"
#include "../../Core/Backend/LayerManager.h"
#include "../../Core/Backend/Program.h"

namespace RE::Application {
    DeathMenuLayer::DeathMenuLayer(const std::shared_ptr<Core::Player>& player) :
        m_continueBtn(Core::RectButton(
            600.0f,
            300.0f,
            300.0f,
            75.0f,
            "Continue...")),
        m_quitBtn(Core::RectButton(
            600.0f,
            400.0f,
            300.0f,
            75.0f,
            "Quit...")),
        m_headerFont(LoadFontEx(
            "../assets/Fonts/penakut.ttf",
            400,
            NULL,
            0)),
        m_backgroundOpacity(0)
    {
        if (!IsFontValid(m_headerFont)) {
            Core::logFatal("Error loading font: DeathMenuLayer(Args...)");
            return;
        }

        m_type = Core::layerType::PRIMARY_LAYER;
        m_isEnabled = true;

        m_continueBtn.setColor(
            {122, 122, 122, 255},
            {183, 183, 183, 255});

        m_continueBtn.setClickEvent([player] {
            player->reform(Core::loadGame());
        });

        m_quitBtn.setColor(
            {61, 61, 61, 255},
            {122, 122, 122, 255});

        m_quitBtn.setClickEvent([] {
            RE::Core::Program::getInstance().shutdown();
        });
    }

    DeathMenuLayer::~DeathMenuLayer() {
        #ifdef DEBUG
                std::cout << "DeathMenuLayer object destroyed at address: " << this << std::endl;
        #endif

        UnloadFont(m_headerFont);
    }

    void DeathMenuLayer::update() {
        m_continueBtn.update();
        m_quitBtn.update();
    }

    void DeathMenuLayer::draw() {
        DrawRectangle(
            0,
            0,
            GetScreenWidth(),
            GetScreenHeight(),
            {255, 0, 0, m_backgroundOpacity});
        DrawTextEx(
            m_headerFont,
            "Dead.",
            {575.0f, 50.0f},
            200.0f,
            3.0f,
            BLACK);

        m_backgroundOpacity += 3;

        m_continueBtn.draw();
        m_quitBtn.draw();
    }
}