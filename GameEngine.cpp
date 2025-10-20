#include "GameEngine.h"

GameEngine::GameEngine() :
    board(Board::getInstance()),
    // scoreManager(),
    // storageManager(),
    inputHandler(InputHandler::getInstance()),
    blockFactory(BlockFactory::getInstance()),
    holdBlock(nullptr),
    currentBlock(nullptr)
{
    board.setGameEngine(*this);
    inputHandler.setGameEngine(*this);
}

GameEngine& GameEngine::getInstance() {
    static GameEngine instance;
    return instance;
}

void GameEngine::spawnNextBlock() {
    const Position spawnPosition = board.getSpawnPosition();
    currentBlock = blockFactory.createNextBlock(spawnPosition);
}

void GameEngine::tick() {

    if (!currentBlock) return;

    Position pos = currentBlock->getPosition();
    pos.y--;

    if (board.isValidPosition(*currentBlock, pos)) {
        currentBlock->move(0, -1);
    } else {
        board.placeBlock(*currentBlock);
        int linesCleared = board.clearFullLines();

        // TODO: scoring full line
        spawnNextBlock();

        // TODO: game over check
    }
}

void GameEngine::requestMove(const int dx) const {
    if (!currentBlock) return;

    const Position currentPos = currentBlock->getPosition();
    const Position targetPos = {currentPos.x + dx, currentPos.y};

    if (board.isValidPosition(*currentBlock, targetPos)) {
        currentBlock->move(dx, 0);
        // render update
    }
}

void GameEngine::requestRotate(const bool clockwise) const {
    if (!currentBlock) return;

    if (clockwise) {
        currentBlock->rotateCW();
    } else {
        currentBlock->rotateCCW();
    }

    Position position = currentBlock->getPosition();

    if (board.isValidPosition(*currentBlock, position)) {
        // render updated block
    } else {

        // maybe Wall Kick Logic

        if (clockwise) {
            currentBlock->rotateCCW();
        } else {
            currentBlock->rotateCW();
        }
    }
}


void GameEngine::requestHardDrop() const {
    if (!currentBlock) return;

    const int dropDistance = board.getDropDistance(*currentBlock);

    if (dropDistance > 0) {
        currentBlock->move(0, -dropDistance);

        // TODO: points for the hard drop distance

        board.placeBlock(*currentBlock);

        // TODO: lines clear

        // spawnNextBlock();
    }
}

void GameEngine::requestSoftDrop() const {
    if (!currentBlock) return;

    auto currentPos = currentBlock->getPosition();
    currentPos.y--;
    if (board.isValidPosition(*currentBlock, currentPos)) {
        currentBlock->move(0, -1);
    }
}

