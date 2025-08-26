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
    void setClickEvent(std::function<void()> event) noexcept;                                       // Set the RectButton's click event
    void setColor(Color primaryColor, Color hoverColor) noexcept;                                   // Set the primary and hover color of the RectButton
    void setCustomFont(Font font, int fontSize, float fontSpacing, Color color) noexcept;           // Set a custom font with parameters
    void draw(const std::unique_ptr<SceneCamera>& camera) const;                                    // Update/draw functionality baked into one for simplicity.
};

// Base class for any number of UI elements or objects
class UIElement {
protected:
    float m_posX{};     // X position of the element
    float m_posY{};     // Y position of the element
    bool m_active{};    // Should the element be drawn?
public:
    UIElement() = default;
    virtual ~UIElement() = default;

    // Return whether the element is active in our scene and should be drawn on a draw call.
    [[nodiscard]] bool getStatus() const noexcept {
        return m_active;
    };

    virtual void update() = 0;
    virtual void draw() const = 0;
};

// Text "alert" class, displays text for a timed period in the corner of screen.
class TextAlert final : public UIElement {
    float m_duration;           // How long the text should be displayed for.
    float m_elapsedTime;        // How the text has been displayed.
    float m_fontSize;           // Size of the font. 30.0f by default.
    float m_fontSpacing;        // Font spacing. 5.0f by default.
    Font m_font;                // Font. JetBrainsMono-Bold by default.
    std::string m_message;      // Message to be displayed.
public:
    TextAlert(std::string text, float duration);

    void update() override;         // Update the TextAlert. Must be called to work correctly.
    void draw() const override;     // Draw the text element if it is active.
};

class UIHandler {
    std::vector<std::unique_ptr<UIElement>> m_activeElements{};     // All the active UIElements in the Scene.
public:
    UIHandler() = default;
    ~UIHandler() = default;

    void addElement(std::unique_ptr<UIElement> newElement);     // Add a new element to the scene.
    void updateUI();                                            // Update all the active elements in the scene remove those that are no longer active.
    void drawUI() const;                                        // Draw all active elements in the Scene.
};

#endif //UI_H
