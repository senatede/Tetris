#include "SaveCommand.h"

SaveCommand& SaveCommand::getInstance() {
    static SaveCommand instance;
    return instance;
}

void SaveCommand::execute() {
    if (engine) {
        engine->requestSave();
    }
}