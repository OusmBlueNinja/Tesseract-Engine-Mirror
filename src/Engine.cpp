// src/Engine.cpp



// Settings

#define VSync 0

#include "Engine.h"
#include <cstdio>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Dear ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Windows/RenderWindow.h"
#include "Windows/PerformanceWindow.h"
#include "Windows/LoggerWindow.h"





AssetManager g_AssetManager;

bool MyEngine::Init(int width, int height, const std::string& title)
{
    DEBUG_PRINT("[START] Engine Init");
    // ------------------------------------------
    // 1) Initialize GLFW
    // ------------------------------------------
    if (!glfwInit())
    {
        fprintf(stderr, "[Engine] Failed to initialize GLFW\n");
        return false;
    }

    // Setup hints for OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        fprintf(stderr, "[Engine] Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(VSync); // vsync

    // ------------------------------------------
    // 2) Initialize GLEW
    // ------------------------------------------
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "[Engine] Failed to initialize GLEW\n");
        return false;
    }

    // ------------------------------------------
    // 3) Initialize ImGui
    // ------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // (Optional) Multi-viewport
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Style
    ImGui::StyleColorsDark();

    // Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize windows
    m_RenderWindow      = std::make_unique<RenderWindow>();
    m_PerformanceWindow = std::make_unique<PerformanceWindow>();
    m_LoggerWindow      = std::make_unique<LoggerWindow>();

    // Some initial logs
    m_LoggerWindow->AddLog("Engine initialized.");
    m_LoggerWindow->AddLog("Welcome to Tesseract Engine!");

    m_Running = true;
    m_LastTime = glfwGetTime();
    DEBUG_PRINT("[OK] Engine Init ");

    return true;
}

void MyEngine::Run()
{
    DEBUG_PRINT("[START] Engine Run ");
    
    while (!glfwWindowShouldClose(m_Window) && m_Running)
    {
        // Poll
        glfwPollEvents();

        // Calculate FPS
        double current_time = glfwGetTime();
        double delta = current_time - m_LastTime;
        m_FrameCount++;
        if (delta >= 1.0)
        {
            m_Fps = (float)(m_FrameCount / delta);
            m_Ms  = 1000.0f / m_Fps;
            m_FrameCount = 0;
            m_LastTime = current_time;
        }

        // Start new frame
        BeginFrame();

        // Show main DockSpace
        ShowDockSpace();

        // Show our windows
        m_RenderWindow->Show();                    // The spinning triangle as ImGui::Image
        m_PerformanceWindow->Show(m_Fps, m_Ms);    // FPS & ms
        m_LoggerWindow->Show();                    // Logs

        // After rendering
        m_PerformanceWindow->UpdatePerformanceStats(-1, -1);


        // End frame
        EndFrame();
    }
    DEBUG_PRINT("[OK] Engine Run ");

}

void MyEngine::Cleanup()
{
    DEBUG_PRINT("[START] Engine Cleanup ");
    
    // ImGui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // GLFW cleanup
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
    glfwTerminate();

    m_Running = false;
    DEBUG_PRINT("[OK] Engine Cleanup ");

}

void MyEngine::BeginFrame()
{
    // ImGui new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void MyEngine::EndFrame()
{
    // Render ImGui
    ImGui::Render();

    // Clear the default framebuffer
    int display_w, display_h;
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.05f, 0.05f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the ImGui data
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // (Optional) handle multi-viewport
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    // Swap
    glfwSwapBuffers(m_Window);
}

void MyEngine::ShowDockSpace()
{
    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoTitleBar
                      | ImGuiWindowFlags_NoCollapse
                      | ImGuiWindowFlags_NoResize
                      | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus
                      | ImGuiWindowFlags_NoNavFocus;
    }

    // Style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar(2);

    // Menu bar example
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
                m_Running = false; // Stop the engine
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        ImGui::Text("Docking is not enabled. Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable.");
    }

    ImGui::End();
}
