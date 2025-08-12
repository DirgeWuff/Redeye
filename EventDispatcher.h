//
// Created by DirgeWuff on 8/8/2025.
//

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

template<typename EventType>
using eventHandler = std::function<void(const EventType&)>;

template<typename EventType>
class EventDispatcher {
    std::unordered_map<std::string, std::vector<eventHandler<EventType>>> m_eventHandlers;
public:
    void subscribe(const std::string& sensorId, eventHandler<EventType> handler);
    void unsubscribe(const std::string& sensorId);
    void dispatch(const std::string& sensorId, const EventType& e) const;
};

template<typename EventType>
void EventDispatcher<EventType>::subscribe(const std::string& sensorId, eventHandler<EventType> handler) {
    m_eventHandlers[sensorId].emplace_back(std::move(handler));
}

template<typename EventType>
void EventDispatcher<EventType>::unsubscribe(const std::string& sensorId) {
    m_eventHandlers.erase(sensorId);
}

template<typename EventType>
void EventDispatcher<EventType>::dispatch(const std::string& sensorId, const EventType& e) const {
    auto iterator = m_eventHandlers.find(sensorId);
    if (iterator != m_eventHandlers.end()) {
        for (const auto& handler : iterator->second) {
            handler(e);
        }
    }
}

#endif //EVENTDISPATCHER_H
