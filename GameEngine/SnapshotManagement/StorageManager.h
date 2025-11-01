#pragma once
#include <string>
#include <memory>
#include "Snapshot.h"

class GameEngine;

class StorageManager {
    const std::string saveFilePath = "tetris_save.dat";
    GameEngine* engine = nullptr;

    StorageManager() = default;
    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;

    void serialize(const Snapshot& snapshot) const;
    std::unique_ptr<Snapshot> deserialize() const;
public:
    static StorageManager& getInstance();
    void setGameEngine(GameEngine* engine);

    void saveGame() const; 
    std::unique_ptr<Snapshot> loadGame() const;
};