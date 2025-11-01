#include "ScoreManager.h"
#include "../GameEngine.h"
#include "SnapshotManagement/Snapshot.h"

ScoreManager::ScoreManager() : leaderboard(Leaderboard::getInstance()) {}

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
    LinesCleared = 0;
}

void ScoreManager::restoreFromSnapshot(const Snapshot &snapshot) {
    score = snapshot.score;
    level = snapshot.level;
    totalLinesCleared = snapshot.totalLinesCleared;
}


void ScoreManager::setLevel(const int newLevel) {
    if (newLevel > 0) this->level = newLevel;
    engine->updateLevelSpeed();
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
            case 1:
                basePoints = 100;
                BackToBackTetrisPossibility = false;
                break;
            case 2:
                basePoints = 300;
                BackToBackTetrisPossibility = false;
                break;
            case 3:
                basePoints = 500;
                BackToBackTetrisPossibility = false;
                break;
            case 4:
                basePoints = BackToBackTetrisPossibility ? 1200 : 800;
                BackToBackTetrisPossibility = true;
                break;
            default: basePoints = 0;
        }
        score += basePoints * level;

        totalLinesCleared += linesCleared;
        LinesCleared += linesCleared;
        if (LinesCleared >= 10) {
            LinesCleared -= 10;
            level++;
            engine->updateLevelSpeed();
        }
    }
}

long long ScoreManager::getScore() const { return score; }
int ScoreManager::getLevel() const { return level; }
int ScoreManager::getTotalLinesCleared() const { return totalLinesCleared; }

bool ScoreManager::isAGoodScore() const { return leaderboard.isAGoodScore(score); }
bool ScoreManager::isANewRecord() const { return leaderboard.isANewRecord(score); }
void ScoreManager::saveScore(const std::string& name) const {
    leaderboard.addEntry(name, score);
    leaderboard.save();
}

std::vector<LeaderboardEntry> ScoreManager::getLeaderboard() const { return leaderboard.getEntries(); };
