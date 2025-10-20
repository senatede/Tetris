#include "HoldCommand.h"


HoldCommand& HoldCommand::getInstance() {
    static HoldCommand instance;
    return instance;
}

void HoldCommand::execute() {
    if (engine) {
        engine->requestHold();
    }
}