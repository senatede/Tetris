#include "InputHandler.h"

InputHandler& InputHandler::getInstance() {
    static InputHandler instance;
    return instance;
};

void InputHandler::setGameEngine(GameEngine* gameEngine) {
    this->engine = gameEngine;
};

void InputHandler::bind(KeyType key, std::unique_ptr<Command> command) {
    bindings[key] = std::move(command);
}

void InputHandler::handleKey(KeyType key) {
    const auto it = bindings.find(key);
    if (it != bindings.end()) {
        it->second->execute();
    }
}