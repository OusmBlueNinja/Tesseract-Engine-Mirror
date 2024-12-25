// src/main.cpp

#include <cstdio>
#include "Engine.h"

int main()
{
    MyEngine engine;
    // Initialize the engine (creates GLFW window, sets up ImGui, etc.)
    if (!engine.Init(1280, 720, "Tesseract Engine"))
    {
        fprintf(stderr, "Failed to initialize engine.\n");
        return 1;
    }

    // Main loop
    engine.Run();

    // Cleanup
    engine.Cleanup();

    return 0;
}
