#include "eventqueue.h"


EventQueue::EventQueue(Context *parent)
    : parent(parent)
{

}

void EventQueue::start() {
    if (workThread.get_id() == std::thread::id())
        workThread = std::thread(&EventQueue::threadLoop, this);
    else
        std::cerr << "A worker thread is already running!" << std::endl << std::flush;
}

void EventQueue::schedule(std::chrono::time_point<std::chrono::system_clock> time, std::function<void ()> event) {
    std::unique_lock<std::recursive_mutex> lock(queueLock);
    queue.emplace(time, event);
    lock.unlock();
    cv.notify_all();
}

void EventQueue::schedule(std::chrono::seconds after, std::function<void ()> event) {
    schedule(std::chrono::system_clock::now() + after, event);
}

void EventQueue::schedule(std::function<void ()> event) {
    schedule(std::chrono::time_point<std::chrono::system_clock> {}, event);
}

void EventQueue::threadLoop() {
    while (true) {
        std::unique_lock<std::recursive_mutex> lock(queueLock);
        auto time = std::chrono::system_clock::now();
        while (!queue.empty() && queue.begin()->first <= time) {
            std::cerr << "The time is now" << std::to_string(time.time_since_epoch().count()) << std::endl << std::flush;
            std::cerr << "The event time is" << std::to_string(queue.begin()->first.time_since_epoch().count()) << std::endl << std::flush;
            queue.begin()->second();
            queue.erase(queue.begin());
        }
        if (queue.begin() != queue.end()) {
            std::cerr << "Waiting for an event" << std::endl << std::flush;
            auto nextTime = queue.begin()->first;
            lock.unlock();
            std::unique_lock<std::mutex> cvLock(cvMutex);
            cv.wait_until(cvLock, nextTime);
        }
        else {
            std::cerr << "Waiting before somebody wakes me up" << std::endl << std::flush;
            lock.unlock();
            std::unique_lock<std::mutex> cvLock(cvMutex);
            cv.wait(cvLock);
        }
    }
}
