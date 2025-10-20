#pragma once
#include "Command.h"

class RotateCCWCommand final : public Command {
    RotateCCWCommand() = default;
    RotateCCWCommand(const RotateCCWCommand&) = delete;
    RotateCCWCommand& operator=(const RotateCCWCommand&) = delete;
public:
    static RotateCCWCommand& getInstance();

    void execute() override;
};
