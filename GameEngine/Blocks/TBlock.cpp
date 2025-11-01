#include "TBlock.h"
#include <array>

static constexpr std::array<std::array<Position, 4>, 4> T_BLOCK_SHAPES = {{
    // R0
    {{ {-1, 0}, {0, 0}, {1, 0}, {0, 1} }},
    // R90
    {{ {0, 1}, {0, 0}, {0, -1}, {1, 0} }},
    // R180
    {{ {-1, 0}, {0, 0}, {1, 0}, {0, -1} }},
    // R270
    {{ {0, 1}, {0, 0}, {0, -1}, {-1, 0} }}
}};


TBlock::TBlock(const Position p, const Rotation r) : Block(p, r)
{
    this->type = Cell::T;
    calculateShape();
    SuperRotation = JLSTZ_WALL_KICK_DATA;
}

void TBlock::calculateShape() {
    int rotationIndex = static_cast<int>(rotation);

    this->shapeOffsets.assign(
        T_BLOCK_SHAPES[rotationIndex].begin(),
        T_BLOCK_SHAPES[rotationIndex].end()
    );
}
