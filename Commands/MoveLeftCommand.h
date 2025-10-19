#pragma once
#include "Commands/Command.h"

class MoveLeftCommand final : public Command {
    MoveLeftCommand() = default;
    MoveLeftCommand(const MoveLeftCommand&) = delete;
    MoveLeftCommand& operator=(const MoveLeftCommand&) = delete;
public:
    static MoveLeftCommand& getInstance();

    void execute() override;
};