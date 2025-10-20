#pragma once
#include "Command.h"

class SoftDropCommand final : public Command {
    SoftDropCommand() = default;
    SoftDropCommand(const SoftDropCommand&) = delete;
    SoftDropCommand& operator=(const SoftDropCommand&) = delete;
public:
    static SoftDropCommand& getInstance();

    void execute() override;
};