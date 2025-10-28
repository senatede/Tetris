#include "Timer.h"
#include "GameEngine.h"
#include <iostream>
#include <utility>

Timer& Timer::getInstance() {
    static Timer instance;
    return instance;
}

void Timer::setGameEngine(GameEngine* gameEngine) {
    this->engine = gameEngine;
}

void Timer::setCallback(std::function<void()> func) {
    callback = std::move(func);
}

void Timer::timingLoop() {
    while (isRunning) {
        std::unique_lock<std::mutex> lock(intervalMutex);
        auto status = cv.wait_for(lock, std::chrono::milliseconds(intervalMs));
        if (!isRunning) {
            break;
        }

        if (status == std::cv_status::timeout) {
            lock.unlock();
            if (engine) {
                engine->tick();
            }
        }
    }
}

void Timer::start(const int initialIntervalMs) {
    if (isRunning.load()) {
        setInterval(initialIntervalMs);
        return;
    }

    if (workerThread.joinable() && workerThread.get_id() != std::this_thread::get_id()) {
        workerThread.join();
    }
    intervalMs = initialIntervalMs;
    isRunning.store(true);
    workerThread = std::thread(&Timer::timingLoop, this);
}

void Timer::stop() {
    if (!isRunning) return;
    isRunning = false;
    cv.notify_one();
    if (workerThread.joinable() && workerThread.get_id() != std::this_thread::get_id()) {
        workerThread.join();
    }
}

void Timer::setInterval(const int newIntervalMs) {
    std::lock_guard<std::mutex> lock(intervalMutex);
    intervalMs = newIntervalMs;
    cv.notify_one();
}