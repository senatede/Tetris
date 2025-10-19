#pragma once
#include <vector>
#include "GameEngine.h"
#include "Cell.h"

class Board final {
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

    GameEngine* engine = nullptr;

    Board(int w, int h);
    virtual ~Board() = default;

    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

public:
    static Board& getInstance(int w, int h);

    void setGameEngine(GameEngine* gameEngine);
};
