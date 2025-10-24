//
// Created by DirgeWuff on 10/1/25.
//

#include "StartMenuLayer.h"
#include "GameLayer.h"
#include "../../Core/Backend/Application.h"
#include "../../Core/Backend/LayerManager.h"

StartMenuLayer::StartMenuLayer() :
    m_startButton(
        600.0f,
        300.0f,
        300.0f,
        75.0f,
        "Start..."),
    m_quitButton(
        600.0f,
        400.0f,
        300.0f,
        75.0f,
        "Exit game..."),
    m_headerFont(LoadFontEx(
        "../assets/Fonts/Jo_wrote_a_lovesong.ttf",
        400.0f,
        NULL,
        0))
{
    if (!IsFontValid(m_headerFont)) {
        logErr("Cannot load header font: StartMenuLayer::StartMenuLayer()");
        return;
    }

    m_type = layerType::PRIMARY_LAYER;
    m_isEnabled = true;
    m_startButton.setColor(
        {122, 122, 122, 255},
        {183, 183, 183, 255});

    m_startButton.setClickEvent([] {
            LayerManager::getInstance().resumeLayer(layerKey::GAME_LAYER);
            LayerManager::getInstance().requestLayerPop(layerKey::START_MENU);
    });

    m_quitButton.setColor(
        {61, 61,61, 255},
        {122, 122,122, 255});

    m_quitButton.setClickEvent([] {
            Application::getInstance().shutdown();
        });
}

StartMenuLayer::~StartMenuLayer() {
    UnloadFont(m_headerFont);
}

void StartMenuLayer::update() {
    m_startButton.update();
    m_quitButton.update();
}

void StartMenuLayer::draw() {
    ClearBackground(BLACK);

    DrawTextEx(
        m_headerFont,
        "redeye",
        {525.0f, 50.0f},
        200.0f,
        3.0f,
        RED);

    m_startButton.draw();
    m_quitButton.draw();
}

