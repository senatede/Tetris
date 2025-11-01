#include "BlockFactory.h"
#include "../Blocks/IBlock.h"
#include "../Blocks/OBlock.h"
#include "../Blocks/TBlock.h"
#include "../Blocks/LBlock.h"
#include "../Blocks/JBlock.h"
#include "../Blocks/SBlock.h"
#include "../Blocks/ZBlock.h"
#include "SnapshotManagement/Snapshot.h"

BlockFactory::BlockFactory() : rng(BagGenerator::getInstance()) {};

BlockFactory& BlockFactory::getInstance() {
    static BlockFactory instance;
    return instance;
}

void BlockFactory::loadFromSnapshot(const Snapshot& snapshot) const {
    rng.setBag(snapshot.bag);
}

std::unique_ptr<Block> BlockFactory::createNextBlock(const Position& spawnPos) const {
    const Cell blockType = rng.next();
    return createBlock(blockType, spawnPos);
}

std::unique_ptr<Block> BlockFactory::createBlock(const Cell blockType, const Position& spawnPos, const Rotation& rotation) {
    switch (blockType) {
        case Cell::I: return std::make_unique<IBlock>(spawnPos, rotation);
        case Cell::O: return std::make_unique<OBlock>(spawnPos, rotation);
        case Cell::T: return std::make_unique<TBlock>(spawnPos, rotation);
        case Cell::L: return std::make_unique<LBlock>(spawnPos, rotation);
        case Cell::J: return std::make_unique<JBlock>(spawnPos, rotation);
        case Cell::S: return std::make_unique<SBlock>(spawnPos, rotation);
        case Cell::Z: return std::make_unique<ZBlock>(spawnPos, rotation);
        default: return nullptr;
    }
}

std::vector<Cell> BlockFactory::peekNext(const int count) const {
    return rng.peek(count);
}
