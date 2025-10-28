#include <cmath>

#include "GameEngine.h"
#include "Board/Board.h"
#include "ScoreManager.h"
#include "InputHandler.h"
#include "BlockFactory/BlockFactory.h"
#include "Blocks/Block.h"
#include "Timer.h"

GameEngine::GameEngine(int boardWidth, int boardHeight, InputHandler& input_handler, ScoreManager& score_manager) :
    board(Board::getInstance(boardWidth, boardHeight)),
    scoreManager(score_manager),
    // storageManager(),
    inputHandler(input_handler),
    blockFactory(BlockFactory::getInstance()),
    holdBlock(nullptr),
    currentBlock(nullptr),
    tickTimer(Timer::getInstance()),
    gameState(GameState::IDLE)
{
    board.setGameEngine(this);
    scoreManager.setGameEngine(this);
    inputHandler.setGameEngine(this);
    tickTimer.setGameEngine(this);
}

GameEngine& GameEngine::getInstance(const int boardWidth, const int boardHeight, InputHandler& input_handler, ScoreManager& score_manager) {
    static GameEngine instance(boardWidth, boardHeight, input_handler, score_manager);
    return instance;
}

void GameEngine::reset() {
    std::lock_guard lock(gameMutex);
    scoreManager.reset();
    board.reset();
    gameState = GameState::IDLE;
    holdBlock = nullptr;
    currentBlock = nullptr;
    hasHeldThisTurn = false;
    tickTimer.stop();
}


void GameEngine::startGame(const int level) {
    std::lock_guard lock(gameMutex);
    reset();
    scoreManager.setLevel(level);
    gameState = GameState::RUNNING;

    currentBlock = nullptr;
    holdBlock = nullptr;
    hasHeldThisTurn = false;

    spawnNextBlock();

    const int startLevel = scoreManager.getLevel();
    const int startInterval = calculateGravityInterval(startLevel);
    tickTimer.start(startInterval);
}

GameState GameEngine::getGameState() const {
    return gameState.load();
}

void GameEngine::spawnNextBlock() {
    if (gameState != GameState::RUNNING) return;
    hasHeldThisTurn = false;

    const Position spawnPosition = board.getSpawnPosition();
    currentBlock = blockFactory.createNextBlock(spawnPosition);
    if (!board.isValidPosition(*currentBlock, spawnPosition)) {
        gameState = GameState::GAME_OVER;
        std::thread([this]{ tickTimer.stop(); }).detach();
    }
    isSoftLocked = false;
    shouldRender = true;
}

bool GameEngine::needsRender() const {
    return shouldRender;
}
void GameEngine::markRender() {
    shouldRender = false;
}

std::vector<std::vector<Cell>> GameEngine::getRenderGrid() const {
    std::lock_guard lock(gameMutex);
    return board.getRenderGrid(currentBlock.get());
}

void GameEngine::tick() {
    std::lock_guard lock(gameMutex);
    if (gameState.load() != GameState::RUNNING) return;
    if (!currentBlock) return;

    Position pos = currentBlock->getPosition();
    pos.y--;

    if (board.isValidPosition(*currentBlock, pos)) {
        currentBlock->move(0, -1);
    } else if (!isSoftLocked) {
        isSoftLocked = true;
        lockTimeStart = std::chrono::steady_clock::now();
        lockResetCount = 0;
    } else {
         auto elapsed = std::chrono::steady_clock::now() - lockTimeStart;
        if (elapsed >= LOCK_DELAY) {

            board.placeBlock(*currentBlock);
            scoreManager.addLineClear(board.clearFullLines());

            spawnNextBlock();

        }
    }
    shouldRender = true;
}

void GameEngine::requestMove(const int dx) {
    std::lock_guard lock(gameMutex);
    if (gameState != GameState::RUNNING) return;
    if (!currentBlock) return;

    const Position currentPos = currentBlock->getPosition();
    const Position targetPos = {currentPos.x + dx, currentPos.y};

    if (board.isValidPosition(*currentBlock, targetPos)) {
        currentBlock->move(dx, 0);

        if (isSoftLocked && lockResetCount < MAX_LOCK_RESETS) {
            lockTimeStart = std::chrono::steady_clock::now();
            lockResetCount++;
        }
    }
    shouldRender = true;
}

