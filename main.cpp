#include "Renderer/Renderer.h"

int main() {
    const auto renderer = std::make_shared<Renderer>();
    renderer->initializeObserver();
    renderer->run();
    return 0;
}