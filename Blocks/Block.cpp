#include "Block.h"

Block::Block(const Position p) : position(p), rotation(Rotation::R0), type(Cell::Empty){}

void Block::move(int dx, int dy) {

}

void Block::rotateCW() {
    auto newRotation = static_cast<Rotation>(
        (static_cast<int>(rotation) + 1) % 4);

    this->rotation = newRotation;
    calculateShape();
}

void Block::rotateCCW() {
    auto newRotation = static_cast<Rotation>(
        (static_cast<int>(rotation) - 1 + 4) % 4);

    this->rotation = newRotation;
    calculateShape();
}


std::vector<Position> Block::getGlobalCells() const {
    std::vector<Position> globalPositions;

    for (const auto& offset : shapeOffsets) {
        Position absolutePos = {
            position.x + offset.x,
            position.y + offset.y
        };
        globalPositions.push_back(absolutePos);
    }
    return globalPositions;
}
