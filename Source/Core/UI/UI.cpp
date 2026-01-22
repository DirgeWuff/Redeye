//
// Created by DirgeWuff on 8/14/2025.
//

#include "raylib.h"
#include "UI.h"

// Buttons
// =====================================================================================================================
RectButton::~RectButton() {
    UnloadFont(m_buttonFont);

    #ifdef DEBUG
        logDbg("RectButton destroyed at address: ", this);
    #endif
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

TextAlert::~TextAlert() {
    UnloadFont(m_font);

    #ifdef DEBUG
        logDbg("TextAlert destroyed at address: ", this);
    #endif
}

// Text
// =====================================================================================================================
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
