#pragma once
#include "Command.h"

class PauseCommand final : public Command {
    PauseCommand() = default;
    PauseCommand(const PauseCommand&) = delete;
    PauseCommand& operator=(const PauseCommand&) = delete;
public:
    static PauseCommand& getInstance();

    void execute() override;
};