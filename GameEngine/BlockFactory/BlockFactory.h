#pragma once
#include <random>
#include "BagGenerator.h"
#include "../Blocks/Block.h"

class BlockFactory {
    BagGenerator& rng;

    BlockFactory();
    BlockFactory(const BlockFactory&) = delete;
    BlockFactory& operator=(const BlockFactory&) = delete;

public:
    static BlockFactory& getInstance();

    void loadFromSnapshot(const Snapshot& snapshot) const;

    std::unique_ptr<Block> createNextBlock(const Position& spawnPos) const;
    static std::unique_ptr<Block> createBlock(Cell block, const Position& spawnPos = {0, 0}, const Rotation& rotation = Rotation::R0) ;
    std::vector<Cell> peekNext(int count) const;
};