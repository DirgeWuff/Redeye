//
// Created by DirgeWuff on 8/14/2025.
//

#ifndef UI_H
#define UI_H

#include <functional>
#include <memory>
#include <string>
#include "raylib.h"
#include "Camera.h"

class RectButton {
    Rectangle m_primaryRect;                    // Main rectangle used by the RectButton.
    Rectangle m_clickRect;                      // Rectangle used by the RectButton when clicked.
    Color m_primaryColor;                       // Primary color of the RectButton. Uses this color when not hovered over.
    Color m_hoverColor;                         // Color of the RectButton when hovered over.
    const std::string m_buttonText;             // Text displayed by RectButton.
    int m_fontSize;                             // Font size of the RectButton's text. 30% of the button's height by default.
    float m_fontSpacing;                        // Spacing of the RectButton's font. 1.0f by default.
    Color m_fontColor;                          // Color of the RectButton's font. Black by default.
    Font m_buttonFont;                          // Font of the RectButton's text. JetBrainsMono-Bold by default.
    std::function<void()> m_clickEvent{};       // Function executed when the RectButton is clicked. NULL by default.
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
    // Write overloads to this as needed
    void setClickEvent(std::function<void()> event) noexcept;                               // Set the RectButton's click event
    void setColor(Color primaryColor, Color hoverColor) noexcept;                           // Set the primary and hover color of the RectButton
    void setCustomFont(Font font, int fontSize, float fontSpacing, Color color) noexcept;   // Set custom font parameters for a RectButton
    void draw(const std::unique_ptr<SceneCamera>& camera) const;                            // Update/draw functionality baked into one for simplicity.

};

#endif //UI_H
