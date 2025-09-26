//
// Created by DirgeWuff on 8/8/2025.
//

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <functional>
#include <string>
#include <vector>

// Underlying framework used to dispatch events within the game.

template<typename EventType>
class EventDispatcher {
    using eventHandler = std::function<void(const EventType&)>;
    using matcher = std::function<bool(const std::string&)>;

    // Exceedingly bad and confusing naming in this struct...
    struct subscription {
        std::string id;             // Id if the caller
        matcher matchFunction;      // Function used to match a substring. Should return true if not npos
        eventHandler handler;       // The function to be called when matcher finds a match

        subscription(
            std::string id,
            matcher matcher,
            eventHandler handler) :
        id(std::move(id)),
        matchFunction(std::move(matcher)),
        handler(std::move(handler))
        {

        }
    };



    std::vector<subscription> m_activeSubscriptions;
public:
    void subscribe(std::string&& sensorId, matcher matchFunction, eventHandler handler);    // Subscribe an id to a matcher and handler function
    void unsubscribe(const std::string& sensorId);                                          // Unsubscribe an id from a matcher and handler
    void dispatch(const std::string& sensorId, const EventType& e) const;                   // Dispatch an event
};

template<typename EventType>
void EventDispatcher<EventType>::subscribe(
    std::string&& sensorId,
    matcher matchFunction,
    eventHandler handler)
{
    // Add subscription object to current subscriptions.
    m_activeSubscriptions.emplace_back(sensorId, std::move(matchFunction), std::move(handler));
}

template<typename EventType>
void EventDispatcher<EventType>::unsubscribe(const std::string& sensorId) {
    // If a subscription matching sensorId is present in m_activeSubscriptions, find and erase it.
    m_activeSubscriptions.erase(
        std::remove_if(m_activeSubscriptions.begin(), m_activeSubscriptions.end(),
            [&](const subscription& sub) { return sub.id == sensorId; }),
            m_activeSubscriptions.end());
}

template<typename EventType>
void EventDispatcher<EventType>::dispatch(const std::string& sensorId, const EventType& e) const {
    // If the matcher finds a substring match in m_activeSubscriptions, call the handler with e as param.
   for (const auto& sub : m_activeSubscriptions) {
       if (sub.matchFunction(sensorId)) {
           sub.handler(e);
       }
   }
}

#endif //EVENTDISPATCHER_H
