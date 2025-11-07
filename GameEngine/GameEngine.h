#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include "Board/Cell.h"
#include "IObserver.h"

struct Snapshot;
class Board;
class ScoreManager;
class StorageManager;
class InputHandler;
class BlockFactory;
class Block;
class Timer;
struct Position;

enum class GameState { IDLE, LOADED, RUNNING, PAUSED, GAME_OVER };

struct RenderData {
    std::vector<std::vector<Cell>> grid;
    Cell holdType = Cell::Empty;
    std::vector<Cell> nextTypes;
    long long score = 0;
    int level = 1;
    int totalLinesCleared = 0;
    GameState gameState = GameState::IDLE;
};

class GameEngine {
    int boardWidth = 10, boardHeight = 20;

    Board& board;
    ScoreManager& scoreManager;
    StorageManager& storageManager;
    InputHandler& inputHandler;
    BlockFactory& blockFactory;
    std::unique_ptr<Block> holdBlock;
    std::unique_ptr<Block> currentBlock;
    Timer& tickTimer;
    std::atomic<GameState> gameState;
    bool hasHeldThisTurn = false;
    int peekNextN = 3;

    // Soft lock:
    std::chrono::time_point<std::chrono::steady_clock> lockTimeStart;
    const std::chrono::milliseconds LOCK_DELAY = std::chrono::milliseconds(500);
    bool isSoftLocked = false;
    int lockResetCount = 0;
    const int MAX_LOCK_RESETS = 15;

    mutable std::recursive_mutex gameMutex;
    std::shared_ptr<IObserver> observer = nullptr;
    void notifyObserver();

    RenderData cachedRenderData;

    GameEngine(int boardWidth, int boardHeight, InputHandler& input_handler, ScoreManager& score_manager);
    ~GameEngine();

    void spawnNextBlock();
public:
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    static GameEngine& getInstance(int boardWidth, int boardHeight, InputHandler& input_handler, ScoreManager& score_manager);

    void setObserver(std::shared_ptr<IObserver> observer);

    void startNewGame(int level = 1);
    void startGame();
    void pause();
    void resume();
    void tick();
    void reset();

    void requestMove(int dx);
    void requestRotate(bool clockwise);
    void requestHardDrop();
    void requestSoftDrop();
    void requestHold();
    void requestSave() const;
    void requestLoad();
    void updateLevelSpeed() const;

    GameState getGameState() const;
    std::pair<int, int> getBoardSize() const;
    static int calculateGravityInterval(int level) ;

    const RenderData& getRenderData();

    Snapshot createSnapshot() const;
    void restoreFromSnapshot(const Snapshot& snapshot);
};