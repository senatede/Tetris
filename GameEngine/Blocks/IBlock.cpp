#include "IBlock.h"

#include <array>

static constexpr std::array<std::array<Position, 4>, 4> I_BLOCK_SHAPES = {{
    // R0
    {{ {-1, 0}, {0, 0}, {1, 0}, {2, 0} }},
    // R90
    {{ {1, 1}, {1, 0}, {1, -1}, {1, -2} }},
    // R180
    {{ {-1, -1}, {0, -1}, {1, -1}, {2, -1} }},
    // R270
    {{ {0, 1}, {0, 0}, {0, -1}, {0, -2} }},
}};

static const std::map<std::pair<Rotation, Rotation>, std::vector<Position>> I_WALL_KICK_DATA = {
    { {Rotation::R0, Rotation::R90}, {
        Position{0, 0}, {-2, 0}, {+1, 0}, {-2, +1}, {+1, -2}
    } },
    { {Rotation::R90, Rotation::R0}, {
        Position{0, 0}, {+2, 0}, {-1, 0}, {+2, -1}, {-1, +2}
    } },
    { {Rotation::R90, Rotation::R180}, {
        Position{0, 0}, {-1, 0}, {+2, 0}, {-1, -2}, {+2, +1}
    } },
    { {Rotation::R180, Rotation::R90}, {
        Position{0, 0}, {+1, 0}, {-2, 0}, {+1, +2}, {-2, -1}
    } },
    { {Rotation::R180, Rotation::R270}, {
        Position{0, 0}, {+2, 0}, {-1, 0}, {+2, -1}, {-1, +2}
    } },
    { {Rotation::R270, Rotation::R180}, {
        Position{0, 0}, {-2, 0}, {+1, 0}, {-2, +1}, {+1, -2}
    } },
    { {Rotation::R270, Rotation::R0}, {
        Position{0, 0}, {+1, 0}, {-2, 0}, {+1, +2}, {-2, -1}
    } },
    { {Rotation::R0, Rotation::R270}, {
        Position{0, 0}, {-1, 0}, {+2, 0}, {-1, -2}, {+2, +1}
    } },
};

IBlock::IBlock(const Position p, const Rotation r)
    : Block(p, r)
{
    this->type = Cell::I;
    calculateShape();
    SuperRotation = I_WALL_KICK_DATA;
}

void IBlock::calculateShape() {
    const int rotationIndex = static_cast<int>(rotation);

    this->shapeOffsets.assign(
        I_BLOCK_SHAPES[rotationIndex].begin(),
        I_BLOCK_SHAPES[rotationIndex].end()
    );
}