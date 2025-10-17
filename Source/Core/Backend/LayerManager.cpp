//
// Created by DirgeWuff on 9/26/25.
//

#include "LayerManager.h"
#include "ranges"

LayerManager LayerManager::m_managerInstance = LayerManager();

LayerManager& LayerManager::getInstance() {
    return m_managerInstance;
}

void LayerManager::popLayer(const std::string& id) {
    const auto it = m_layers.find(id);

    if (it != m_layers.end()) {
        assert(it->second);

        it->second->destroy();
        m_layers.erase(id);
    }
    else {
        logErr(
            std::string("Cannot pop layer, no such key found: " + id +
                std::string("LayerManger::popLayer()")));
    }
}

void LayerManager::pollEvents() const {
    for (const auto& layer : std::views::values(m_layers)) {
        assert(layer);
        if (!layer->isEnabled()) continue;

        layer->pollEvents();
    }
}

void LayerManager::update() const {
    for (const auto& layer : std::views::values(m_layers)) {
        assert(layer);
        if (!layer->isEnabled()) continue;

        layer->update();
    }
}

void LayerManager::draw() const {
    for (const auto& layer : std::views::values(m_layers)) {
        // Sloppy, deterministic way of drawing layers in the right order.
        // Should come up with a better way of doing this at some point...
        assert(layer);

        if (!layer->isEnabled()) continue;
        if (layer->getType() != layerType::PRIMARY_LAYER) continue;

        layer->draw();
    }

    for (const auto& layer : std::views::values(m_layers)) {
        if (!layer->isEnabled()) continue;
        if (layer->getType() != layerType::OVERLAY_LAYER) continue;

        layer->draw();
    }
}

void LayerManager::updateLayerStack() noexcept {
    if (m_popRequestQueue.empty()) return;

    for (const auto& request : m_popRequestQueue) {
        popLayer(request);
    }

    m_popRequestQueue.clear();
}

void LayerManager::suspendOverlays() const noexcept {
    for (const auto& layer : std::views::values(m_layers)) {
        assert(layer);

        if (layer->getType() == layerType::OVERLAY_LAYER) {
            layer->suspend();
        }
    }
}

void LayerManager::resumeOverlays() const noexcept {
    for (const auto& layer : std::views::values(m_layers)) {
        assert(layer);

        if (layer->getType() == layerType::OVERLAY_LAYER) {
            layer->resume();
        }
    }
}