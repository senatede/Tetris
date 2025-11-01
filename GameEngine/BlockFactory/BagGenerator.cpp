#include "BagGenerator.h"
#include <algorithm>

#include "SnapshotManagement/Snapshot.h"


BagGenerator::BagGenerator() : generator(static_cast<unsigned int>(std::time(nullptr))) {
    nextBag = generateNewBag();
    refillIfEmpty();
}

BagGenerator& BagGenerator::getInstance() {
    static BagGenerator instance;
    return instance;
}

void BagGenerator::setBag(std::vector<Cell> newBag) {
    bag = std::move(newBag);
}

std::vector<Cell> BagGenerator::generateNewBag() {
    std::vector<Cell> newBag;
    newBag.reserve(7);

    newBag.push_back(Cell::I);
    newBag.push_back(Cell::O);
    newBag.push_back(Cell::T);
    newBag.push_back(Cell::L);
    newBag.push_back(Cell::J);
    newBag.push_back(Cell::S);
    newBag.push_back(Cell::Z);

    std::shuffle(newBag.begin(), newBag.end(), generator);
    return newBag;
}


void BagGenerator::refillIfEmpty() {
    if (bag.empty()) {
        bag = std::move(nextBag);
        nextBag = generateNewBag();
    }
}

Cell BagGenerator::next() {
    refillIfEmpty();

    const Cell nextBlockType = bag.back();
    bag.pop_back();

    return nextBlockType;
}


std::vector<Cell> BagGenerator::peek(const int count) {
    refillIfEmpty();

    std::vector<Cell> preview;
    preview.reserve(count);

    const int currentSize = bag.size();
    for (int i = 0; i < count && i < currentSize; ++i) {
        preview.push_back(bag[currentSize - 1 - i]);
    }
    const int neededFromNextBag = count - preview.size();

    if (neededFromNextBag > 0) {
        const int nextSize = nextBag.size();

        for (int i = 0; i < neededFromNextBag && i < nextBag.size(); ++i) {
            preview.push_back(nextBag[nextSize - 1- i]);
        }
    }

    return preview;
}