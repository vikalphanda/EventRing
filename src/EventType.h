#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include <string>

class EventTypeA {
    std::string dataA;
public:
    EventTypeA(const std::string& data) : dataA(data) {}
    json toJson() const {
        return json{{"type", "EventTypeA"}, {"data", dataA}};
    }
};

class EventTypeB {
    int dataB;
public:
    EventTypeB(int data) : dataB(data) {}
    json toJson() const {
        return json{{"type", "EventTypeB"}, {"data", dataB}};
    }
};

#endif // EVENT_TYPES_H
