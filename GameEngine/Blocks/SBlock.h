#pragma once
#include "Block.h"

class SBlock final : public Block {
protected:
    void calculateShape() override;
public:
    SBlock(Position p, Rotation r);
};
