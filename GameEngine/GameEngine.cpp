#include <cmath>

#include "GameEngine.h"
#include "Board/Board.h"
#include "ScoreManagement/ScoreManager.h"
#include "InputHandler.h"
#include "BlockFactory/BlockFactory.h"
#include "Blocks/Block.h"
#include "Timer.h"
#include "SnapshotManagement/Snapshot.h"
#include "SnapshotManagement/StorageManager.h"

GameEngine::GameEngine(const int boardWidth, const int boardHeight, InputHandler& input_handler, ScoreManager& score_manager) :
    boardWidth(boardWidth),
    boardHeight(boardHeight),
    board(Board::getInstance(boardWidth, boardHeight)),
    scoreManager(score_manager),
    storageManager(StorageManager::getInstance()),
    inputHandler(input_handler),
    blockFactory(BlockFactory::getInstance()),
    holdBlock(nullptr),
    currentBlock(nullptr),
    tickTimer(Timer::getInstance()),
    gameState(GameState::IDLE)
{
    board.setGameEngine(this);
    scoreManager.setGameEngine(this);
    storageManager.setGameEngine(this);
    inputHandler.setGameEngine(this);
    tickTimer.setGameEngine(this);
}

GameEngine& GameEngine::getInstance(const int boardWidth, const int boardHeight, InputHandler& input_handler, ScoreManager& score_manager) {
    static GameEngine instance(boardWidth, boardHeight, input_handler, score_manager);
    return instance;
}

void GameEngine::setObserver(std::shared_ptr<IObserver> obs) {
    std::lock_guard lock(gameMutex);
    this->observer = std::move(obs);
}

void GameEngine::notifyObserver() {
    std::lock_guard lock(gameMutex);
    if (observer) {
        observer->onStateChanged();
    }
}

GameEngine::~GameEngine() {
    std::lock_guard lock(gameMutex);
    tickTimer.stop();
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


void GameEngine::startNewGame(const int level) {
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

void GameEngine::startGame() {
    std::lock_guard lock(gameMutex);
    gameState = GameState::RUNNING;

    const int startLevel = scoreManager.getLevel();
    const int startInterval = calculateGravityInterval(startLevel);
    tickTimer.start(startInterval);
}

GameState GameEngine::getGameState() const {
    return gameState.load();
}

std::pair<int, int> GameEngine::getBoardSize() const {
    return std::make_pair(boardWidth, boardHeight);
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
    notifyObserver();
}

const RenderData& GameEngine::getRenderData() {
    std::lock_guard lock(gameMutex);

    cachedRenderData.grid = board.getRenderGrid(currentBlock.get());
    cachedRenderData.holdType = holdBlock ? holdBlock->getType() : Cell::Empty;
    cachedRenderData.nextTypes = blockFactory.peekNext(peekNextN);
    cachedRenderData.score = scoreManager.getScore();
    cachedRenderData.level = scoreManager.getLevel();
    cachedRenderData.totalLinesCleared = scoreManager.getTotalLinesCleared();
    cachedRenderData.gameState = gameState.load();

    return cachedRenderData;
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
    notifyObserver();
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
    notifyObserver();
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
        notifyObserver();

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
            notifyObserver();

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
        notifyObserver();
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
        notifyObserver();
    }
}

void GameEngine::pause() {
    std::lock_guard lock(gameMutex);
    if (gameState == GameState::RUNNING) {
        tickTimer.stop();
        gameState = GameState::PAUSED;
        notifyObserver();
    }
}

void GameEngine::resume() {
    std::lock_guard lock(gameMutex);
    if (gameState == GameState::PAUSED) {
        gameState = GameState::RUNNING;

        const int level = scoreManager.getLevel();
        const int currentInterval = calculateGravityInterval(level);
        tickTimer.start(currentInterval);
        notifyObserver();
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
    notifyObserver();
}

void GameEngine::requestSave() const {
    std::lock_guard lock(gameMutex);
    if (gameState.load() != GameState::IDLE && gameState.load() != GameState::PAUSED) return;

    storageManager.saveGame();
}

void GameEngine::requestLoad() {
    std::lock_guard lock(gameMutex);

    if (const std::unique_ptr<Snapshot> loadedState = storageManager.loadGame()) {
        restoreFromSnapshot(*loadedState);
        gameState = GameState::LOADED;
    }

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

Snapshot GameEngine::createSnapshot() const {
    Snapshot snapshot{};

    snapshot.grid = board.getGrid();
    snapshot.score = scoreManager.getScore();
    snapshot.level = scoreManager.getLevel();
    snapshot.totalLinesCleared = scoreManager.getTotalLinesCleared();

    if (currentBlock) {
        snapshot.currentBlockType = currentBlock->getType();
        snapshot.currentBlockPosition = currentBlock->getPosition();
        snapshot.currentBlockRotation = currentBlock->getRotation();
    } else
        snapshot.currentBlockType = Cell::Empty;

    if (holdBlock) {
        snapshot.holdBlockType = holdBlock->getType();
    } else
        snapshot.holdBlockType = Cell::Empty;

    snapshot.bag = blockFactory.peekNext(7);

    return snapshot;
}

void GameEngine::restoreFromSnapshot(const Snapshot& snapshot) {
    reset();

    board.setGrid(snapshot.grid);
    scoreManager.restoreFromSnapshot(snapshot);

    if (snapshot.currentBlockType != Cell::Empty) {
        currentBlock = BlockFactory::createBlock(
            snapshot.currentBlockType,
            snapshot.currentBlockPosition,
            snapshot.currentBlockRotation
        );
    } else
        currentBlock.reset();

    if (snapshot.holdBlockType != Cell::Empty) {
        holdBlock = BlockFactory::createBlock(snapshot.holdBlockType);
    } else
        holdBlock.reset();

    blockFactory.loadFromSnapshot(snapshot);
    notifyObserver();
}