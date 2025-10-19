#pragma once

#include "InputHandler.h"

class GameEngine {
private:
    Board board;
    ScoreManager scoreManager;
    StorageManager storageManager;
    InputHandler& inputHandler;
    BlockFactory blockFactory;
    Queue<Block> nextQueue;
    Block holdBlock;
    Block currentBlock;
    Timer tickTimer;
    State gameState;
public:
    GameEngine();
    ~GameEngine() = default;
};