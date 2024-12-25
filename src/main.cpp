// src/main.cpp

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../IconsFontAwesome6.h" // Include the Font Awesome icons header

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

#include "ECS.h" // Include the ECS header

// =====================
// Logger Implementation
// =====================

enum class LogLevel { INFO, WARNING, ERROR };

struct LogEntry {
    LogLevel level;
    std::string message;
};

class Logger {
public:
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    void Log(LogLevel level, const std::string& message) {
        entries.emplace_back(LogEntry{ level, message });
        if (entries.size() > max_entries)
            entries.erase(entries.begin());
    }

    const std::vector<LogEntry>& GetEntries() const { return entries; }

    void Clear() { // Added Clear method
        entries.clear();
    }

private:
    Logger() : max_entries(1000) {}
    std::vector<LogEntry> entries;
    size_t max_entries;
};

#define DEBUG 1

#ifdef DEBUG
#define LOGPOINT(msg) std::cout << "[TESRCT] [" << __func__ << ":" << __LINE__ << "] " << (msg) << std::endl;
#else
#define LOGPOINT(msg)
#endif

// =====================
// ECS Instances
// =====================

EntityManager entityManager;
ComponentManager componentManager;

// =====================
// Function Declarations
// =====================

void ShowMainMenuBar();
void ShowViewport();
void ShowConsole(bool* p_open);
void ShowEntityTree(EntityManager& em, ComponentManager& cm, Entity& selectedEntity);
void ShowInspector(EntityManager& em, ComponentManager& cm, Entity selectedEntity);

// =====================
// Callback for GLFW errors
// =====================

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// =====================
// Helper function to get color based on log level
// =====================

ImVec4 GetColorForLogLevel(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
    case LogLevel::WARNING:
        return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    case LogLevel::ERROR:
        return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
    default:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

// =====================
// UI Function Definitions
// =====================

// 1. Main Menu Bar
void ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) { /* Handle New */ }
            if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Handle Open */ }
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Handle Save */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) { /* Handle Exit */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Handle Undo */ }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Handle Redo */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Copy", "Ctrl+C")) { /* Handle Copy */ }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) { /* Handle Paste */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            static bool show_console = true;
            if (ImGui::MenuItem("Show Console", NULL, &show_console)) { /* Toggle Console */ }
            // Add more view toggles as needed
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// 2. Viewport Panel
void ShowViewport()
{
    ImGui::Begin("Viewport", NULL, ImGuiWindowFlags_NoCollapse);

    // Get the size of the viewport
    ImVec2 viewport_size = ImGui::GetContentRegionAvail();

    // For demonstration, we'll render a colored rectangle
    // In a real engine, you'd render your scene here

    // Calculate the position relative to window
    ImVec2 pos = ImGui::GetCursorScreenPos();

    // Define rectangle dimensions
    ImVec2 rect_min = pos;
    ImVec2 rect_max = ImVec2(pos.x + viewport_size.x, pos.y + viewport_size.y);

    // Render a colored rectangle
    ImGui::GetWindowDrawList()->AddRectFilled(rect_min, rect_max, IM_COL32(100, 100, 200, 255));

    ImGui::End();
}

// 3. Console Panel
void ShowConsole(bool* p_open)
{
    ImGui::Begin("Console", p_open, ImGuiWindowFlags_NoCollapse);

    // Options menu
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Clear")) {
            Logger::GetInstance().Clear();
        }
        ImGui::EndPopup();
    }

    // Reserve enough left-over height for 1 separator and 1 input text
    ImGui::Separator();

    // Begin child region for scrolling
    ImGui::BeginChild("ConsoleChild", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    // Iterate through log entries
    for (const auto& entry : Logger::GetInstance().GetEntries())
    {
        ImVec4 color = GetColorForLogLevel(entry.level);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(entry.message.c_str());
        ImGui::PopStyleColor();
    }

    // Auto-scroll to the bottom
    ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::End();
}

