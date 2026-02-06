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
#include "RectButton.h"

namespace RE::Core {
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
        m_primaryRect(std::move(other.m_primaryRect)),
        m_clickRect(std::move(other.m_clickRect)),
        m_textSize(other.m_textSize),
        m_textPos(other.m_textPos),
        m_textPosClicked(other.m_textPosClicked),
        m_primaryColor(other.m_primaryColor),
        m_hoverColor(other.m_hoverColor),
        m_fontColor(other.m_fontColor),
        m_fontSize(other.m_fontSize),
        m_fontSpacing(other.m_fontSpacing),
        m_currentState(other.m_currentState)
    {
        #ifdef DEBUG
            logDbg("Move called on RectButton, new address: ", this);
        #endif
    }

    RectButton& RectButton::operator=(RectButton&& other) noexcept {
        if (this != &other) {
            this->m_buttonFont = std::move(other.m_buttonFont);
            this->m_clickEvent = std::move(other.m_clickEvent);
            this->m_buttonText = std::move(other.m_buttonText);
            this->m_primaryRect = std::move(other.m_primaryRect);
            this->m_clickRect = std::move(other.m_clickRect);
            this->m_textSize = other.m_textSize;
            this->m_textPos = other.m_textPos;
            this->m_textPosClicked = other.m_textPosClicked;
            this->m_primaryColor = other.m_primaryColor;
            this->m_hoverColor = other.m_hoverColor;
            this->m_fontColor = other.m_fontColor;
            this->m_fontSize = other.m_fontSize;
            this->m_fontSpacing = other.m_fontSpacing;
            this->m_currentState = other.m_currentState;
        }
        #ifdef DEBUG
                logDbg("Move assignment called on RectButton, new address: ", this);
        #endif

        return *this;
    }

    void RectButton::setClickEvent(std::function<void()> event) noexcept {
        m_clickEvent = std::move(event);
    }

    void RectButton::setColor(Color primaryColor, Color hoverColor) noexcept {
        m_primaryColor = std::move(primaryColor);
        m_hoverColor = std::move(hoverColor);
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
                    m_fontSize,
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
                    m_fontSize,
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
                    m_fontSize * g_buttonHeightScaleFactor,
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
                    m_fontSize,
                    m_fontSpacing,
                    m_fontColor);

                break;
            }
            default: break;
        }
    }
}