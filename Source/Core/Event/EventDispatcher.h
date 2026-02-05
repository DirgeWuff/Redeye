//
// Author: DirgeWuff
// Created on: 8/8/25
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Module purpose/description:
//
// Generic/type-safe event dispatch system. Ingests an event struct and
// dispatches event to correct handler std::function that can then execute
// logic based on the event type and info.

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <functional>
#include <vector>
#include "Event.h"

namespace RE::Core {
    // Underlying framework used to dispatch events within the game.
    template<typename T>
    class EventDispatcher {
        using eventHandler = std::function<void(const T&)>;
        using matcher = std::function<bool(const T&)>;

        struct subscription {
            subId id;
            matcher matchFunction;
            eventHandler handler;

            subscription(
                const subId s,
                matcher m,
                eventHandler h) :
                    id(s),
                    matchFunction(std::move(m)),
                    handler(std::move(h))
            {

            }
        };

        std::vector<subscription> m_activeSubscriptions;
    public:
        void subscribe(subId id, matcher matchFunction, eventHandler handler);
        void unsubscribe(const subId& id);
        void dispatch(const T& e) const;
    };

    template<typename T>
    void EventDispatcher<T>::subscribe(
        const subId id,
        matcher matchFunction,
        eventHandler handler)
    {
        m_activeSubscriptions.emplace_back(id, std::move(matchFunction), std::move(handler));
    }

    template<typename T>
    void EventDispatcher<T>::unsubscribe(const subId& id) {
        m_activeSubscriptions.erase(
            std::remove_if(m_activeSubscriptions.begin(), m_activeSubscriptions.end(),
                [&](const subscription& sub) { sub.type == id; }),
                m_activeSubscriptions.end());
    }

    template<typename T>
    void EventDispatcher<T>::dispatch(const T& e) const {
        for (const auto& sub : m_activeSubscriptions) {
            if (sub.matchFunction(e)) {
                sub.handler(e);
            }
        }
    }
}

#endif //EVENTDISPATCHER_H
