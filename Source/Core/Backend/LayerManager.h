//
// Created by DirgeWuff on 9/26/25.
//

#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <unordered_map>
#include <memory>
#include "Layer.h"
#include "LayerTypes.h"
#include "../Utility/Error.h"

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

#endif //LAYERMANAGER_H