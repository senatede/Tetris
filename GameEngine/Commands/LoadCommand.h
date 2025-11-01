#pragma once
#include "Command.h"

class LoadCommand final : public Command {
    LoadCommand() = default;
    LoadCommand(const LoadCommand&) = delete;
    LoadCommand& operator=(const LoadCommand&) = delete;
public:
    static LoadCommand& getInstance();

    void execute() override;
};