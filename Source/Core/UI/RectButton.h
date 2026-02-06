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
// Animated, configurable rectangular button that can be used in game UI,
// and programmed to execute a std::function<void()> when clicked.

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

namespace RE::Core {
    class RectButton {
        Font m_buttonFont;
        std::function<void()> m_clickEvent{};
        std::string m_buttonText;
        Rectangle m_primaryRect;
        Rectangle m_clickRect;
        Vector2 m_textSize;
        Vector2 m_textPos;
        Vector2 m_textPosClicked;
        Color m_primaryColor;
        Color m_hoverColor;
        Color m_fontColor;
        int m_fontSize;
        float m_fontSpacing;
        buttonStates m_state;
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
                m_primaryColor({95, 25, 25, 255}),
                m_hoverColor({173, 28,28, 255}),
                m_fontColor(BLACK),
                m_fontSize(static_cast<int>(std::floor(height * g_buttonTextScaleFactor))),
                m_fontSpacing(1.0f),
                m_state(buttonStates::BUTTON_IDLE)
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
            NULL, // NOLINT
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

        RectButton(const RectButton&) = delete;
        RectButton(RectButton&& other) noexcept;
        RectButton& operator=(const RectButton&) = delete;
        RectButton& operator=(RectButton&& other) noexcept;

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
}

#endif //UI_H