void GameEngine::requestRotate(const bool clockwise) {
    std::lock_guard lock(gameMutex);
    if (gameState != GameState::RUNNING) return;
    if (!currentBlock) return;

    auto oldRotation = currentBlock->getRotation();
    if (clockwise) {
        currentBlock->rotateCW();
    } else {
        currentBlock->rotateCCW();
    }
    auto newRotation = currentBlock->getRotation();

    Position position = currentBlock->getPosition();

    if (board.isValidPosition(*currentBlock, position)) {
        shouldRender = true;

        if (isSoftLocked && lockResetCount < MAX_LOCK_RESETS) {
            lockTimeStart = std::chrono::steady_clock::now();
            lockResetCount++;
        }
        return;
    }

    for (auto offset : currentBlock->getSuperRotationOffSets(oldRotation, newRotation)) {
        Position newPosition = {position.x + offset.x, position.y + offset.y};
        if (board.isValidPosition(*currentBlock, newPosition)) {
            currentBlock->move(offset.x, offset.y);
            shouldRender = true;

            if (isSoftLocked && lockResetCount < MAX_LOCK_RESETS) {
                lockTimeStart = std::chrono::steady_clock::now();
                lockResetCount++;
            }
            return;
        }
    }

    if (clockwise)
        currentBlock->rotateCCW();
    else
        currentBlock->rotateCW();
}


void GameEngine::requestHardDrop() {
    std::lock_guard lock(gameMutex);
    if (gameState != GameState::RUNNING) return;
    if (!currentBlock) return;

    const int dropDistance = board.getDropDistance(*currentBlock);

    if (dropDistance > 0) {
        currentBlock->move(0, -dropDistance);

        scoreManager.addHardDropPoints(dropDistance);

        board.placeBlock(*currentBlock);

        scoreManager.addLineClear(board.clearFullLines());
        spawnNextBlock();
        shouldRender = true;
    }
}

void GameEngine::requestSoftDrop() {
    std::lock_guard lock(gameMutex);
    if (gameState != GameState::RUNNING) return;
    if (!currentBlock) return;

    auto currentPos = currentBlock->getPosition();
    currentPos.y--;
    if (board.isValidPosition(*currentBlock, currentPos)) {
        currentBlock->move(0, -1);

        scoreManager.addSoftDropPoints();
        shouldRender = true;
    }
}

void GameEngine::pause() {
    std::lock_guard lock(gameMutex);
    if (gameState == GameState::RUNNING) {
        tickTimer.stop();
        gameState = GameState::PAUSED;
        shouldRender = true;
    }
}

void GameEngine::resume() {
    std::lock_guard lock(gameMutex);
    if (gameState == GameState::PAUSED) {
        gameState = GameState::RUNNING;

        const int level = scoreManager.getLevel();
        const int currentInterval = calculateGravityInterval(level);
        tickTimer.start(currentInterval);
        shouldRender = true;
    }
}

void GameEngine::requestHold() {
    std::lock_guard lock(gameMutex);
    if (gameState != GameState::RUNNING) return;
    if (!currentBlock || hasHeldThisTurn) return;
    hasHeldThisTurn = true;

    if (!holdBlock) {
        holdBlock = std::move(currentBlock);
        holdBlock->resetRotation();
        spawnNextBlock();
    } else {
        std::unique_ptr<Block> tempBlock = std::move(currentBlock);
        currentBlock = std::move(holdBlock);

        const Position spawnPos = board.getSpawnPosition();
        currentBlock->setPosition(spawnPos);

        holdBlock = std::move(tempBlock);
        holdBlock->resetRotation();
    }
    shouldRender = true;
}

int GameEngine::calculateGravityInterval(const int level) {
    auto interval = static_cast<int>(1000 * pow((0.8 - (level - 1.0) / 250.0), level - 1));
    if (interval <= 0) return 1;
    return interval;
}

void GameEngine::updateLevelSpeed() const {
    std::lock_guard lock(gameMutex);
    if (gameState != GameState::RUNNING) return;

    const int level = scoreManager.getLevel();
    const int newInterval = calculateGravityInterval(level);

    tickTimer.setInterval(newInterval);
}

std::vector<std::vector<Cell>> GameEngine::getRenderHold() {
    auto cells = std::vector(2, std::vector(4, Cell::Empty));
    if (holdBlock) {
        const auto type = holdBlock->getType();
        for (auto pos : holdBlock->getGlobalCellsAt({1, 0}))
            cells[pos.y][pos.x] = type;
    }
    shouldRender = true;
    return cells;
}

std::vector<std::vector<std::vector<Cell>>> GameEngine::getRenderNext() {
    auto blocksTypes = blockFactory.peekNext(3);
    std::vector<std::vector<std::vector<Cell>>> blocks;
    for (auto blockType : blocksTypes) {
        auto cells = std::vector(2, std::vector(4, Cell::Empty));
        auto block = BlockFactory::createBlock(blockType, {0, 0});
        for (auto pos : block->getGlobalCellsAt({1, 0}))
            cells[pos.y][pos.x] = blockType;
        blocks.emplace_back(cells);
    }
    shouldRender = true;
    return blocks;
}