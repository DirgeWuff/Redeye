//
// Author: DirgeWuff
// Created on: 8/14/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// A collection of UI-related classes, including text and buttons.
// These objects are used to create the menus and popups in the game.

#ifndef UI_H
#define UI_H

#include <functional>
#include <memory>
#include <string>
#include "raylib.h"
#include "raymath.h"
#include "../Camera/Camera.h"
#include "../Backend/Layer.h"
#include "../Utility/Globals.h"
#include "../Utility/Enum.h"

// Animated rectangular button, based on Raylib Rectangle
class RectButton {
    Font m_buttonFont;
    std::function<void()> m_clickEvent{};
    const std::string m_buttonText;
    Rectangle m_primaryRect;
    Rectangle m_clickRect;
    Vector2 m_textSize;
    Vector2 m_textPos;
    Vector2 m_textPosClicked;
    buttonStates m_state;
    Color m_primaryColor;
    Color m_hoverColor;
    Color m_fontColor;
    int m_fontSize;
    float m_fontSpacing;
public:
    RectButton() = delete;
    ~RectButton();

    template<typename T>
    RectButton(
        float cornerX,
        float cornerY,
        float width,
        float height,
        T&& text) :
            m_buttonText(std::string(std::forward<T>(text))),
            m_state(buttonStates::BUTTON_IDLE),
            m_primaryColor({95, 25, 25, 255}),
            m_hoverColor({173, 28,28, 255}),
            m_fontColor(BLACK),
            m_fontSize(static_cast<int>(std::floor(height * g_buttonTextScaleFactor))),
            m_fontSpacing(1.0f)
{
    m_primaryRect = {cornerX, cornerY, width, height},
    m_clickRect = {
        cornerX + cornerX * g_buttonPosXScaleFactor / 2.0f,
        cornerY + cornerY * g_buttonPosYScaleFactor / 2.0f,
        width * g_buttonWidthScaleFactor,
        height * g_buttonHeightScaleFactor};

    m_buttonFont = LoadFontEx(
        "../assets/Fonts/JetBrainsMono-Bold.ttf",
        m_fontSize * 2,
        NULL,
        0);

    if (!IsFontValid(m_buttonFont)) {
        logFatal("Error loading button font: RectButton(Args...)");
        return;
    }

    m_textSize = MeasureTextEx(
        m_buttonFont,
        m_buttonText.c_str(),
        m_fontSize,
        m_fontSpacing);

    m_textPos = {
        m_primaryRect.x + m_primaryRect.width / 2.0f - m_textSize.x / 2.0f,
        m_primaryRect.y + m_primaryRect.height / 2.0f - m_textSize.y / 2.0f};

    m_textPosClicked = m_textPos + Vector2Multiply(
        m_textPos,
        {g_buttonPosXScaleFactor, g_buttonPosYScaleFactor}) / 2.0f;

    #ifdef DEBUG
        logDbg("RectButton constructed at address: ", this);
    #endif
}

    RectButton(const RectButton& other) = delete;
    RectButton& operator=(const RectButton& other) = delete;

    template<typename F>
    void setCustomFont(
    F&& font,
    const int fontSize,
    const float fontSpacing,
    const Color color) noexcept
    {
        if (!IsFontValid(font)) {
            logFatal("Font invalid: RectButton::setCustomFont(Args...)");
            return;
        }

        m_buttonFont = std::forward<F>(font);
        m_fontSize = fontSize;
        m_fontSpacing = fontSpacing;
        m_fontColor = std::move(color);
    }

    void setClickEvent(std::function<void()> event) noexcept;
    void setColor(Color primaryColor, Color hoverColor) noexcept;
    void update();
    void draw() const;
};

// Text alert, used to display text for a duration on the screen, such as when a checkpoint is reached
class TextAlert final : public Layer {
    Font m_font;
    std::string m_message;
    Vector2 m_position;
    float m_duration;
    float m_elapsedTime;
    float m_fontSize;
    float m_fontSpacing;
public:
    ~TextAlert() override;

    template<typename T>
    TextAlert(T&& text, const float duration) :
        m_message(std::string(std::forward<T>(text))),
        m_position({30.0f, 30.0f}),
        m_duration(duration),
        m_elapsedTime(0.0f),
        m_fontSize(30.0f),
        m_fontSpacing(2.0f)
{
    m_type = layerType::OVERLAY_LAYER;
    m_isEnabled = true;

    m_font = LoadFontEx(
        "../assets/Fonts/penakut.ttf",
        static_cast<int>(m_fontSize),
        NULL,
        0);

    if (!IsFontValid(m_font)) {
        logFatal("Error loading font: TextAlert(Args...)");
        return;
    }
}
    TextAlert(const TextAlert&) = delete;
    TextAlert(const TextAlert&&) = delete;



    void update() override;
    void draw()  override;
};

#endif //UI_H
