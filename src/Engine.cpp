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

#include "Engine/ThemeManager.h"
#include "Engine/SceneManager.h"
#include "Engine/LuaAPI.h"

// #define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "TestModel.h"

AssetManager g_AssetManager;

LoggerWindow *g_LoggerWindow;

SceneManager g_SceneManager;

std::vector<std::shared_ptr<GameObject>> g_GameObjects;

int g_GPU_Triangles_drawn_to_screen = 0;

GameObject *g_SelectedObject; // Pointer to the currently selected object

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

    // Pseudocode:
    int newId = g_GameObjects.size();
    auto newGameObject = std::make_shared<GameObject>(newId, ("Default"));

    DEBUG_PRINT("Created Default GameObject");

    newGameObject->AddComponent(std::make_shared<TransformComponent>());
    newGameObject->AddComponent(std::make_shared<MeshComponent>());

    DEBUG_PRINT("Added Componenets");

    // Suppose we loaded a VAO, an EBO with 36 indices for the cube,
    // and a texture ID from the asset manager
    auto mesh = newGameObject->GetComponent<MeshComponent>();
    auto transform = newGameObject->GetComponent<TransformComponent>();

    DEBUG_PRINT("Got pointers to Componenets");

    if (mesh)
    {
        // printf("Got Valid Mesh Component\n");
        mesh->vao = CreateCubeVAO();
        mesh->indexCount = 36;
        mesh->textureID = g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/wood.png");
    }
    else
    {

        DEBUG_PRINT("Could not find Mesh Component\n");
    }

    if (transform)
    {
        // printf("Got Valid Transform Component\n");
        transform->position = glm::vec3(0.f, 0.f, 0.f);
        transform->rotation = glm::vec3(0.f, 0.5f, 0.f);
        transform->scale = glm::vec3(1.f, 1.f, 1.f);
    }
    else
    {

        DEBUG_PRINT("Could not find Transform Component");
    }

    g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/bricks.png");
    g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/default.png");
    g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/lush_grass.png");
    g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/vegetation_tree_bark_40.png");

    g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/sky.png");

    // Load a model
    Model *modelPtr = g_AssetManager.loadAsset<Model *>(AssetType::MODEL, "assets/models/LowPolyFiatUNO.obj");
    if (modelPtr == nullptr)
    {
        DEBUG_PRINT("Failed to load model.");
    }
    else
    {
        Model *model = reinterpret_cast<Model *>(modelPtr);
        DEBUG_PRINT("Model loaded successfully with %lld vertices and %lld indices.", model->vertices.size(), model->indices.size());
    }

    Model *modelPtr4 = g_AssetManager.loadAsset<Model *>(AssetType::MODEL, "assets/models/shopping-cart.obj");

    Model *model4 = reinterpret_cast<Model *>(modelPtr4);

    DEBUG_PRINT("Model loaded successfully with %lld vertices and %lld indices.", model4->vertices.size(), model4->indices.size());

    g_GameObjects.push_back(newGameObject);
    DEBUG_PRINT("Put componenent into Global Componenets Subsystem");

    // printf("%p\n", &g_GameObjects);

    // Possibly create more GameObjects with different positions or textures

    ThemeManager_ChangeTheme(2);
    DEBUG_PRINT("Changed Theme to default");

    while (!glfwWindowShouldClose(m_Window) && m_Running)
    {
        // Poll events
        glfwPollEvents();

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

        // Show main DockSpace
        ShowDockSpace();

        m_InspectorWindow->Show();


        if (1) {
            for (auto& Gameobject : g_GameObjects) {
                
                // Handle Componenets That require Updates

                std::shared_ptr<ScriptComponent> script = Gameobject->GetComponent<ScriptComponent>();
                if (script){ // Stupid Null Checks
                    script->Update(frame_delta);
                } 


            }
        }

        // Pass per-frame delta time to Lua

        m_RenderWindow->Show(); // The spinning triangle as ImGui::Image

        m_PerformanceWindow->Show(m_Fps, m_Ms); // FPS & ms

        m_LoggerWindow->Show(); // Logs

        m_SceneWindow->Show();

        // After rendering
        m_PerformanceWindow->UpdatePerformanceStats(-1, g_GPU_Triangles_drawn_to_screen);

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
