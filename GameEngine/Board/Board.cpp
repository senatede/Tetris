#include "Board.h"
#include "../GameEngine.h"

Board::Board(const int w, const int h) :
    width(w),
    height(h),
    grid(h, std::vector<Cell>(w, Cell::Empty))
{};

Board& Board::getInstance(const int w, const int h) {
    static Board instance(w, h);
    return instance;
}

void Board::setGameEngine(GameEngine* gameEngine) {
    this->engine = gameEngine;
}

void Board::reset() {
    grid = std::vector<std::vector<Cell>>(height, std::vector<Cell>(width, Cell::Empty));
}

Position Board::getSpawnPosition() const {
    const int spawnX = width / 2 - 1;
    const int spawnY = height - 2;
    return {spawnX, spawnY};
}

void Board::setGrid(const std::vector<std::vector<Cell>>& newGrid) {
    grid = newGrid;
}

std::vector<std::vector<Cell>> Board::getGrid() const {
    return grid;
}

std::vector<std::vector<Cell>> Board::getRenderGrid(const Block* currentBlock) const {
    auto renderGrid = grid;
    if (currentBlock) {
        const auto globalCells = currentBlock->getGlobalCellsAt(currentBlock->getPosition());
        const Cell type = currentBlock->getType();

        const auto ghostCells = currentBlock->getGlobalCellsAt(getGhostPosition(*currentBlock));
        const Cell ghostType = ghostMap.at(type);
        for (const auto& ghostCell : ghostCells) {
            if (ghostCell.y >= 0 && ghostCell.y < height &&
                ghostCell.x >= 0 && ghostCell.x < width) {
                renderGrid[ghostCell.y][ghostCell.x] = ghostType;
                }
        }

        for (const auto& cellPos : globalCells) {
            if (cellPos.y >= 0 && cellPos.y < height &&
                cellPos.x >= 0 && cellPos.x < width) {
                    renderGrid[cellPos.y][cellPos.x] = type;
                }
        }
    }
    return renderGrid;
}

void Board::placeBlock(const Block& block) {
    const std::vector<Position> globalCells = block.getGlobalCellsAt(block.getPosition());

    const Cell typeToPlace = block.getType();

    for (const auto& cellPos : globalCells)
        grid[cellPos.y][cellPos.x] = typeToPlace;
}

bool Board::isValidPosition(const Block& block, const Position& newPos) const {
    const std::vector<Position> globalCells = block.getGlobalCellsAt(newPos);

    for (const auto& cellPos : globalCells) {
        if (cellPos.x < 0 || cellPos.x >= width || cellPos.y >= height || cellPos.y < 0) {
            return false;
        }

        if (grid[cellPos.y][cellPos.x] != Cell::Empty) {
            return false;
        }
    }

    return true;
}

int Board::clearFullLines() {
    int linesClearedCount = 0;
    for (int y = 0; y < height; ++y) {

        if (isLineFull(y)) {
            linesClearedCount++;

            for (int k = y; k < height - 1; ++k) {
                grid[k] = grid[k + 1];
            }
            grid[height - 1].assign(width, Cell::Empty);

            y--;
        }
    }
    return linesClearedCount;
}

bool Board::isLineFull(const int y) const {
    for (int x = 0; x < width; ++x) {
        if (grid[y][x] == Cell::Empty) {
            return false;
        }
    }
    return true;
}

int Board::getDropDistance(const Block& block) const {
    int distance = 0;

    auto pos = block.getPosition();
    pos.y--;

    while (isValidPosition(block, pos)) {
        distance++;
        pos.y--;
    }

    return distance;
}

Position Board::getGhostPosition(const Block& block) const {
    const int dropDistance = getDropDistance(block);
    Position pos = block.getPosition();
    pos.y -= dropDistance;
    return pos;
}