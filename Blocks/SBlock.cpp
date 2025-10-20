#include "SBlock.h"
#include <array>

static constexpr std::array<std::array<Position, 4>, 4> S_BLOCK_SHAPES = {{
    // R0
    {{ {-1, 0}, {0, 0}, {0, 1}, {1, 1} }},
    // R90
    {{ {0, 1}, {0, 0}, {1, 0}, {1, -1} }},
    // R180
    {{ {0, 0}, {1, 0}, {-1, -1}, {0, -1} }},
    // R270
    {{ {-1, 1}, {-1, 0}, {0, 0}, {0, -1} }}
}};

void SBlock::calculateShape() {
    int rotationIndex = static_cast<int>(rotation);
    this->shapeOffsets.assign(
        S_BLOCK_SHAPES[rotationIndex].begin(),
        S_BLOCK_SHAPES[rotationIndex].end()
    );
}