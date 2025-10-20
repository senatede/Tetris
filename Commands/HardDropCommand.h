#pragma once
#include "Command.h"

class HardDropCommand final : public Command {
    HardDropCommand() = default;
    HardDropCommand(const HardDropCommand&) = delete;
    HardDropCommand& operator=(const HardDropCommand&) = delete;
public:
    static HardDropCommand& getInstance();

    void execute() override;
};