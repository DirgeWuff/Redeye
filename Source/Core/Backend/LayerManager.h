//
// Created by DirgeWuff on 9/26/25.
//

#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>
#include "Layer.h"
#include "../Utility/Error.h"

class LayerManager final {
    static LayerManager m_managerInstance;
    std::unordered_map<std::string, std::unique_ptr<Layer>> m_layers{};
    std::vector<std::string> m_popRequestQueue;

    LayerManager() = default;
    ~LayerManager() = default;

    void popLayer(const std::string& id);
public:
    static LayerManager& getInstance();

    template<typename T>
    void requestLayerPop(const T& id) {
        m_popRequestQueue.push_back(id);
    }

    template<typename I, typename L>
    void pushLayer(I&& id, L&& layer) {
        m_layers.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::forward<I>(id)),
            std::forward_as_tuple(std::move(layer)));
    }

    template<typename T>
    bool stackContains(const T& id) {
        auto it = m_layers.find(id);

        if (it == m_layers.end()) {
            return false;
        }

        return true;
    }

    template<typename T>
    void resumeLayer(const T& id) {
        const auto it = m_layers.find(id);

        if (it != m_layers.end()) {
            assert(it->second);
            it->second->resume();
        }
        else {
            logErr(std::string("Cannot enable layer, no such key found: ") + std::string(id));
        }
    }

    template<typename T>
    void suspendLayer(const T& id) {
        const auto it = m_layers.find(id);

        if (it != m_layers.end()) {
            assert(it->second);
            it->second->suspend();
        }
        else {
            logErr(std::string("Cannot disable layer, no such key found: ") + std::string(id));
        }
    }

    void pollEvents() const;
    void update() const;
    void draw() const;
    void updateLayerStack() noexcept;
    void suspendOverlays() const noexcept;
    void resumeOverlays() const noexcept;
};

#endif //LAYERMANAGER_H