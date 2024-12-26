#include "SceneWindow.h"
#include "imgui.h"
#include "glm/vec3.hpp"

// Include your asset manager and any other necessary headers
#include "Engine/AssetManager.h"
#include "TestModel.h"

// Globals
extern std::vector<GameObject> g_GameObjects;
extern GameObject* g_SelectedObject;


extern AssetManager g_AssetManager;

// Constructor

// Helper: Create a default cube GameObject
GameObject CreateDefaultCube() {
    GameObject cube;
    cube.name = "Cube";
    cube.transform.position = glm::vec3(0.f, 0.f, 0.f);
    cube.transform.rotation = glm::vec3(0.f, 0.5f, 0.f);
    cube.transform.scale = glm::vec3(1.f, 1.f, 1.f);
    cube.mesh.vao = CreateCubeVAO(); // Implement your VAO creation logic
    cube.mesh.indexCount = 36;
    cube.mesh.textureID = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(g_AssetManager.loadAsset(AssetType::TEXTURE, "assets/textures/wood.png")));
    return cube;
}

// Show function implementation
void SceneWindow::Show() {
    if (ImGui::Begin("Scene Window")) {
        // Add Button
        if (ImGui::Button("Add Object")) {
            AddGameObject();
        }

        ImGui::Separator();

        // Begin child region for the list to make it scrollable
        ImGui::BeginChild("GameObjectList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

        // List GameObjects
        for (int index = 0; index < static_cast<int>(g_GameObjects.size()); ++index) {
            GameObject& obj = g_GameObjects[index];
            bool isSelected = (g_SelectedObject == &obj);

            // Create a selectable item for each GameObject
            if (ImGui::Selectable(obj.name.c_str(), isSelected)) {
                g_SelectedObject = &obj;
            }

            // Right-click context menu to remove GameObject
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Remove")) {
                    RemoveGameObject(index);
                    ImGui::EndPopup();
                    break; // Exit the loop as the list has been modified
                }
                ImGui::EndPopup();
            }
        }

        ImGui::EndChild();

        ImGui::Separator();

        // Show currently selected object details at the bottom
        if (g_SelectedObject) {
            ImGui::Text("Selected Object: %s", g_SelectedObject->name.c_str());
            // Optionally add details or editable fields here
            // Example:
            // ImGui::DragFloat3("Position", &g_SelectedObject->transform.position.x, 0.1f);
            // ImGui::DragFloat3("Rotation", &g_SelectedObject->transform.rotation.x, 0.1f);
            // ImGui::DragFloat3("Scale", &g_SelectedObject->transform.scale.x, 0.1f);
        } else {
            ImGui::Text("No Object Selected");
        }
    }
    ImGui::End();
}

// AddGameObject: Adds a new GameObject
void SceneWindow::AddGameObject() {
    GameObject newObj = CreateDefaultCube();
    // Optionally, modify the name to ensure uniqueness
    newObj.name += " " + std::to_string(g_GameObjects.size() + 1);
    g_GameObjects.push_back(newObj);
}

// RemoveGameObject: Removes a GameObject by index
void SceneWindow::RemoveGameObject(int index) {
    if (index >= 0 && index < static_cast<int>(g_GameObjects.size())) {
        // If the object to be removed is selected, clear the selection
        if (g_SelectedObject == &g_GameObjects[index]) {
            g_SelectedObject = nullptr;
        }
        g_GameObjects.erase(g_GameObjects.begin() + index);
    }
}

// GetSelectedObject function implementation
GameObject* SceneWindow::GetSelectedObject() const {
    return g_SelectedObject;
}
