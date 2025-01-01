// src/Engine.cpp

// Settings

#define VSync 0

#include "Engine.h"
#include <cstdio>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

// Dear ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Windows/RenderWindow.h"
#include "Windows/PerformanceWindow.h"
#include "Windows/LoggerWindow.h"
#include "Windows/InspectorWindow.h"
#include "Windows/SceneWindow.h"
#include "Windows/Icons.h"

#include "Windows/ProfilerWindow.h"

// Create an instance

// In your rendering loop

#include "Engine/ThemeManager.h"
#include "Engine/SceneManager.h"
#include "Engine/LuaAPI.h"
#include "Engine/Utilitys.h"

#include "Engine/ScopedTimer.h"
#include "Engine/Profiler.h"

// #define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "TestModel.h"

AssetManager g_AssetManager;

LoggerWindow *g_LoggerWindow;

SceneManager g_SceneManager;

std::vector<std::shared_ptr<GameObject>> g_GameObjects;

std::shared_ptr<CameraComponent> g_RuntimeCameraObject;

int g_GPU_Triangles_drawn_to_screen = 0;

GameObject *g_SelectedObject; // Pointer to the currently selected object

int m_GameRunning = 0;

bool MyEngine::Init(int width, int height, const std::string &title)
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
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#ifdef DEBUG
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
#endif

    // Path to your font files
    const char *regularFontPath = "assets/fonts/Roboto-Medium.ttf"; // Replace with your font file path
    const char *fontAwesomePath = "assets/fonts/fa-solid-900.ttf";  // Replace with Font Awesome font path

    float fontSize = 16.0f; // Font size for both regular font and icons

    // Load the regular font
    ImFont *regularFont = io.Fonts->AddFontFromFileTTF(regularFontPath, fontSize);
    if (!regularFont)
    {
        fprintf(stderr, "Failed to load regular font: %s\n", regularFontPath);
        return false;
    }

    // Configure Font Awesome
    ImFontConfig fontAwesomeConfig;
    fontAwesomeConfig.MergeMode = true;     // Merge into the main font
    fontAwesomeConfig.PixelSnapH = true;    // Snap pixels for better alignment
    fontAwesomeConfig.GlyphOffset.y = 2.0f; // Adjust vertical alignment (tweak this value if needed)

    // Define the glyph range for Font Awesome
    static const ImWchar fontAwesomeRanges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

    // Load Font Awesome and merge it into the regular font
    ImFont *fontAwesomeFont = io.Fonts->AddFontFromFileTTF(fontAwesomePath, fontSize, &fontAwesomeConfig, fontAwesomeRanges);
    if (!fontAwesomeFont)
    {
        fprintf(stderr, "Failed to load Font Awesome font: %s\n", fontAwesomePath);
        return false;
    }

    // Build the fonts
    io.Fonts->Build();

    // (Optional) Multi-viewport
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Style
    ImGui::StyleColorsDark();

    // Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize windows
    m_RenderWindow = std::make_unique<RenderWindow>();
    m_PerformanceWindow = std::make_unique<PerformanceWindow>();
    m_LoggerWindow = std::make_unique<LoggerWindow>();
    m_InspectorWindow = std::make_unique<InspectorWindow>();
    m_SceneWindow = std::make_unique<SceneWindow>();
    m_luaEditor = std::make_unique<LuaEditorWindow>();
    m_profilerWindow = std::make_unique<ProfilerWindow>();

    m_GameRunning = false;
    m_FirstTickGameRunning = true;
    m_showProfiler = false;

    g_LoggerWindow = m_LoggerWindow.get();

    // Optionally, call 'onInit' Lua function

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

    DEBUG_PRINT("Transition to Editor");
    // printf("%p\n", &g_GameObjects);

    // Possibly create more GameObjects with different positions or textures

    ThemeManager_ChangeTheme(2);
    DEBUG_PRINT("Changed Theme to default");

    while (!glfwWindowShouldClose(m_Window) && m_Running)
    {

        // Poll events
        {
            ScopedTimer timer("glfwPollEvents");
            glfwPollEvents();
        }

        // Calculate current time
        double current_time = glfwGetTime();

        // Calculate per-frame delta time
        double frame_delta = current_time - m_LastFrameTime;
        m_LastFrameTime = current_time;

        // Accumulate time for FPS calculation
        m_TimeAccumulator += frame_delta;
        m_FrameCount++;

        // Update FPS every 0.1 seconds
        if (m_TimeAccumulator >= 0.1)
        {
            m_Fps = static_cast<float>(m_FrameCount / m_TimeAccumulator);
            m_Ms = 100.0f / m_Fps; // Assuming m_Ms represents milliseconds per frame

            // Reset counters
            m_FrameCount = 0;
            m_TimeAccumulator = 0.0;
        }

        // Start new frame
        BeginFrame();

        if (m_FirstTickGameRunning && m_GameRunning)
        {

            ScopedTimer timer("SaveScene");
            m_FirstTickGameRunning = false;

            std::string savePath = createTempFolder().string() + "/TesseractEngineTempScene.scene";
            DEBUG_PRINT("Save path: %s", savePath.c_str());
            g_SceneManager.SaveScene(g_GameObjects, savePath);

            ScopedTimer LUA_INIT_timer("GameObjectsScriptInit");
            for (auto &Gameobject : g_GameObjects)
            {

                // Handle Components That Require Updates
                std::shared_ptr<ScriptComponent> script = Gameobject->GetComponent<ScriptComponent>();
                if (script)
                {                                                                         // Null Checks
                    ScopedTimer Lua_timer("GameObjectLuaCall_INIT: " + Gameobject->name); // var has to be named that or it will be redecl

                    script->Init();
                }
            }
        }

        if (!m_FirstTickGameRunning && !m_GameRunning)
        {
            ScopedTimer timer("LoadScene");
            m_FirstTickGameRunning = true;

            std::string loadPath = createTempFolder().string() + "/TesseractEngineTempScene.scene";

            DEBUG_PRINT("Load path: %s", loadPath.c_str());

            g_SceneManager.LoadScene(g_GameObjects, loadPath);
        }

        // Show main DockSpace
        ShowDockSpace();

        if (m_GameRunning)
        {
            ScopedTimer timer("UpdateGameObjects");
            for (auto &Gameobject : g_GameObjects)
            {

                Gameobject->Update(frame_delta);
            }
        }

        // Render and show various windows
        {
            ScopedTimer timer("RenderGame");

            m_RenderWindow->Show(&m_GameRunning); // The spinning triangle as ImGui::Image
        }
        {
            ScopedTimer timer("ShowEditor");

            m_InspectorWindow->Show();
            m_PerformanceWindow->Show(m_Fps, m_Ms); // FPS & ms
            m_LoggerWindow->Show();                 // Logs
            m_SceneWindow->Show();
            // m_luaEditor->Show();

            if (m_showProfiler)
            {
                m_profilerWindow->Show();
            }
        }

        // After rendering
        m_PerformanceWindow->UpdatePerformanceStats(-1, g_GPU_Triangles_drawn_to_screen);

        // End frame
        EndFrame();

        // Mark the end of frame for profiling
        Profiler::Get().EndFrame();
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
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
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
    // Initialize dockspace_flags without ImGuiDockNodeFlags_DockSpace
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // Style adjustments
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar(2);

    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                m_Running = false; // Stop the engine
            }
            if (ImGui::MenuItem("Save"))
            {
                m_LoggerWindow->AddLog("Saveing Scene", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
                g_SceneManager.SaveScene(g_GameObjects, "./scenes/Default.scene");
            }
            if (ImGui::MenuItem("Load"))
            {
                m_LoggerWindow->AddLog("Loading Scene", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));

                g_SceneManager.LoadScene(g_GameObjects, "./scenes/Default.scene");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::Checkbox("Show Profiler", &m_showProfiler); // Add a checkbox to toggle the profiler
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Engine"))
        {

            if (ImGui::MenuItem(m_GameRunning ? "Stop" : "Start"))
            {
                m_GameRunning = !m_GameRunning; // Stop the engine
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        // Optional: Log the flags for debugging
        // DEBUG_PRINT("DockSpace Flags: %d", dockspace_flags);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        ImGui::Text("Docking is not enabled. Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable.");
    }

    ImGui::End();
}
