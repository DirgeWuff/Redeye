//
// Created by DirgeWuff on 8/14/2025.
//


#include <cmath>
#include "../external_libs/Raylib/include/raylib.h"
#include "../external_libs/Raylib/include/raymath.h"
#include "../UI.h"


RectButton::RectButton(
    const float cornerX,
    const float cornerY,
    const float width,
    const float height,
    std::string text,
    const std::unique_ptr<SceneCamera>& camera) :
m_primaryColor(95, 25, 25, 255),
m_hoverColor(173, 28,28, 255),
m_buttonText(std::move(text)),
m_fontSize(static_cast<int>(std::floor(height * 0.30f))),
m_fontSpacing(1.0f),
m_fontColor(BLACK),
m_buttonFont(LoadFontEx(
    "../assets/Fonts/JetBrainsMono-Bold.ttf",
    m_fontSize * 2,
    nullptr,
    0))
{
    const Vector2 adjustedPos = GetScreenToWorld2D({cornerX, cornerY}, *camera->getCameraPtr());
    m_primaryRect = {adjustedPos.x, adjustedPos.y, width, height},
    m_clickRect = {
        adjustedPos.x + adjustedPos.x * 0.50f / 2,
        adjustedPos.y + adjustedPos.y * 0.004f / 2,
        width * 0.98f,
        height * 0.98f};
}

void RectButton::setClickEvent(std::function<void()> event) noexcept {
    m_clickEvent = std::move(event);
}

void RectButton::setColor(Color primaryColor, Color hoverColor) noexcept {
    m_primaryColor = std::move(primaryColor);
    m_hoverColor = std::move(hoverColor);
}

void RectButton::setCustomFont(
    const Font font,
    const int fontSize,
    const float fontSpacing,
    const Color color) noexcept
{
    m_buttonFont = std::move(font);
    m_fontSize = fontSize;
    m_fontSpacing = fontSpacing;
    m_fontColor = std::move(color);
}

void RectButton::draw(const std::unique_ptr<SceneCamera>& camera) const {
    const Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), *camera->getCameraPtr());

    const Vector2 textSize = MeasureTextEx(
        m_buttonFont,
        m_buttonText.c_str(),
        m_fontSize,
         1.0f);
    const Vector2 textPos = {
        m_primaryRect.x + m_primaryRect.width / 2.0f - textSize.x / 2.0f,
        m_primaryRect.y + m_primaryRect.height / 2.0f - textSize.y / 2.0f};
    const Vector2 textPosClicked = textPos + Vector2Multiply(textPos, {0.02f, 0.004f}) / 2;

    if (!CheckCollisionPointRec(mousePos, m_primaryRect)) {
        DrawRectangleRec(m_primaryRect, m_primaryColor);
        DrawTextEx(
            m_buttonFont,
            m_buttonText.c_str(),
            textPos,
            m_fontSize,
            m_fontSpacing,
            m_fontColor);
    }
    else {
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            DrawRectangleRec(m_primaryRect, m_hoverColor);
            DrawTextEx(
                m_buttonFont,
                m_buttonText.c_str(),
                textPos,
                m_fontSize,
                m_fontSpacing,
                BLACK);
        }
        else {
            DrawRectangleRec(m_clickRect, m_hoverColor);
            DrawTextEx(
                m_buttonFont,
                m_buttonText.c_str(),
                textPosClicked,
                m_fontSize * 0.98f,
                m_fontSpacing,
                BLACK);

            if (m_clickEvent) {
                m_clickEvent();
            }
        }
    }
}

