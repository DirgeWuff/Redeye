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

    TextAlertLayer::TextAlertLayer(TextAlertLayer&& other) noexcept :
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

    TextAlertLayer& TextAlertLayer::operator=(TextAlertLayer&& other) noexcept {
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