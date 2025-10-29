#pragma once
#include "../GameEngine/GameEngine.h"
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <termios.h>

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;
constexpr int FPS = 60;

#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define BLUE    "\033[34m"
#define ORANGE "\033[38;5;208m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"


class CLI {
    inline static std::mutex console_mutex;
    inline static termios old_term{};
    inline static std::atomic<bool> running{false};
    inline static std::mutex input_mutex;
    inline static std::queue<char> input_queue;
    inline static std::condition_variable input_cv;

    InputHandler& inputHandler;
    ScoreManager& scoreManager;
    GameEngine& gameEngine;
    std::thread input_worker;

    static void setTerminalRawBlocking();
    static void restoreTerminal();
    static void handleSigint(int);

    void inputThreadFunc();
    static void pushKey(char c);
    static bool popKey(char &out);

    static std::string getCellChar(Cell cell);

    static void clearScreen();
    static int getTerminalWidth();
    static void print_in_the_center(const std::string& s, size_t len=0);
    static std::string repeat(const std::string& s, size_t n);

    void drawBoard() const;

    void startScreen() const;
    void gameOverScreen() const;
    void quittingScreen() const;
    void pausedScreen() const;
    void leaderboardScreen() const;
    void saveScoreScreen() const;

    void playLoop() const;
public:
    CLI();
    void run();
};