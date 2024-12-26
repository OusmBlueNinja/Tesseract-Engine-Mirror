#include "SceneWindow.h"
#include "imgui.h"
#include "glm/vec3.hpp"

#include "Engine/AssetManager.h"

#include "TestModel.h"

// Globals
extern std::vector<GameObject> g_GameObjects;
extern GameObject* g_SelectedObject;

extern AssetManager g_AssetManager;



// Helper: Create a default cube GameObject
GameObject CreateDefaultCube() {
    GameObject cube;
    cube.name = "Cube";
    cube.transform.position = glm::vec3(0.f, 0.f, 0.f);
    cube.transform.rotation = glm::vec3(0.f, 0.5f, 0.f);
    cube.transform.scale = glm::vec3(1.f, 1.f, 1.f);
    cube.mesh.vao = CreateCubeVAO(); // Implement your VAO creation logic
    cube.mesh.indexCount = 36;
    cube.mesh.textureID = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(
        g_AssetManager.loadAsset(AssetType::TEXTURE, "assets/textures/wood.png")));
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

        // List GameObjects
        int index = 0;
        for (auto it = g_GameObjects.begin(); it != g_GameObjects.end(); ++it, ++index) {
            GameObject& obj = *it;
            std::string uniqueID = obj.name + "##" + std::to_string(index);

            if (ImGui::TreeNode(uniqueID.c_str())) {
                // Select GameObject
                if (ImGui::Selectable("Select", g_SelectedObject == &obj)) {
                    g_SelectedObject = &obj;
                }

                // Right-click context menu to remove GameObject
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Remove")) {
                        RemoveGameObject(index);
                        ImGui::EndPopup();
                        break;
                    }
                    ImGui::EndPopup();
                }

                ImGui::TreePop();
            }
        }

        ImGui::Separator();

        // Show currently selected object details at the bottom
        if (g_SelectedObject) {
            ImGui::Text("Selected Object: %s", g_SelectedObject->name.c_str());
            // Optionally add details or editable fields here
        } else {
            ImGui::Text("No Object Selected");
        }
    }
    ImGui::End();
}

// AddGameObject: Adds a new GameObject
void SceneWindow::AddGameObject() {
    g_GameObjects.push_back(CreateDefaultCube());
}

// RemoveGameObject: Removes a GameObject by index
void SceneWindow::RemoveGameObject(int index) {
    if (index >= 0 && index < static_cast<int>(g_GameObjects.size())) {
        g_GameObjects.erase(g_GameObjects.begin() + index);
        g_SelectedObject = nullptr; // Clear selection if the removed object was selected
    }
}

// GetSelectedObject function implementation
GameObject* SceneWindow::GetSelectedObject() const {
    return g_SelectedObject;
}
