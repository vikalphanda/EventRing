#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <memory>
#include "Event.h"

class EventHandler
{
public:
    EventHandler(size_t queueSize = 1024);
    ~EventHandler();

    void addEvent(const Event& event);
private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

#endif