//
// Created by DirgeWuff on 9/26/25.
//

#ifndef LAYER_H
#define LAYER_H

#include "../Utility/Enum.h"
#include "../Utility/Logging.h"

// Base layer class. Any type of object that can be placed into layer stack for use.
class Layer {
protected:
    layerType m_type{};
    bool m_isEnabled{};
public:
    Layer() {
        #ifdef DEBUG
            logDbg("Default Layer constructed at address: ", this);
        #endif
    };

    virtual ~Layer() {
        #ifdef DEBUG
                logDbg("Default Layer destroyed at address: ", this);
        #endif
    };

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
