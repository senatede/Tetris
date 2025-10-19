#pragma once
#include "Block.h"

class OBlock final : public Block {
protected:
    void calculateShape() override;
public:
    OBlock(Position p);
};
