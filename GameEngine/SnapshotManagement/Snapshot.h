#pragma once
#include <vector>
#include "Board/Cell.h"
#include "Blocks/Block.h"
#include "Board/Position.h"

class Block;

struct Snapshot {
    std::vector<std::vector<Cell>> grid;

    long long score;
    int level;
    int totalLinesCleared;

    Cell currentBlockType;
    Position currentBlockPosition;
    Rotation currentBlockRotation;

    Cell holdBlockType;

    std::vector<Cell> bag;
};