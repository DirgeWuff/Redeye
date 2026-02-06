//
// Author: DirgeWuff
// Created on: 1/27/26
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Wrapper class for EventDispatcher<T>, allowing a single, monolithic object
// dispatch multiple event types within the game layer.

#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <unordered_map>
#include <typeindex>
#include <memory>
#include "EventDispatcher.h"

namespace RE::Core {
    class EventBus {
        // Just here for type-erasure
        struct route {
            virtual ~route() = default;
        };

        template<typename T>
        struct eventRoute final : route {
            EventDispatcher<T> dispatcher;
        };

        // std::type_index is a wrapper for std::type_info and can be used for type-deduction
        std::unordered_map<std::type_index, std::unique_ptr<route>> m_routes;

    public:
        EventBus() = default;
        ~EventBus() = default;

        EventBus(const EventBus&) = delete;

        EventBus(EventBus&& other) noexcept :
            m_routes(std::move(other.m_routes))
        {
            #ifdef DEBUG
                logDbg("Move called on EventBus, new address: ", this);
            #endif
        };

        EventBus& operator=(const EventBus&) = delete;\

        EventBus& operator=(EventBus&& other) noexcept {
            if (this != &other) {
                this->m_routes = std::move(other.m_routes);
            }

            #ifdef DEBUG
                logDbg("Move assignment called on EventBus, new address: ", this);
            #endif

            return *this;
        };

        template<typename T>
        void addDispatcher(EventDispatcher<T> dispatcher) {
            const auto type = std::type_index(typeid(T));

            const auto it = m_routes.find(type); // NOLINT

            if (it != m_routes.end()) {
                logDbg("Event route already exists: ", type.name(),  ". EventBus::addDispatcher(Args...)");
                return;
            }

            auto basePtr = std::make_unique<eventRoute<T>>();
            basePtr->dispatcher = std::move(dispatcher);

            m_routes.emplace(type, std::move(basePtr));
        }

        template<typename T>
        EventDispatcher<T>& get() {
            const auto type = std::type_index(typeid(T)); // Get type of T

            // Find the ptr in m_routes...
            const auto it = m_routes.find(type);

            // Return the dispatcher.
            return static_cast<eventRoute<T>*>(it->second.get())->dispatcher;
        }
    };
}

#endif //EVENTBUS_H
