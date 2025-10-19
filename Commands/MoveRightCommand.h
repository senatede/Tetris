#pragma once
#include "Commands/Command.h"

class MoveRightCommand final : public Command {
    MoveRightCommand() = default;
    MoveRightCommand(const MoveRightCommand&) = delete;
    MoveRightCommand& operator=(const MoveRightCommand&) = delete;
public:
    static MoveRightCommand& getInstance();

    void execute() override;
};