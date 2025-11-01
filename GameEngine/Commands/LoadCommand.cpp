#include "LoadCommand.h"

LoadCommand& LoadCommand::getInstance() {
    static LoadCommand instance;
    return instance;
}

void LoadCommand::execute() {
    if (engine) {
        engine->requestLoad();
    }
}