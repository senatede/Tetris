#pragma once
#include "Block.h"

class TBlock final : public Block {
protected:
    void calculateShape() override;
public:
    TBlock(Position p);
};