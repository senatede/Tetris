#include "MoveRightCommand.h"


MoveRightCommand& MoveRightCommand::getInstance() {
    static MoveRightCommand instance;
    return instance;
}

void MoveRightCommand::execute() {
    if (engine) {
        engine->requestMove(1);
    }
}