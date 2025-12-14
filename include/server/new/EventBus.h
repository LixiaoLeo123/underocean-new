//
// Created by 15201 on 12/6/2025.
//
#ifndef UNDEROCEAN_EVENTBUS_H
#define UNDEROCEAN_EVENTBUS_H

#include <vector>
#include <functional>
#include <memory>
#include <algorithm>
using EventTypeID = size_t;  //only use inside this system
inline EventTypeID getUniqueEventID() {
    static EventTypeID lastID = 0;
    return lastID++;
}
template<typename T>
EventTypeID getEventID() {
    static EventTypeID typeID = getUniqueEventID();
    return typeID;
}
class IEventDispatcher {
public:
    virtual ~IEventDispatcher() = default;
};
template<typename EventType>
class EventDispatcher : public IEventDispatcher {
public:
    struct Callback {
        std::function<void(const EventType&)> func_;
        int priority_;
    };
    void subscribe(Callback cb) {
        listeners.push_back(std::move(cb));
        sort();
    }
    void publish(const EventType& event) {
        for (auto& callback : listeners) {
            callback.func_(event);
        }
    }
    void clear() {
        listeners.clear();
    }
private:
    std::vector<Callback> listeners;
    void sort() {
        std::sort(listeners.begin(), listeners.end(),
        [](const Callback& a, const Callback& b) {
            return a.priority_ > b.priority_;
        });
    }
};
class EventBus {
public:
    EventBus() = default;
    ~EventBus() = default;
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> callback, int priority = 0) {
        EventTypeID id = getEventID<EventType>();
        if (id >= dispatchers.size()) {
            dispatchers.resize(id + 1);
        }
        if (!dispatchers[id]) {
            dispatchers[id] = std::make_unique<EventDispatcher<EventType>>();
        }
        auto* specificDispatcher = static_cast<EventDispatcher<EventType>*>(dispatchers[id].get());
        specificDispatcher->subscribe({std::move(callback), priority});
    }
    template<typename EventType>
    void publish(const EventType& event) {
        EventTypeID id = getEventID<EventType>();
        if (id < dispatchers.size() && dispatchers[id]) {
            auto* specificDispatcher = static_cast<EventDispatcher<EventType>*>(dispatchers[id].get());
            specificDispatcher->publish(event);
        }
    }
    void reset() {
        dispatchers.clear();
    }
private:
    std::vector<std::unique_ptr<IEventDispatcher>> dispatchers;
};
#endif //UNDEROCEAN_EVENTBUS_H