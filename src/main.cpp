// src/main.cpp

#include <cstdio>
#include "Engine.h"




int main()
{
    DEBUG_PRINT("[START] Creating Global Engine ");

    MyEngine engine;
    DEBUG_PRINT("[OK] Creating Global Engine ");

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
