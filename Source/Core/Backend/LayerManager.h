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
// Class declaration for the layer manager, responsible for the ownership and
// lifetime management of all layer objects in the game. Defines an interface
// for creating, updating, switching and removing layers from the game.
// This class is a singleton.

#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <unordered_map>
#include <memory>
#include "Layer.h"
#include "../Utility/Enum.h"
#include "../Utility/Logging.h"

namespace RE::Core {
    class LayerManager final {
        static LayerManager m_managerInstance;
        std::unordered_map<layerKey, std::unique_ptr<Layer>> m_layers{};
        std::vector<layerKey> m_popRequestQueue{};

        LayerManager() = default;
        ~LayerManager() = default;

        void popLayer(const layerKey& id);
    public:
        static LayerManager& getInstance();

        void requestLayerPop(const layerKey& id);
        void pushLayer(const layerKey& id, std::unique_ptr<Layer> layer);
        void resumeLayer(const layerKey& id);
        void suspendLayer(const layerKey& id);
        void pollEvents() const;
        void update() const;
        void draw() const;
        void updateLayerStack() noexcept;
        void suspendOverlays() const noexcept;
        void resumeOverlays() const noexcept;
        [[nodiscard]] bool stackContains(const layerKey& id) const;
    };
}

#endif //LAYERMANAGER_H