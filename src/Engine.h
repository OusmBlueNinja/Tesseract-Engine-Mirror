// src/Engine.h

#pragma once
#include <string>
#include <memory>
#include "Windows/RenderWindow.h"
#include "Windows/PerformanceWindow.h"
#include "Windows/LoggerWindow.h"
#include "Engine/AssetManager.h"
#include "Windows/InspectorWindow.h"
#include "Windows/SceneWindow.h"


#include "Componenets/GameObject.h"
#include "Componenets/Mesh.h"
#include "Componenets/Transform.h"

#include "TestModel.h"



#include "gcml.h"

// Forward declaration to avoid including GLFW in the header if you prefer
struct GLFWwindow;

// The main engine class that owns the application loop
class MyEngine
{
public:
    

    bool Init(int width, int height, const std::string& title);
    void Run();
    void Cleanup();

private:
    // Internal helpers
    void BeginFrame();
    void EndFrame();
    void ShowDockSpace();

private:



    GLFWwindow* m_Window = nullptr;
    bool m_Running = false;

    // Windows
    std::unique_ptr<RenderWindow>       m_RenderWindow;
    std::unique_ptr<PerformanceWindow>  m_PerformanceWindow;
    std::unique_ptr<LoggerWindow>       m_LoggerWindow;
    std::unique_ptr<InspectorWindow>    m_InspectorWindow;
    std::unique_ptr<SceneWindow>        m_SceneWindow;



    // For FPS calculation
    float m_Fps = 0.0f;
    float m_Ms  = 0.0f;
    double m_LastTime = 0.0;
    int m_FrameCount  = 0;
};
