#include "JBlock.h"
#include <array>
#include <map>

static constexpr std::array<std::array<Position, 4>, 4> J_BLOCK_SHAPES = {{
    // R0
    {{ {-1, 1}, {-1, 0}, {0, 0}, {1, 0} }},
    // R90
    {{ {0, 1}, {1, 1}, {0, 0}, {0, -1} }},
    // R180
    {{ {-1, 0}, {0, 0}, {1, 0}, {1, -1} }},
    // R270
    {{ {0, 1}, {0, 0}, {0, -1}, {-1, -1} }}
}};

JBlock::JBlock(const Position p) : Block(p) {
    this->type = Cell::J;
    calculateShape();
    SuperRotation = JLSTZ_WALL_KICK_DATA;
}

void JBlock::calculateShape() {
    int rotationIndex = static_cast<int>(rotation);
    this->shapeOffsets.assign(
        J_BLOCK_SHAPES[rotationIndex].begin(),
        J_BLOCK_SHAPES[rotationIndex].end()
    );
}