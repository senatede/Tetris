#include "IBlock.h"

#include <array>

static constexpr std::array<std::array<Position, 4>, 4> I_BLOCK_SHAPES = {{
    // R0
    {{ {-1, 0}, {0, 0}, {1, 0}, {2, 0} }},
    // R90
    {{ {1, -1}, {1, 0}, {1, 1}, {1, 2} }},
    // R180
    {{ {-1, 1}, {0, 1}, {1, 1}, {2, 1} }},
    // R270
    {{ {0, -1}, {0, 0}, {0, 1}, {0, 2} }},

}};

IBlock::IBlock(const Position p)
    : Block(p)
{
    this->type = Cell::I;
    calculateShape();
}

void IBlock::calculateShape() {
    int rotationIndex = static_cast<int>(rotation);

    this->shapeOffsets.assign(
        I_BLOCK_SHAPES[rotationIndex].begin(),
        I_BLOCK_SHAPES[rotationIndex].end()
    );
}