//
// Created by DirgeWuff on 9/26/25.
//

#ifndef LAYER_H
#define LAYER_H

// Type of layer. Whether or not the layer can overlay/stack on another layer
enum class layerType {
    PRIMARY_LAYER,
    OVERLAY_LAYER
};

// Base layer class. Any type of object that can be placed into layer stack for use.
class Layer {
protected:
    layerType m_type;
    bool m_isEnabled;
public:
    Layer() = default;
    virtual ~Layer() = default;

    virtual void pollEvents() {}
    virtual void update() {}
    virtual void draw() {}

    void suspend() noexcept { if (m_isEnabled) m_isEnabled = false; }
    void resume() noexcept { if (!m_isEnabled) m_isEnabled = true; }
    virtual void destroy() {}

    [[nodiscard]] layerType& getType() noexcept { return m_type; }
    [[nodiscard]] bool isEnabled() const noexcept { return m_isEnabled; }
};

#endif //LAYER_H
