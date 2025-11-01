#pragma once
#include "Block.h"

class JBlock final : public Block {
protected:
    void calculateShape() override;
public:
    JBlock(Position p, Rotation r);
};
