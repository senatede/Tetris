#include "Block.h"

Block::Block(const Position p) : position(p), rotation(Rotation::R0), type(Cell::Empty){}

Position Block::getPosition() const {
    return position;
}

Cell Block::getType() const {
    return type;
}

void Block::move(const int dx, const int dy) {
    position.x += dx;
    position.y += dy;
}

void Block::rotateCW() {
    const auto newRotation = static_cast<Rotation>(
        (static_cast<int>(rotation) + 1) % 4);

    this->rotation = newRotation;
    calculateShape();
}

void Block::rotateCCW() {
    const auto newRotation = static_cast<Rotation>(
        (static_cast<int>(rotation) - 1 + 4) % 4);

    this->rotation = newRotation;
    calculateShape();
}

std::vector<Position> Block::getGlobalCellsAt(const Position& newPos) const {
    std::vector<Position> globalPositions;

    for (const auto& offset : shapeOffsets) {
        Position absolutePos = {
            newPos.x + offset.x,
            newPos.y + offset.y
        };
        globalPositions.push_back(absolutePos);
    }
    return globalPositions;
}