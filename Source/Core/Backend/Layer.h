//
// Author: DirgeWuff
// Created on: 9/26/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Virtual/base class that all objects of type Layer inherit from.
// A layer is any piece of code that is designed to induce or modify
// program behavior. A layer is capable of polling for user events,
// updating itself, and being drawn to the screen.

#ifndef LAYER_H
#define LAYER_H

#include "../Utility/Enum.h"
#include "../Utility/Logging.h"

namespace RE::Core {
    class Layer {
    protected:
        layerType m_type{};
        bool m_isEnabled{};
    public:
        Layer() {
            #ifdef DEBUG
                        logDbg("Default Layer constructed at address: ", this);
            #endif
        }

        virtual ~Layer() {
            #ifdef DEBUG
                        logDbg("Default Layer destroyed at address: ", this);
            #endif
        }

        virtual void pollEvents() {}
        virtual void update() {}
        virtual void draw() {}

        void suspend() noexcept { if (m_isEnabled) m_isEnabled = false; }
        void resume() noexcept { if (!m_isEnabled) m_isEnabled = true; }
        virtual void destroy() {}

        [[nodiscard]] layerType& getType() noexcept { return m_type; }
        [[nodiscard]] bool isEnabled() const noexcept { return m_isEnabled; }
    };
}

#endif //LAYER_H
