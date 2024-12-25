// src/UI.cpp
#include "UI.h"
#include "Logger.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
void ShowViewport(TextureManager& textureManager, EntityManager& em, ComponentManager& cm)
{
    ImGui::Begin("Viewport", NULL, ImGuiWindowFlags_NoCollapse);

    // Get the size of the viewport
    ImVec2 viewport_size = ImGui::GetContentRegionAvail();

    // Set viewport background to black
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 rect_min = pos;
    ImVec2 rect_max = ImVec2(pos.x + viewport_size.x, pos.y + viewport_size.y);
    draw_list->AddRectFilled(rect_min, rect_max, IM_COL32(0, 0, 0, 255)); // Black background

    // Iterate through active entities
    const auto& entities = em.GetActiveEntities();
    for (auto entity : entities) {
        if (cm.HasComponent<SpriteComponent>(entity) && cm.HasComponent<TransformComponent>(entity)) {
            auto& sprite = cm.GetComponent<SpriteComponent>(entity);
            auto& transform = cm.GetComponent<TransformComponent>(entity);

            // Get texture ID
            GLuint textureID = textureManager.GetTexture(sprite.texturePath);
            if (textureID == 0) {
                continue; // Failed to load texture
            }

            // Convert GLuint to ImTextureID
            ImTextureID imgui_tex_id = (ImTextureID)(intptr_t)textureID;

            // Define size based on scale
            ImVec2 size(transform.scale.x, transform.scale.y);

            // Define position based on position
            ImVec2 img_pos = ImVec2(pos.x + transform.position.x, pos.y + transform.position.y);
            ImVec2 img_size = ImVec2(img_pos.x + size.x, img_pos.y + size.y);

            // Render the image
            draw_list->AddImage(imgui_tex_id, img_pos, img_size);
        }
    }

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
            static char buffer[256];
            strncpy(buffer, sprite.texturePath.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination
            if (ImGui::InputText("Texture Path", buffer, sizeof(buffer))) {
                std::string newPath(buffer);
                if (newPath != sprite.texturePath) {
                    sprite.texturePath = newPath;
                    Logger::GetInstance().Log(LogLevel::INFO, "Updated SpriteComponent texture path to " + newPath + " for Entity " + std::to_string(selectedEntity));
                }
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
            // Assign a default path; ensure the texture exists
            std::string defaultPath = "assets/textures/default.png";
            cm.AddComponent<SpriteComponent>(selectedEntity, SpriteComponent(defaultPath));
            Logger::GetInstance().Log(LogLevel::INFO, "Added SpriteComponent to Entity " + std::to_string(selectedEntity) + " with texture path: " + defaultPath);
        }
    }

    ImGui::End();
}
