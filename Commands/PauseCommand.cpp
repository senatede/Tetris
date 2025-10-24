#include "PauseCommand.h"


PauseCommand& PauseCommand::getInstance() {
    static PauseCommand instance;
    return instance;
}

void PauseCommand::execute() {
    if (engine) {
        engine->pause();
    }
}