#include "ResumeCommand.h"


ResumeCommand& ResumeCommand::getInstance() {
    static ResumeCommand instance;
    return instance;
}

void ResumeCommand::execute() {
    if (engine) {
        engine->resume();
    }
}