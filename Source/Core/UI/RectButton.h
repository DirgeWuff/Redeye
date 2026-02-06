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
#include <string>
#include "raylib.h"
#include "../Utility/Enum.h"

namespace RE::Core {
    class RectButton {
        Font m_buttonFont;
        std::function<void()> m_clickEvent;
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
        buttonStates m_currentState;
        buttonStates m_previousState;
    public:
        RectButton() = delete;
        ~RectButton();

        RectButton(
            float cornerX,
            float cornerY,
            float width,
            float height,
            std::string&& text);

        RectButton(const RectButton&) = delete;
        RectButton(RectButton&& other) noexcept;
        RectButton& operator=(const RectButton&) = delete;
        RectButton& operator=(RectButton&& other) noexcept;

        void setCustomFont(
        const Font& font,
        int fontSize,
        float fontSpacing,
        Color color) noexcept;

        void setClickEvent(std::function<void()> event) noexcept;
        void setColor(const Color& primaryColor, const Color& hoverColor) noexcept;
        void update();
        void draw() const;
    };
}

#endif //UI_H
