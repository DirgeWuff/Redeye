//
// Author: DirgeWuff
// Created on: 2/6/26 (Originally written 8/14/25)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Class definition for TextAlertLayer and it's members.

#include "TextAlertLayer.h"

namespace RE::Application {
    TextAlertLayer::~TextAlertLayer() {
        UnloadFont(m_font);

        #ifdef DEBUG
            logDbg("TextAlert destroyed at address: ", this);
        #endif
    }

    void TextAlertLayer::update() {
        const float deltaTime = GetFrameTime();

        if (m_isEnabled) {
            m_elapsedTime += deltaTime;

            if (m_elapsedTime >= m_duration) {
                m_isEnabled = false;
            }
        }
    }

    void TextAlertLayer::draw() {
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