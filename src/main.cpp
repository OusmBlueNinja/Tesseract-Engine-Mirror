// src/main.cpp

#include <cstdio>
#include "Engine.h"




int main()
{
    DEBUG_PRINT("[START] Creating Global Engine ");
    // Small error where the constuctor would crash 
    MyEngine engine;
    DEBUG_PRINT("[OK] Creating Global Engine ");


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
