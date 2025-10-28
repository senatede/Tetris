#pragma once
#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

class GameEngine;

class Timer {
    std::function<void()> callback;
    std::thread workerThread;
    std::mutex intervalMutex;
    std::condition_variable cv;

    std::atomic<bool> isRunning = false;
    std::atomic<int> intervalMs = 1000;

    GameEngine* engine = nullptr;

    Timer() = default;
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    void timingLoop();
public:
    static Timer& getInstance();
    void setGameEngine(GameEngine* gameEngine);

    void setCallback(std::function<void()> func);
    void start(int initialIntervalMs = 1000);
    void stop();
    void setInterval(int newIntervalMs);
};
