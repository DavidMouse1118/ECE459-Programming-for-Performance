#pragma once

#include "Event.h"
#include "Container.h"
#include <mutex>
#include <condition_variable>

class EventQueue
{
    std::condition_variable_any canRead;
    Container<Event> queue;

public:
    EventQueue();
    ~EventQueue();

    std::recursive_mutex queueMutex;
    std::mutex stdoutMutex;

    void enqueueEvent(Event e, bool inFront = false);
    Event dequeueEvent();
    Event peek();
    bool isEmpty();

    void dump();
};
