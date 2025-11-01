#pragma once
#include "Block.h"

class IBlock final : public Block {
protected:
    void calculateShape() override;
public:
    IBlock(Position p, Rotation r);
};