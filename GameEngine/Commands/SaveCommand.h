#pragma once
#include "Command.h"

class SaveCommand final : public Command {
    SaveCommand() = default;
    SaveCommand(const SaveCommand&) = delete;
    SaveCommand& operator=(const SaveCommand&) = delete;
public:
    static SaveCommand& getInstance();

    void execute() override;
};