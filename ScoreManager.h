#pragma once

class GameEngine;

class ScoreManager {
    long long score;
    int level;
    int totalLinesCleared;
    GameEngine* engine = nullptr;
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
};