#pragma once
#include "Command.h"

class HoldCommand final : public Command {
    HoldCommand() = default;
    HoldCommand(const HoldCommand&) = delete;
    HoldCommand& operator=(const HoldCommand&) = delete;
public:
    static HoldCommand& getInstance();

    void execute() override;
};