#include <iostream>

#include "engine.h"

int main()
{
    try {
        Engine engine;
        engine.run();
    } catch (const Error& error) {
        std::cout << error.what() << "\n";
    }
    Platform::shutdown();
}
