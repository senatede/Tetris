#include "RotateCCWCommand.h"


RotateCCWCommand& RotateCCWCommand::getInstance() {
    static RotateCCWCommand instance;
    return instance;
}

void RotateCCWCommand::execute() {
    if (engine) {
        engine->requestRotate(false);
    }
}