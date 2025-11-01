#pragma once
#include <map>

class GameEngine;
class Command;

enum class KeyType { LEFT, RIGHT, ROTATE_CW, ROTATE_CCW, HARD_DROP, SOFT_DROP, HOLD, PAUSE, RESUME, SAVE, LOAD};

class InputHandler {
    GameEngine* engine = nullptr;
    std::map<KeyType, Command*> bindings;

    InputHandler();
    ~InputHandler() = default;
public:
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

    static InputHandler& getInstance();
    void setGameEngine(GameEngine* gameEngine);

    void bind(KeyType key, Command* command);
    void handleKey(KeyType key);
};
