#include "HardDropCommand.h"

HardDropCommand& HardDropCommand::getInstance() {
    static HardDropCommand instance;
    return instance;
}

void HardDropCommand::execute() {
    if (engine) {
        engine->requestHardDrop();
    }
}