#pragma once
#include "Command.h"

class RotateCWCommand final : public Command {
    RotateCWCommand() = default;
    RotateCWCommand(const RotateCWCommand&) = delete;
    RotateCWCommand& operator=(const RotateCWCommand&) = delete;
public:
    static RotateCWCommand& getInstance();

    void execute() override;
};
