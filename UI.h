//
// Created by DirgeWuff on 8/14/2025.
//

#ifndef UI_H
#define UI_H

#include <functional>
#include <memory>
#include <string>

#include "Camera.h"
#include "external_libs/Raylib/include/raylib.h"

class RectButton {
    Rectangle m_primaryRect;
    Rectangle m_clickRect;
    Color m_primaryColor;
    Color m_hoverColor;
    const std::string m_buttonText;
    int m_fontSize;
    float m_fontSpacing;
    Color m_fontColor;
    Font m_buttonFont;
    std::function<void()> m_clickEvent{};
public:

    RectButton(
        float cornerX,
        float cornerY,
        float width,
        float height,
        std::string text,
        const std::unique_ptr<SceneCamera>& camera);

    RectButton() = delete;
    ~RectButton() = default;
    void setClickEvent(std::function<void()> event) noexcept;     // Write overloads to this as needed
    void setColor(Color primaryColor, Color hoverColor) noexcept;
    void setCustomFont(Font font, int fontSize, float fontSpacing, Color color) noexcept;
    void draw(const std::unique_ptr<SceneCamera>& camera) const; // Update/draw functionality baked into one for simplicity.

};

#endif //UI_H
