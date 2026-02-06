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
// Class definition for RectButton and its members.

#include "raylib.h"
#include "raymath.h"
#include "RectButton.h"
#include "../Utility/Globals.h"

namespace RE::Core {
    RectButton::RectButton(
        const float cornerX,
        const float cornerY,
        const float width,
        const float height,
        std::string&& text) :
            m_buttonText(std::move(text)),
            m_primaryColor({95, 25, 25, 255}),
            m_hoverColor({173, 28,28, 255}),
            m_fontColor(BLACK),
            m_fontSize(static_cast<int>(std::floor(height * g_buttonTextScaleFactor))),
            m_fontSpacing(1.0f),
            m_currentState(buttonStates::BUTTON_IDLE),
            m_previousState(buttonStates::BUTTON_IDLE)
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

    RectButton::~RectButton() {
        UnloadFont(m_buttonFont);

        #ifdef DEBUG
            logDbg("RectButton destroyed at address: ", this);
        #endif
    }

    RectButton::RectButton(RectButton&& other) noexcept :
        m_buttonFont(other.m_buttonFont),
        m_clickEvent(std::move(other.m_clickEvent)),
        m_buttonText(std::move(other.m_buttonText)),
        m_primaryRect(other.m_primaryRect),
        m_clickRect(other.m_clickRect),
        m_textSize(other.m_textSize),
        m_textPos(other.m_textPos),
        m_textPosClicked(other.m_textPosClicked),
        m_primaryColor(other.m_primaryColor),
        m_hoverColor(other.m_hoverColor),
        m_fontColor(other.m_fontColor),
        m_fontSize(other.m_fontSize),
        m_fontSpacing(other.m_fontSpacing),
        m_currentState(other.m_currentState),
        m_previousState(other.m_previousState)
    {
        #ifdef DEBUG
            logDbg("Move called on RectButton, new address: ", this);
        #endif
    }

    RectButton& RectButton::operator=(RectButton&& other) noexcept {
        if (this != &other) {
            this->m_buttonFont = other.m_buttonFont;
            this->m_clickEvent = std::move(other.m_clickEvent);
            this->m_buttonText = std::move(other.m_buttonText);
            this->m_primaryRect = other.m_primaryRect;
            this->m_clickRect = other.m_clickRect;
            this->m_textSize = other.m_textSize;
            this->m_textPos = other.m_textPos;
            this->m_textPosClicked = other.m_textPosClicked;
            this->m_primaryColor = other.m_primaryColor;
            this->m_hoverColor = other.m_hoverColor;
            this->m_fontColor = other.m_fontColor;
            this->m_fontSize = other.m_fontSize;
            this->m_fontSpacing = other.m_fontSpacing;
            this->m_currentState = other.m_currentState;
            this->m_previousState = other.m_previousState;
        }
        #ifdef DEBUG
                logDbg("Move assignment called on RectButton, new address: ", this);
        #endif

        return *this;
    }

    void RectButton::setCustomFont(
        const Font& font,
        const int fontSize,
        const float fontSpacing,
        const Color color) noexcept
    {
        if (!IsFontValid(font)) {
            logFatal("Font invalid: RectButton::setCustomFont(Args...)");
            return;
        }

        m_buttonFont = font;
        m_fontSize = fontSize;
        m_fontSpacing = fontSpacing;
        m_fontColor = color;
    }

    void RectButton::setClickEvent(std::function<void()> event) noexcept {
        m_clickEvent = std::move(event);
    }

    void RectButton::setColor(const Color& primaryColor, const Color& hoverColor) noexcept {
        m_primaryColor = primaryColor;
        m_hoverColor = hoverColor;
    }

    void RectButton::update() {
        const Vector2 mousePos = GetMousePosition(); // NOLINT
        const bool isPrimaryHovered = CheckCollisionPointRec(mousePos, m_primaryRect);
        const bool isSecondaryHovered = CheckCollisionPointRec(mousePos, m_clickRect);
        const bool isMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        if (!isPrimaryHovered) {
            m_previousState = m_currentState;
            m_currentState = buttonStates::BUTTON_IDLE;
        }
        else if (isPrimaryHovered && !isMouseDown && m_previousState != buttonStates::BUTTON_HELD) { // NOLINT
            m_previousState = m_currentState;
            m_currentState = buttonStates::BUTTON_HOVERED;
        }
        else if (isSecondaryHovered && isMouseDown) {
            m_previousState = m_currentState;
            m_currentState = buttonStates::BUTTON_HELD;
        }
        else if (m_previousState == buttonStates::BUTTON_HELD) {
            m_previousState = m_currentState;
            m_currentState = buttonStates::BUTTON_RELEASED;

            if (m_clickEvent) {
                m_clickEvent();
            }
        }
    }

    void RectButton::draw() const {
        switch (m_currentState) {
            case buttonStates::BUTTON_IDLE: {
                DrawRectangleRec(m_primaryRect, m_primaryColor);
                DrawTextEx(
                    m_buttonFont,
                    m_buttonText.c_str(),
                    m_textPos,
                    static_cast<float>(m_fontSize),
                    m_fontSpacing,
                    m_fontColor);

                break;
            }
            case buttonStates::BUTTON_HOVERED: {
                DrawRectangleRec(m_primaryRect, m_hoverColor);
                DrawTextEx(
                    m_buttonFont,
                    m_buttonText.c_str(),
                    m_textPos,
                    static_cast<float>(m_fontSize),
                    m_fontSpacing,
                    m_fontColor);

                break;
            }
            case buttonStates::BUTTON_HELD: {
                DrawRectangleRec(m_clickRect, m_hoverColor);
                DrawTextEx(
                    m_buttonFont,
                    m_buttonText.c_str(),
                    m_textPosClicked,
                    static_cast<float>(m_fontSize) * g_buttonHeightScaleFactor,
                    m_fontSpacing,
                    m_fontColor);

                break;
            }
            case buttonStates::BUTTON_RELEASED: {
                DrawRectangleRec(m_primaryRect, m_hoverColor);
                DrawTextEx(
                    m_buttonFont,
                    m_buttonText.c_str(),
                    m_textPos,
                    static_cast<float>(m_fontSize),
                    m_fontSpacing,
                    m_fontColor);

                break;
            }
            default: break;
        }
    }
}