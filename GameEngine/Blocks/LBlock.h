#pragma once
#include "Block.h"

class LBlock final : public Block {
protected:
    void calculateShape() override;
public:
    LBlock(Position p, Rotation r);
};
