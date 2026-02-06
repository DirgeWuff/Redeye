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
// TextAlertLayer, used to display a text object to the player when
// an event occurs. This is an overlay layer to be drawn on top of
// GameLayer. Moved from UI.h

#ifndef TEXTALERT_H
#define TEXTALERT_H

#include <string>
#include "raylib.h"
#include "../../Core/Backend/Layer.h"

namespace RE::Application {
    // Text alert, used to display text for a duration on the screen, such as when a checkpoint is reached
    class TextAlertLayer final : public Core::Layer {
        Font m_font;
        std::string m_message;
        Vector2 m_position;
        float m_duration;
        float m_elapsedTime;
        float m_fontSize;
        float m_fontSpacing;
    public:
        ~TextAlertLayer() override;

        template<typename T>
        TextAlertLayer(T&& text, const float duration) :
            m_message(std::string(std::forward<T>(text))),
            m_position({30.0f, 30.0f}),
            m_duration(duration),
            m_elapsedTime(0.0f),
            m_fontSize(30.0f),
            m_fontSpacing(2.0f)
        {
            m_type = Core::layerType::OVERLAY_LAYER;
            m_isEnabled = true;

            m_font = LoadFontEx(
                "../assets/Fonts/penakut.ttf",
                static_cast<int>(m_fontSize),
                NULL, // NOLINT
                0);

            if (!IsFontValid(m_font)) {
                Core::logFatal("Error loading font: TextAlert(Args...)");
                return;
            }
        }
        TextAlertLayer(const TextAlertLayer&) = delete;
        TextAlertLayer(TextAlertLayer&& other) noexcept;
        TextAlertLayer(const TextAlertLayer&&) = delete;
        TextAlertLayer& operator=(TextAlertLayer&& other) noexcept;

        void update() override;
        void draw()  override;
    };
}

#endif //TEXTALERT_H
