#include "EventQueue.h"
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

EventQueue::EventQueue() {
    // nop
}

EventQueue::~EventQueue() {
    if (queue.size() != 0) {
        dump();
        assert(false);
    }
}

void EventQueue::enqueueEvent(Event e, bool inFront) {
    std::unique_lock<std::recursive_mutex> queueLock(queueMutex);

    // Unbounded buffer
    if (inFront) {
        queue.pushFront(e);
    } else {
        queue.pushBack(e);
    }

    queueLock.unlock();
    canRead.notify_all();
}

Event EventQueue::dequeueEvent() {
    std::unique_lock<std::recursive_mutex> queueLock(queueMutex);

    canRead.wait(queueLock, [&]() -> bool {
        return queue.size() > 0;
    });

    Event e = queue.popFront();

    queueLock.unlock();
    canRead.notify_all();

    return e;
}

Event EventQueue::peek() {
    std::unique_lock<std::recursive_mutex> queueLock(queueMutex);
    return queue.front();
}

bool EventQueue::isEmpty() {
    std::unique_lock<std::recursive_mutex> queueLock(queueMutex);
    return queue.size() == 0;
}

void EventQueue::dump() {
    std::unique_lock<std::recursive_mutex> queueLock(queueMutex);

    cout << "EventQueue [Start]" << endl;

    for (int i = 0; i < queue.size(); i++) {
        queue[i].dump();
    }

    cout << "EventQueue [End]" << endl;
}
