#pragma once
#include "Block.h"

class ZBlock final : public Block {
protected:
    void calculateShape() override;
public:
    ZBlock(Position p, Rotation r);
};
