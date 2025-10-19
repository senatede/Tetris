#pragma once

#include <map>

#include "GameEngine.h"
#include "Commands/Command.h"

enum class KeyType { LEFT, RIGHT, ROTATE_CW, ROTATE_CCW, HARD_DROP, SOFT_DROP, PAUSE };

class InputHandler {
    GameEngine* engine = nullptr;
    std::map<KeyType, std::unique_ptr<Command>> bindings;

    InputHandler() = default;
    ~InputHandler() = default;
public:
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

    static InputHandler& getInstance();
    void setGameEngine(GameEngine* gameEngine);

    void bind(KeyType key, std::unique_ptr<Command> command);
    void handleKey(KeyType key);
};
