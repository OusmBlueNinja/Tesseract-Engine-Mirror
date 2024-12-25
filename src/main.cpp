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

#define DEBUG 1

#ifdef DEBUG
#define LOGPOINT(msg) std::cout << "[TESRCT] [" << __func__ << ":" << __LINE__ << "] " << (msg) << std::endl;
#else
#define LOGPOINT(msg)
#endif

// Logger Implementation
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


// Function Declarations
void ShowMainMenuBar();
void ShowViewport();
void ShowConsole(bool* p_open);

// Callback for GLFW errors
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Helper function to get color based on log level
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

    // Example log messages
    //Logger::GetInstance().Log(LogLevel::INFO, "Engine initialized successfully.");
    //Logger::GetInstance().Log(LogLevel::WARNING, "This is a warning message.");
    //Logger::GetInstance().Log(LogLevel::ERROR, "This is an error message.");
    Logger::GetInstance().Log(LogLevel::INFO, "Done!");

    // Variables for Console
    bool show_console = true;

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

// Function Definitions

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
            if (ImGui::MenuItem("Show Console", NULL, true)) { /* Toggle Console */ }
            if (ImGui::MenuItem("Toggle Viewport")) { /* Toggle Viewport */ }
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

    // Calculate the center position
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
