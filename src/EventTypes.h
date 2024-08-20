#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include "Event.h"
#include <string>

class EventTypeA: public IEvent {
    std::string dataA;
public:
    EventTypeA(const std::string& data) : dataA(data) {}
    json ToJson() const {
        return json{{"type", "EventTypeA"}, {"data", dataA}};
    }
};

class EventTypeB : public IEvent {
    int dataB;
public:
    EventTypeB(int data) : dataB(data) {}
    json ToJson() const {
        return json{{"type", "EventTypeB"}, {"data", dataB}};
    }
};

#endif // EVENT_TYPES_H
