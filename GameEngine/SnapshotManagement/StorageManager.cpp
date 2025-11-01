#include "StorageManager.h"
#include "../GameEngine.h"
#include <fstream>
#include <sstream>
#include <iostream>

static int cellToInt(Cell c) { return static_cast<int>(c); }
static Cell intToCell(int i) { return static_cast<Cell>(i); }
static int rotationToInt(Rotation r) { return static_cast<int>(r); }
static Rotation intToRotation(int i) { return static_cast<Rotation>(i); }

StorageManager& StorageManager::getInstance() {
    static StorageManager instance;
    return instance;
}

void StorageManager::setGameEngine(GameEngine* engine) {
    this->engine = engine;
}


void StorageManager::serialize(const Snapshot& snapshot) const {
    std::ofstream out(saveFilePath);
    if (!out.is_open()) return;

    out << snapshot.score << ";" << snapshot.level << ";" << snapshot.totalLinesCleared << "\n";

    out << cellToInt(snapshot.currentBlockType) << ";"
        << snapshot.currentBlockPosition.x << ";" << snapshot.currentBlockPosition.y << ";"
        << rotationToInt(snapshot.currentBlockRotation) << "\n";

    out << cellToInt(snapshot.holdBlockType) << "\n";

    for (const Cell block : snapshot.bag) {
        out << cellToInt(block) << " ";
    }
    out << "\n";

    for (const auto& row : snapshot.grid) {
        for (const Cell cell : row) {
            out << cellToInt(cell) << " ";
        }
        out << "\n";
    }
}

std::unique_ptr<Snapshot> StorageManager::deserialize() const {
    std::ifstream in(saveFilePath);
    if (!in.is_open()) return nullptr;

    Snapshot snapshot{};
    std::string line;

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ';'); snapshot.score = std::stoll(token);
        std::getline(ss, token, ';'); snapshot.level = std::stoi(token);
        std::getline(ss, token, ';'); snapshot.totalLinesCleared = std::stoi(token);
    } else return nullptr;

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        int type, rot;
        ss >> type; snapshot.currentBlockType = intToCell(type);
        ss.ignore(1);
        ss >> snapshot.currentBlockPosition.x;
        ss.ignore(1);
        ss >> snapshot.currentBlockPosition.y;
        ss.ignore(1);
        ss >> rot; snapshot.currentBlockRotation = intToRotation(rot);
    } else return nullptr;

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        int type;
        ss >> type; snapshot.holdBlockType = intToCell(type);
    } else return nullptr;

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        int type;
        for (int i = 0; i < 7 && ss >> type; ++i) {
            snapshot.bag.emplace_back(intToCell(type));
        }
        std::reverse(snapshot.bag.begin(), snapshot.bag.end());
    } else return nullptr;

    int boardWidth = engine->getBoardSize().first;
    int boardHeight = engine->getBoardSize().second;
    snapshot.grid.resize(boardHeight, std::vector<Cell>(boardWidth, Cell::Empty));
    for (int y = 0; y < boardHeight; ++y) {
        if (std::getline(in, line)) {
            std::stringstream ss(line);
            int cellInt;
            for (int x = 0; x < boardWidth; ++x) {
                if (ss >> cellInt)
                    snapshot.grid[y][x] = intToCell(cellInt);
                else return nullptr;
            }
        } else return nullptr;
    }
    return std::make_unique<Snapshot>(snapshot);
}

void StorageManager::saveGame() const {
    if (!engine) return;

    const Snapshot snapshot = engine->createSnapshot();
    serialize(snapshot);
}

std::unique_ptr<Snapshot> StorageManager::loadGame() const {
    try {
        return deserialize();
    } catch (const std::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
        return nullptr;
    }
}
