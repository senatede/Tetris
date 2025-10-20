#include "LBlock.h"
#include <array>

static constexpr std::array<std::array<Position, 4>, 4> L_BLOCK_SHAPES = {{
    // R0
    {{ {-1, 0}, {0, 0}, {1, 0}, {1, 1} }},
    // R90
    {{ {0, 1}, {0, 0}, {0, -1}, {1, -1} }},
    // R180
    {{ {-1, 0}, {0, 0}, {1, 0}, {-1, -1} }},
    // R270
    {{ {0, 1}, {0, 0}, {0, -1}, {-1, 1} }}
}};

LBlock::LBlock(const Position p) : Block(p) {
    this->type = Cell::L;
    calculateShape();
}

void LBlock::calculateShape() {
    int rotationIndex = static_cast<int>(rotation);
    this->shapeOffsets.assign(
        L_BLOCK_SHAPES[rotationIndex].begin(),
        L_BLOCK_SHAPES[rotationIndex].end()
    );
}