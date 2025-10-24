#include "ScoreManager.h"
#include "GameEngine.h"

ScoreManager::ScoreManager() : score(0), level(1), totalLinesCleared(0) {}

ScoreManager& ScoreManager::getInstance() {
    static ScoreManager instance;
    return instance;
}

void ScoreManager::setGameEngine(GameEngine* gameEngine) {
    this->engine = gameEngine;
};

void ScoreManager::reset() {
    score = 0;
    level = 1;
    totalLinesCleared = 0;
}

void ScoreManager::setLevel(const int newLevel) {
    if (newLevel > 0) this->level = newLevel;
}

void ScoreManager::addHardDropPoints(const int distance) {
    score += distance * 2;
}

void ScoreManager::addSoftDropPoints() {
    score += 1;
}

void ScoreManager::addLineClear(const int linesCleared) {
    if (linesCleared > 0) {
        long long basePoints = 0;
        switch (linesCleared) {
            case 1: basePoints = 100; break;
            case 2: basePoints = 300; break;
            case 3: basePoints = 500; break;
            case 4: basePoints = 800; break;
            default: basePoints = 0;
        }
        score += basePoints * level;

        totalLinesCleared += linesCleared;
        const int newLevel = (totalLinesCleared / 10) + 1;
        if (newLevel > level) {
            level = newLevel;
            engine->updateLevelSpeed();
        }
    }
}

long long ScoreManager::getScore() const { return score; }
int ScoreManager::getLevel() const { return level; }
int ScoreManager::getTotalLinesCleared() const { return totalLinesCleared; }