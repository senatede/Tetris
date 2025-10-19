#include "Board.h"

Board::Board(const int w = 10, const int h = 20) :
    width(w),
    height(h),
    grid(h, std::vector<Cell>(w, Cell::Empty))
{};

Board& Board::getInstance(const int w = 10, const int h = 20) {
    static Board instance(w, h);
    return instance;
}

void Board::setGameEngine(GameEngine* gameEngine) {
    this->engine = gameEngine;
}