// 4. Entity Tree
void ShowEntityTree(EntityManager& em, ComponentManager& cm, Entity& selectedEntity)
{
    ImGui::Begin("Entities");

    // Button to create a new entity
    if (ImGui::Button("Add Entity")) {
        try {
            Entity newEntity = em.CreateEntity();
            Logger::GetInstance().Log(LogLevel::INFO, "Created Entity " + std::to_string(newEntity));
        }
        catch (const std::exception& e) {
            Logger::GetInstance().Log(LogLevel::ERROR, e.what());
        }
    }

    ImGui::Separator();

    // Iterate through active entities
    const auto& entities = em.GetActiveEntities();
    for (auto entity : entities) {
        char label[32];
        sprintf(label, "Entity %d", entity);
        if (ImGui::Selectable(label, selectedEntity == entity)) {
            selectedEntity = entity;
        }
    }

    // Option to destroy the selected entity
    if (selectedEntity != UINT32_MAX) {
        ImGui::Separator();
        if (ImGui::Button("Delete Entity")) {
            em.DestroyEntity(selectedEntity, cm);
            Logger::GetInstance().Log(LogLevel::INFO, "Destroyed Entity " + std::to_string(selectedEntity));
            selectedEntity = UINT32_MAX;
        }
    }

    ImGui::End();
}

// 5. Inspector Panel
void ShowInspector(EntityManager& em, ComponentManager& cm, Entity selectedEntity)
{
    ImGui::Begin("Inspector");

    if (selectedEntity == UINT32_MAX) {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    char label[32];
    sprintf(label, "Entity %d", selectedEntity);
    ImGui::Text("%s", label);
    ImGui::Separator();

    // Display Transform Component
    if (cm.HasComponent<TransformComponent>(selectedEntity)) {
        if (ImGui::TreeNode("Transform")) {
            auto& transform = cm.GetComponent<TransformComponent>(selectedEntity);
            ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.1f);
            ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);

            if (ImGui::Button("Remove Transform")) {
                cm.RemoveComponent<TransformComponent>(selectedEntity);
                Logger::GetInstance().Log(LogLevel::INFO, "Removed TransformComponent from Entity " + std::to_string(selectedEntity));
                ImGui::TreePop();
                ImGui::End();
                return;
            }

            ImGui::TreePop();
        }
    }
    else {
        if (ImGui::Button("Add Transform")) {
            cm.AddComponent<TransformComponent>(selectedEntity, TransformComponent());
            Logger::GetInstance().Log(LogLevel::INFO, "Added TransformComponent to Entity " + std::to_string(selectedEntity));
        }
    }

    // Display Sprite Component
    if (cm.HasComponent<SpriteComponent>(selectedEntity)) {
        if (ImGui::TreeNode("Sprite")) {
            auto& sprite = cm.GetComponent<SpriteComponent>(selectedEntity);
            char buffer[256];
            strncpy(buffer, sprite.texturePath.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination
            if (ImGui::InputText("Texture Path", buffer, sizeof(buffer))) {
                sprite.texturePath = std::string(buffer);
            }

            if (ImGui::Button("Remove Sprite")) {
                cm.RemoveComponent<SpriteComponent>(selectedEntity);
                Logger::GetInstance().Log(LogLevel::INFO, "Removed SpriteComponent from Entity " + std::to_string(selectedEntity));
                ImGui::TreePop();
                ImGui::End();
                return;
            }

            ImGui::TreePop();
        }
    }
    else {
        if (ImGui::Button("Add Sprite")) {
            cm.AddComponent<SpriteComponent>(selectedEntity, SpriteComponent("path/to/texture.png"));
            Logger::GetInstance().Log(LogLevel::INFO, "Added SpriteComponent to Entity " + std::to_string(selectedEntity));
        }
    }

    ImGui::End();
}

// =====================
// Main Function
// =====================

int main(int, char**)
{
    LOGPOINT("Loading Engine");
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
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;

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
    config.GlyphMinAdvanceX = 13.0f; // Adjust if necessary
    ImFont* fa_font = io.Fonts->AddFontFromFileTTF(fa_font_path, 16.0f, &config, icons_ranges);

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
    entityManager.Init();
    componentManager.RegisterComponent<TransformComponent>();
    componentManager.RegisterComponent<SpriteComponent>();

    // Create a default entity with TransformComponent
    Entity defaultEntity = entityManager.CreateEntity();
    componentManager.AddComponent<TransformComponent>(defaultEntity, TransformComponent());

    Logger::GetInstance().Log(LogLevel::INFO, "Engine initialized successfully.");

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
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
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
        ShowViewport();
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
