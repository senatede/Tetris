#pragma once
#include "Leaderboard.h"

class GameEngine;

class ScoreManager {
    long long score = 0;
    int level = 1;
    int LinesCleared = 0;
    int totalLinesCleared = 0;
    bool BackToBackTetrisPossibility = false;
    GameEngine* engine = nullptr;
    Leaderboard& leaderboard;
public:
    ScoreManager();
    ScoreManager(const ScoreManager&) = delete;
    ScoreManager& operator=(const ScoreManager&) = delete;

    static ScoreManager& getInstance();
    void setGameEngine(GameEngine* gameEngine);
    
    void reset();
    void setLevel(int newLevel);
    void addHardDropPoints(int distance);
    void addSoftDropPoints();
    void addLineClear(int linesCleared);
    
    long long getScore() const;
    int getLevel() const;
    int getTotalLinesCleared() const;

    bool isAGoodScore() const;
    bool isANewRecord() const;
    void saveScore(const std::string& name) const;

    std::vector<LeaderboardEntry> getLeaderboard() const;
};