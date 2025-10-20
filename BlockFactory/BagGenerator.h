#pragma once
#include <random>
#include "../Board/Cell.h"

class BagGenerator {
    std::vector<Cell> bag;
    std::vector<Cell> nextBag;
    std::mt19937 generator;

    BagGenerator();
    BagGenerator(const BagGenerator&) = delete;
    BagGenerator& operator=(const BagGenerator&) = delete;

    std::vector<Cell> generateNewBag();
    void refillIfEmpty();
public:
    static BagGenerator& getInstance();

    Cell next();
    std::vector<Cell> peek(int count);
};