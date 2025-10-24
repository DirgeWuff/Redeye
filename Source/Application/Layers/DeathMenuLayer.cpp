//
// Created by DirgeWuff on 10/2/25.
//

#include "DeathMenuLayer.h"
#include "../../Core/Backend/LayerManager.h"
#include "../../Core/Backend/Application.h"

DeathMenuLayer::DeathMenuLayer(const std::shared_ptr<Player>& player) :
    m_continueBtn(RectButton(
        600.0f,
        300.0f,
        300.0f,
        75.0f,
        "Continue...")),
    m_quitBtn(RectButton(
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
        logErr("Error loading font: DeathMenuLayer(Args...)");
        return;
    }

    m_type = layerType::PRIMARY_LAYER;
    m_isEnabled = true;

    m_continueBtn.setColor(
        {122, 122, 122, 255},
        {183, 183, 183, 255});

    m_continueBtn.setClickEvent([player] {
        player->reform(loadGame());
    });

    m_quitBtn.setColor(
        {61, 61, 61, 255},
        {122, 122, 122, 255});

    m_quitBtn.setClickEvent([] {
        Application::getInstance().shutdown();
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