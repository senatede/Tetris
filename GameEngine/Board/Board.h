#pragma once
#include <vector>
#include "Cell.h"
#include "../Blocks/Block.h"

class GameEngine;

class Board final {
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

    GameEngine* engine = nullptr;

    Board(int w, int h);
    ~Board() = default;

    bool isLineFull(int y) const;
public:
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
    static Board& getInstance(int w = 10, int h = 20);
    void setGameEngine(GameEngine* gameEngine);

    void setGrid(const std::vector<std::vector<Cell>>& newGrid);
    std::vector<std::vector<Cell>> getGrid() const;

    void reset();
    Position getSpawnPosition() const;
    std::vector<std::vector<Cell>> getRenderGrid(const Block* currentBlock) const;
    bool isValidPosition(const Block& block, const Position& newPos) const;
    void placeBlock(const Block& block);
    int clearFullLines();
    int getDropDistance(const Block& block) const;
    Position getGhostPosition(const Block& block) const;
};
