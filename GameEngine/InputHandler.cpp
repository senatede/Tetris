#include "InputHandler.h"
#include "Commands/MoveLeftCommand.h"
#include "Commands/MoveRightCommand.h"
#include "Commands/RotateCWCommand.h"
#include "Commands/RotateCCWCommand.h"
#include "Commands/HardDropCommand.h"
#include "Commands/SoftDropCommand.h"
#include "Commands/HoldCommand.h"
#include "Commands/PauseCommand.h"
#include "Commands/ResumeCommand.h"

InputHandler::InputHandler() {
    bind(KeyType::LEFT, &MoveLeftCommand::getInstance());
    bind(KeyType::RIGHT, &MoveRightCommand::getInstance());
    bind(KeyType::ROTATE_CW, &RotateCWCommand::getInstance());
    bind(KeyType::ROTATE_CCW, &RotateCCWCommand::getInstance());
    bind(KeyType::SOFT_DROP, &SoftDropCommand::getInstance());
    bind(KeyType::HARD_DROP, &HardDropCommand::getInstance());
    bind(KeyType::HOLD, &HoldCommand::getInstance());
    bind(KeyType::PAUSE, &PauseCommand::getInstance());
    bind(KeyType::RESUME, &ResumeCommand::getInstance());
}

InputHandler& InputHandler::getInstance() {
    static InputHandler instance;
    return instance;
};

void InputHandler::setGameEngine(GameEngine* gameEngine) {
    this->engine = gameEngine;

    for (auto const& [key, command] : bindings) {
        if (command) {
            command->setGameEngine(gameEngine);
        }
    }
};

void InputHandler::bind(KeyType key, Command* command) {
    bindings[key] = command;
}

void InputHandler::handleKey(const KeyType key) {
    const auto it = bindings.find(key);
    if (it != bindings.end()) {
        it->second->execute();
    }
}