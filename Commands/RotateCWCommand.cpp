#include "RotateCWCommand.h"

RotateCWCommand& RotateCWCommand::getInstance() {
    static RotateCWCommand instance;
    return instance;
}

void RotateCWCommand::execute() {
    if (engine) {
        engine->requestRotate(true);
    }
}