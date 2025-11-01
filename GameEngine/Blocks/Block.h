#pragma once
#include <vector>
#include <map>
#include "../Board/Position.h"
#include "../Board/Cell.h"

enum class Rotation { R0 = 0, R90 = 1, R180 = 2, R270 = 3 };

class Block {
protected:
    Position position;
    Rotation rotation;
    std::vector<Position> shapeOffsets;
    Cell type;
    std::map<std::pair<Rotation, Rotation>, std::vector<Position>> SuperRotation;

    virtual void calculateShape() = 0;

public:
    Block(Position p, Rotation r);

    virtual ~Block() = default;

    void rotateCW();
    void rotateCCW();
    void move(int dx, int dy);

    Position getPosition() const;
    void setPosition(Position pos);
    Rotation getRotation() const;
    void resetRotation();
    Cell getType() const;
    std::vector<Position> getGlobalCellsAt(const Position& newPos) const;
    std::vector<Position> getSuperRotationOffSets(Rotation from, Rotation to) const;
};


static const std::map<std::pair<Rotation, Rotation>, std::vector<Position>> JLSTZ_WALL_KICK_DATA = {
    { {Rotation::R0, Rotation::R90}, {
        Position{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}
    } },
    { {Rotation::R90, Rotation::R0}, {
        Position{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}
    } },
    { {Rotation::R90, Rotation::R180}, {
        Position{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}
    } },
    { {Rotation::R180, Rotation::R90}, {
        Position{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}
    } },
    { {Rotation::R180, Rotation::R270}, {
        Position{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}
    } },
    { {Rotation::R270, Rotation::R180}, {
        Position{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}
    } },
    { {Rotation::R270, Rotation::R0}, {
        Position{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}
    } },
    { {Rotation::R0, Rotation::R270}, {
        Position{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}
    } },
};