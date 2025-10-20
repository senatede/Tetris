#pragma once
#include <vector>
#include "Board/Position.h"
#include "Board/Cell.h"

enum class Rotation { R0 = 0, R90 = 1, R180 = 2, R270 = 3 };

class Block {
protected:
    Position position;
    Rotation rotation;
    std::vector<Position> shapeOffsets;
    Cell type;

    virtual void calculateShape() = 0;

public:
    Block(Position p);

    virtual ~Block() = default;

    void rotateCW();
    void rotateCCW();
    void move(int dx, int dy);

    Position getPosition() const;
    Cell getType() const;
    std::vector<Position> getGlobalCellsAt(const Position& newPos) const;
};