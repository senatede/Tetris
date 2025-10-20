#include "ZBlock.h"
#include <array>

static constexpr std::array<std::array<Position, 4>, 4> Z_BLOCK_SHAPES = {{
    // R0
    {{ {-1, 1}, {0, 1}, {0, 0}, {1, 0} }},
    // R90
    {{ {1, 1}, {0, 0}, {1, 0}, {0, -1} }},
    // R180
    {{ {-1, 0}, {0, 0}, {0, -1}, {1, -1} }},
    // R270
    {{ {0, 1}, {-1, 0}, {0, 0}, {-1, -1} }}
}};

ZBlock::ZBlock(const Position p) : Block(p) {
    this->type = Cell::Z;
    calculateShape();
}

void ZBlock::calculateShape() {
    int rotationIndex = static_cast<int>(rotation);
    this->shapeOffsets.assign(
        Z_BLOCK_SHAPES[rotationIndex].begin(),
        Z_BLOCK_SHAPES[rotationIndex].end()
    );
}