#include "BlockFactory.h"
#include "../Blocks/IBlock.h"
#include "../Blocks/OBlock.h"
#include "../Blocks/TBlock.h"
#include "../Blocks/LBlock.h"
#include "../Blocks/JBlock.h"
#include "../Blocks/SBlock.h"
#include "../Blocks/ZBlock.h"

BlockFactory::BlockFactory() : rng(BagGenerator::getInstance()) {};

BlockFactory& BlockFactory::getInstance() {
    static BlockFactory instance;
    return instance;
}

std::unique_ptr<Block> BlockFactory::createNextBlock(const Position& spawnPos) const {
    const Cell blockType = rng.next();
    return createBlock(blockType, spawnPos);
}

std::unique_ptr<Block> BlockFactory::createBlock(const Cell blockType, const Position& spawnPos) {
    switch (blockType) {
        case Cell::I: return std::make_unique<IBlock>(spawnPos);
        case Cell::O: return std::make_unique<OBlock>(spawnPos);
        case Cell::T: return std::make_unique<TBlock>(spawnPos);
        case Cell::L: return std::make_unique<LBlock>(spawnPos);
        case Cell::J: return std::make_unique<JBlock>(spawnPos);
        case Cell::S: return std::make_unique<SBlock>(spawnPos);
        case Cell::Z: return std::make_unique<ZBlock>(spawnPos);
        default: return nullptr;
    }
}

std::vector<Cell> BlockFactory::peekNext(const int count) const {
    return rng.peek(count);
}
