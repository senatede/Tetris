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

    std::unique_ptr<Block> createNextBlock(const Position& spawnPos) const;
    static std::unique_ptr<Block> createBlock(Cell block, const Position& spawnPos) ;
    std::vector<Cell> peekNext(int count) const;
};