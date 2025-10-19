#include "GameEngine.h"

GameEngine::GameEngine() :
    board(), // Ініціалізуємо Board
    scoreManager(), // Ініціалізуємо ScoreManager
    storageManager(), // Ініціалізуємо StorageManager
    blockFactory(), // Ініціалізуємо BlockFactory
    inputHandler(InputHandler::getInstance()),
    currentBlock(nullptr), // Починаємо без поточної фігури
    holdBlock(nullptr) // Починаємо без затриманої фігури
{
    inputHandler.setGameEngine(*this);
    gameState = State::Idle;
}