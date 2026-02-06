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
// Class/member function definitions for classes contained within UI.h

#include "raylib.h"
#include "UI.h"

namespace RE::Core {
    // Buttons
    // =====================================================================================================================
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
        m_state(other.m_state)
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
            this->m_state = other.m_state;
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
        const Vector2 mousePos = GetMousePosition();

        if (!CheckCollisionPointRec(mousePos, m_primaryRect)) {
            m_state = buttonStates::BUTTON_IDLE;
        }
        else if (CheckCollisionPointRec(mousePos, m_primaryRect) && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            m_state = buttonStates::BUTTON_HOVERED;
        }
        else {
            m_state = buttonStates::BUTTON_CLICKED;

            if (m_clickEvent) {
                m_clickEvent();
            }
        }
    }

    void RectButton::draw() const {
        if (m_state == buttonStates::BUTTON_IDLE) {
            DrawRectangleRec(m_primaryRect, m_primaryColor);
            DrawTextEx(
                m_buttonFont,
                m_buttonText.c_str(),
                m_textPos,
                m_fontSize,
                m_fontSpacing,
                m_fontColor);
        }
        else if (m_state == buttonStates::BUTTON_HOVERED) {
            DrawRectangleRec(m_primaryRect, m_hoverColor);
            DrawTextEx(
                m_buttonFont,
                m_buttonText.c_str(),
                m_textPos,
                m_fontSize,
                m_fontSpacing,
                m_fontColor);
        }
        else if (m_state == buttonStates::BUTTON_CLICKED) {
            DrawRectangleRec(m_clickRect, m_hoverColor);
            DrawTextEx(
                m_buttonFont,
                m_buttonText.c_str(),
                m_textPosClicked,
                m_fontSize * g_buttonHeightScaleFactor,
                m_fontSpacing,
                m_fontColor);
        }
    }

    // Text
    // =====================================================================================================================
    TextAlert::~TextAlert() {
        UnloadFont(m_font);

        #ifdef DEBUG
            logDbg("TextAlert destroyed at address: ", this);
        #endif
    }

    TextAlert::TextAlert(TextAlert&& other) noexcept :
        m_font(std::move(other.m_font)),
        m_message(std::move(other.m_message)),
        m_position(other.m_position),
        m_duration(other.m_duration),
        m_elapsedTime(other.m_elapsedTime),
        m_fontSize(other.m_fontSize),
        m_fontSpacing(other.m_fontSpacing)
    {
        this->m_type = other.m_type;
        this->m_isEnabled = other.m_isEnabled;

        other.m_font = {};

        #ifdef DEBUG
            logDbg("Move called on Text alert, new address: ", this);
        #endif
    }

    TextAlert& TextAlert::operator=(TextAlert&& other) noexcept {
        if (this != &other) {
            this->m_type = other.m_type;
            this->m_isEnabled = other.m_isEnabled;
            this->m_font = std::move(other.m_font);
            this->m_message = std::move(other.m_message);
            this->m_position = other.m_position;
            this->m_duration = other.m_duration;
            this->m_elapsedTime = other.m_elapsedTime;
            this->m_fontSize = other.m_fontSize;
            this->m_fontSpacing = other.m_fontSpacing;

            other.m_font = {};
        }

        #ifdef DEBUG
                logDbg("Move assignment called on TextAlert, new address: ", this);
        #endif

        return *this;
    }


    void TextAlert::update() {
        const float deltaTime = GetFrameTime();

        if (m_isEnabled) {
            m_elapsedTime += deltaTime;

            if (m_elapsedTime >= m_duration) {
                m_isEnabled = false;
            }
        }
    }

    void TextAlert::draw() {
        if (m_isEnabled) {
            DrawTextEx(
                m_font,
                m_message.c_str(),
                {m_position.x, m_position.y},
                m_fontSize,
                m_fontSpacing,
                RED);
        }
    }
}