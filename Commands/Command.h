#pragma once
#include "GameEngine.h"

class Command {
protected:
    GameEngine* engine = nullptr;

    Command() = default;
    virtual ~Command() = default;

    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

public:
    void setGameEngine(GameEngine* gameEngine);

    virtual void execute() = 0;
};