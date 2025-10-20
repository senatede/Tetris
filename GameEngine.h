#pragma once
#include "Board/Board.h"
#include "InputHandler.h"
#include "BlockFactory/BlockFactory.h"
#include "Blocks/Block.h"

class GameEngine {
    Board& board;
    // ScoreManager scoreManager;
    // StorageManager storageManager;
    InputHandler& inputHandler;
    BlockFactory& blockFactory;
    std::unique_ptr<Block> holdBlock;
    std::unique_ptr<Block> currentBlock;
    // Timer tickTimer;
    // State gameState;

    GameEngine();
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    void spawnNextBlock();
public:
    static GameEngine& getInstance();

    void tick();
    void requestMove(int dx) const;
    void requestRotate(bool clockwise) const;
    void requestHardDrop() const;
    void requestSoftDrop() const;
    void requestHold();
    void requestPause();
};