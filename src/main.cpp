// src/main.cpp

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../IconsFontAwesome6.h" // Ensure correct path

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

#include "stb_image.h" // Include stb_image.h

#include "ECS.h" // Include the ECS header
#include "Logger.h" // Include the Logger header
#include "TextureManager.h" // Include the TextureManager header
#include "UI.h" // Include the UI header
#include "Meshes.h" // Include the Meshes header
#include "ShaderComponent.h" // Include the ShaderComponent header
#include "CameraSystem.h" // Include the CameraSystem header

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// =====================
// Callback for GLFW errors
// =====================

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// =====================
// Global Variables for Camera Control
// =====================

bool firstMouse = true;
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;
bool mouseCaptured = true;

// =====================
// Forward Declarations
// =====================

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// =====================
// Camera Entity ID
// =====================

Entity cameraEntity = UINT32_MAX;

// =====================
// Main Function
// =====================

int main(int, char**)
{
    // Setup Logger
    Logger::GetInstance().Log(LogLevel::INFO, "Loading Engine...");

    // Setup GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
    if (!glfwInit())
    {
        Logger::GetInstance().Log(LogLevel::ERROR, "Failed to initialize GLFW.");
        return -1;
    }

    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
    #endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Tesseract Engine - 3D", NULL, NULL);
    if (window == NULL)
    {
        Logger::GetInstance().Log(LogLevel::ERROR, "Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Set viewport
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

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

    // Paths to your fonts
    const char* font_path = "./assets/fonts/Roboto-Medium.ttf"; // Replace with your default font
    const char* fa_font_path = "./assets/fonts/fa-solid-900.ttf"; // Replace with your Font Awesome font

    // Load default font
    ImFont* default_font = io.Fonts->AddFontFromFileTTF(font_path, 16.0f);
    if (!default_font)
    {
        Logger::GetInstance().Log(LogLevel::ERROR, "Failed to load default font!");
        return -1;
    }

    // Define the Font Awesome icon range
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    // Merge Font Awesome icons
    fontConfig.GlyphMinAdvanceX = 13.0f; // Adjust if necessary
    ImFont* fa_font = io.Fonts->AddFontFromFileTTF(fa_font_path, 16.0f, &fontConfig, icons_ranges);
    if (!fa_font)
    {
        Logger::GetInstance().Log(LogLevel::ERROR, "Failed to load Font Awesome font!");
        return -1;
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
    componentManager.RegisterComponent<MeshComponent>();
    componentManager.RegisterComponent<Shader>();
    componentManager.RegisterComponent<CameraComponent>();

    // Create a default entity with TransformComponent, MeshComponent, Shader, and CameraComponent
    Entity defaultEntity = entityManager.CreateEntity();
    componentManager.AddComponent<TransformComponent>(defaultEntity, TransformComponent());
    componentManager.AddComponent<MeshComponent>(defaultEntity, CreateCubeMesh());
    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    componentManager.AddComponent<Shader>(defaultEntity, shader);
    componentManager.AddComponent<CameraComponent>(defaultEntity, CameraComponent());

    Logger::GetInstance().Log(LogLevel::INFO, "Engine initialized successfully.");

    // Instantiate TextureManager
    TextureManager textureManager;

    // Load Textures if necessary
    // GLuint diffuseMap = textureManager.LoadTexture("path_to_diffuse_map.png");
    // GLuint specularMap = textureManager.LoadTexture("path_to_specular_map.png");

    // Setup Framebuffer Object (FBO) for rendering
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a color attachment texture
    GLuint texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    // Create a renderbuffer object for depth and stencil attachment
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_w, display_h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::GetInstance().Log(LogLevel::ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup Camera System
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame
    CameraSystem cameraSystem(window, deltaTime);

    // Setup Mouse Callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Variables for Console
    bool show_console = true;

    // Variable to track the selected entity
    Entity selectedEntity = defaultEntity;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Bind to framebuffer and draw scene as we normally would to color texture 
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D

        // Clear buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader
        Shader& activeShader = componentManager.GetComponent<Shader>(defaultEntity);
        activeShader.Use();

        // Retrieve camera view and projection matrices
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 cameraPos;
        for(auto entity : entityManager.GetActiveEntities()) {
            if(componentManager.HasComponent<CameraComponent>(entity)) {
                auto& camera = componentManager.GetComponent<CameraComponent>(entity);
                view = camera.GetViewMatrix();
                cameraPos = camera.Position;
            }
        }

        projection = glm::perspective(glm::radians(45.0f), 
                                      (float)display_w / (float)display_h, 
                                      0.1f, 100.0f);

        // Set uniforms
        activeShader.SetMat4("view", view);
        activeShader.SetMat4("projection", projection);
        activeShader.SetVec3("viewPos", cameraPos);

        // Set light properties
        activeShader.SetVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
        activeShader.SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        activeShader.SetVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        activeShader.SetVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // Set material properties
        activeShader.SetInt("material.diffuse", 0);
        activeShader.SetInt("material.specular", 1);
        activeShader.SetFloat("material.shininess", 32.0f);

        // Bind textures if any
        // For example:
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, specularMap);

        // Render all meshes
        for(auto entity : entityManager.GetActiveEntities()) {
            if(componentManager.HasComponent<MeshComponent>(entity) && componentManager.HasComponent<TransformComponent>(entity)) {
                auto& mesh = componentManager.GetComponent<MeshComponent>(entity);
                auto& transform = componentManager.GetComponent<TransformComponent>(entity);

                // Create model matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform.position);
                model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, transform.scale);

                activeShader.SetMat4("model", model);

                // Bind VAO and draw
                glBindVertexArray(mesh.VAO);
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Disable depth testing for UI rendering
        glDisable(GL_DEPTH_TEST);

        // Start ImGui frame
        ImGui::NewFrame();

        // Create DockSpace
        ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        const ImGuiViewport* viewport_main = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(ImVec2(viewport_main->Pos.x, viewport_main->Pos.y));
        ImGui::SetNextWindowSize(ImVec2(viewport_main->Size.x, viewport_main->Size.y));
        ImGui::SetNextWindowViewport(viewport_main->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("DockSpaceWindow", nullptr, dockspace_flags);
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
        ShowConsole(&show_console);
        ShowEntityTree(entityManager, componentManager, selectedEntity);
        ShowInspector(componentManager, selectedEntity);

        // Render the framebuffer texture in the Viewport panel
        ImGui::Begin("Viewport");
        ImGui::Text("3D Viewport");
        ImGui::Image((void*)(intptr_t)texColorBuffer, ImVec2(1280, 720), ImVec2(0,1), ImVec2(1,0));
        ImGui::End();

        // Rendering
        ImGui::Render();
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

        // Process camera input
        cameraSystem.ProcessInput(entityManager, componentManager);
    }

    // Cleanup framebuffer objects
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texColorBuffer);
    glDeleteRenderbuffers(1, &rbo);

    // Cleanup
    textureManager.Cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// =====================
// Mouse Movement Callback
// =====================

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cameraEntity == UINT32_MAX)
        return;

    auto& camera = EntityManager::GetInstance().GetComponent<CameraComponent>(cameraEntity); // Assuming GetInstance exists
    if (!cameraEntity) return;

    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // Reversed since y-coordinates go from bottom to top

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// =====================
// Scroll Callback
// =====================

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (cameraEntity == UINT32_MAX)
        return;

    auto& camera = EntityManager::GetInstance().GetComponent<CameraComponent>(cameraEntity); // Assuming GetInstance exists
    if (!cameraEntity) return;

    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
