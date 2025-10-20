#pragma once
#include <vector>
#include "GameEngine.h"
#include "Cell.h"
#include "Blocks/Block.h"

class Board final {
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

    GameEngine* engine = nullptr;

    Board(int w, int h);
    ~Board() = default;

    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    bool Board::isLineFull(int y) const;
public:
    static Board& getInstance(int w = 10, int h = 20);
    void setGameEngine(GameEngine* gameEngine);

    Position getSpawnPosition() const;
    bool isValidPosition(const Block& block, const Position& newPos) const;
    void placeBlock(const Block& block);
    int clearFullLines();
    int getDropDistance(const Block& block) const;
};
