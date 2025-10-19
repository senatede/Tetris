#include "OBlock.h"

OBlock::OBlock(const Position p)
    : Block(p)
{
    this->type = Cell::O;
    calculateShape();
}

void OBlock::calculateShape() {
    shapeOffsets = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1}
    };
}