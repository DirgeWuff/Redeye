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
// Class definition of LayerManager.h and its member functions

#include <cassert>
#include "ranges"
#include "LayerManager.h"

namespace RE::Core {
    LayerManager LayerManager::m_managerInstance = LayerManager();

    LayerManager& LayerManager::getInstance() {
        return m_managerInstance;
    }

    void LayerManager::popLayer(const layerKey& id) {
        const auto it = m_layers.find(id);

        if (it != m_layers.end()) {
            assert(it->second);

            it->second->destroy();
            m_layers.erase(id);
        }
        else {
            logFatal(
                std::string("Cannot pop layer, no such key found: " + layerKeyToStr(id) +
                    std::string("LayerManger::popLayer()")));
        }
    }

    void LayerManager::requestLayerPop(const layerKey& id) {
            m_popRequestQueue.push_back(id);
    }

    void LayerManager::pushLayer(const layerKey& id, std::unique_ptr<Layer> layer) {
        m_layers.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(id),
            std::forward_as_tuple(std::move(layer)));
    }

    bool LayerManager::stackContains(const layerKey& id) const {
        return m_layers.contains(id);
    }

    void LayerManager::resumeLayer(const layerKey& id) {
        const auto it = m_layers.find(id);

        if (it != m_layers.end()) {
            assert(it->second);
            it->second->resume();
        }
        else {
            logFatal(std::string("Cannot enable layer, no such key found: " + layerKeyToStr(id)));
        }
    }

    void LayerManager::suspendLayer(const layerKey& id) {
        const auto it = m_layers.find(id);

        if (it != m_layers.end()) {
            assert(it->second);
            it->second->suspend();
        }
        else {
            logFatal(std::string("Cannot suspend layer, no such key found: " + layerKeyToStr(id)));
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
}