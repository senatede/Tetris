#pragma once
#include <vector>
#include <memory>
#include <cmath>
#include <mutex>

class Board;
class ScoreManager;
class InputHandler;
class BlockFactory;
class Block;
class Timer;
enum class Cell;
struct Position;

enum class GameState { IDLE, RUNNING, PAUSED, GAME_OVER };

class GameEngine {
    Board& board;
    ScoreManager& scoreManager;
    // StorageManager storageManager;
    InputHandler& inputHandler;
    BlockFactory& blockFactory;
    std::unique_ptr<Block> holdBlock;
    std::unique_ptr<Block> currentBlock;
    Timer& tickTimer;
    std::atomic<GameState> gameState;
    bool hasHeldThisTurn = false;
    int peekNextN = 3;

    mutable std::recursive_mutex gameMutex;

    GameEngine(int boardWidth, int boardHeight, InputHandler& input_handler, Timer& tick_timer, ScoreManager& score_manager);
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    void spawnNextBlock();
public:
    static GameEngine& getInstance(int boardWidth, int boardHeight, InputHandler& input_handler, Timer& tick_timer, ScoreManager& score_manager);

    void startGame();
    void pause();
    void resume();
    void tick();
    void reset();

    void requestMove(int dx) const;
    void requestRotate(bool clockwise) const;
    void requestHardDrop();
    void requestSoftDrop() const;
    void requestHold();
    void updateLevelSpeed() const;

    GameState getGameState() const;
    static int calculateGravityInterval(int level) ;
    std::vector<std::vector<Cell>> getRenderGrid() const;
    std::vector<std::vector<Cell>> getRenderHold() const;
    std::vector<std::vector<std::vector<Cell>>> getRenderNext() const;
};