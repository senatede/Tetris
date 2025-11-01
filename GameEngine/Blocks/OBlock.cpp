#include "OBlock.h"

OBlock::OBlock(const Position p, const Rotation r) : Block(p, r)
{
    this->type = Cell::O;
    calculateShape();
    SuperRotation = {};
}

void OBlock::calculateShape() {
    shapeOffsets = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1}
    };
}