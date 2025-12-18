#include <iostream>
#include <stdexcept>

#include "engine.h"

int main()
{
    try {
        Engine engine;
        engine.run();
    } catch (const std::runtime_error& error) {
        std::cout << error.what() << "\n";
    }
    Platform::shutdown();
}
