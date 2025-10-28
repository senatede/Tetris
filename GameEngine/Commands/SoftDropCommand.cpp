#include "SoftDropCommand.h"

SoftDropCommand& SoftDropCommand::getInstance() {
    static SoftDropCommand instance;
    return instance;
}

void SoftDropCommand::execute() {
    if (engine) {
        engine->requestSoftDrop();
    }
}