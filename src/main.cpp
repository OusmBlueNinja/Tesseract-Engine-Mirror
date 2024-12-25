// src/main.cpp

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../IconsFontAwesome6.h"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Include stb_image.h

#include "ECS.h" // Include the ECS header
#include "Logger.h" // Include the Logger header
#include "TextureManager.h" // Include the TextureManager header
#include "UI.h" // Include the UI header

// =====================
// Callback for GLFW errors
// =====================

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// =====================
// Main Function
// =====================

int main(int, char**)
{
    // Setup Logger
    //LOGPOINT("Loading Engine");
    Logger::GetInstance().Log(LogLevel::INFO, "Loading engine...");

    // Setup GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Tesseract Engine", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport (optional)

    // Load Fonts
    ImFontConfig fontConfig;
    fontConfig.MergeMode = true;
    fontConfig.PixelSnapH = true;

    // Path to your fonts
    const char* font_path = "./assets/fonts/Roboto-Medium.ttf"; // Replace with your default font
    const char* fa_font_path = "./assets/fonts/fa-solid-900.ttf"; // Replace with your Font Awesome font

    // Load default font
    ImFont* default_font = io.Fonts->AddFontFromFileTTF(font_path, 16.0f);
    if (!default_font)
    {
        fprintf(stderr, "Failed to load default font!\n");
        return 1;
    }

    // Define the Font Awesome icon range
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    // Merge Font Awesome icons
    fontConfig.GlyphMinAdvanceX = 13.0f; // Adjust if necessary
    ImFont* fa_font = io.Fonts->AddFontFromFileTTF(fa_font_path, 16.0f, &fontConfig, icons_ranges);

    if (!fa_font)
    {
        fprintf(stderr, "Failed to load Font Awesome font!\n");
        return 1;
    }

    // Build the font atlas
    io.Fonts->Build();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic(); // Alternative theme

    // When viewports are enabled, tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    /*
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    */

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initialize ECS
    EntityManager entityManager;
    ComponentManager componentManager;
    entityManager.Init();
    componentManager.RegisterComponent<TransformComponent>();
    componentManager.RegisterComponent<SpriteComponent>();

    // Create a default entity with TransformComponent (without SpriteComponent)
    Entity defaultEntity = entityManager.CreateEntity();
    componentManager.AddComponent<TransformComponent>(defaultEntity, TransformComponent());

    Logger::GetInstance().Log(LogLevel::INFO, "Engine initialized successfully.");

    // Instantiate TextureManager
    TextureManager textureManager;

    // Variables for Console
    bool show_console = true;

    // Variable to track the selected entity
    Entity selectedEntity = defaultEntity;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create DockSpace
        ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Adjust the docking space position by 20 pixels to avoid overlapping with the main menu bar
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 20));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - 20));
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("DockSpace", nullptr, dockspace_flags);
        ImGui::PopStyleVar(2);

        // DockSpace
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        }

        ImGui::End();

        // Show GUI windows
        ShowMainMenuBar();
        ShowViewport(textureManager, entityManager, componentManager); // Pass references
        ShowConsole(&show_console);

        // Show ECS UI panels
        ShowEntityTree(entityManager, componentManager, selectedEntity);
        ShowInspector(entityManager, componentManager, selectedEntity);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark background
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows (for multi-viewport)
        /*
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        */

        glfwSwapBuffers(window);
    }

    // Cleanup
    textureManager.Cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
