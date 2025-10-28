#pragma once
#include "Command.h"

class ResumeCommand final : public Command {
    ResumeCommand() = default;
    ResumeCommand(const ResumeCommand&) = delete;
    ResumeCommand& operator=(const ResumeCommand&) = delete;
public:
    static ResumeCommand& getInstance();

    void execute() override;
};