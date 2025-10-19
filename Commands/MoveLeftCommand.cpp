#include "MoveLeftCommand.h"


MoveLeftCommand& MoveLeftCommand::getInstance() {
    static MoveLeftCommand instance;
    return instance;
}

void MoveLeftCommand::execute() {
    if (engine) {
        engine->requestMove(-1);
    }
}