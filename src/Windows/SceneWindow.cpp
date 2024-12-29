#include "SceneWindow.h"
#include "imgui.h"
#include "glm/vec3.hpp"

// Include your asset manager and any other necessary headers
#include "Engine/AssetManager.h"
#include "TestModel.h"
#include "gcml.h"

#include <iostream>

// Globals
extern std::vector<std::shared_ptr<GameObject>> g_GameObjects;
extern std::shared_ptr<GameObject> g_SelectedObject;

extern AssetManager g_AssetManager;

// Helper: Create a default cube GameObject
std::shared_ptr<GameObject> CreateDefaultCube()
{
    // Pseudocode:
    int newId = g_GameObjects.size();
    auto newGameObject = std::make_shared<GameObject>(newId, ("New GameObject"));
    newGameObject->AddComponent(std::make_shared<TransformComponent>()); // Ensure each entity has a TransformComponent by default
    newGameObject->AddComponent(std::make_shared<MeshComponent>());      // Ensure each entity has a TransformComponent by default

    // Suppose we loaded a VAO, an EBO with 36 indices for the cube,
    // and a texture ID from the asset manager
    std::shared_ptr<MeshComponent> mesh = newGameObject->GetComponent<MeshComponent>();

    mesh->vao = CreateCubeVAO();
    mesh->indexCount = 36;
    mesh->textureID = g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/wood.png");

    return newGameObject;
}

void SceneWindow::Show()
{
    if (ImGui::Begin("Scene Window##SceneWindow"))
    {
        // Add Button
        if (ImGui::Button("Add Object"))
        {
            AddGameObject();
            std::cout << "Added a new GameObject. Total objects: " << g_GameObjects.size() << std::endl;
        }

        ImGui::Separator();

        // Begin child region for the list to make it scrollable
        ImGui::BeginChild("GameObjectList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

        // Initialize an external index to keep track of each GameObject's position
        size_t index = 0;

        // Iterate through GameObjects using a range-based for loop
        for (auto &obj : g_GameObjects)
        {
            // Determine if the current GameObject is selected
            bool isSelected = (g_SelectedObject == obj);

            // Create a unique label for each selectable item using the index
            // This ensures ImGui uniquely identifies each item
            std::string label = obj->name + "##" + std::to_string(index);

            // Render the GameObject as a selectable item in the list
            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                // Update the selected GameObject when clicked
                g_SelectedObject = obj;
            }

            // Handle right-click context menu for the current item
            if (ImGui::BeginPopupContextItem())
            {
                // Option to remove the GameObject
                if (ImGui::MenuItem("Remove"))
                {
                    // Remove the GameObject by its index
                    RemoveGameObject(static_cast<int>(index));

                    // End the popup before breaking out of the loop
                    ImGui::EndPopup();

                    // Since we've modified the container, exit the loop to prevent issues
                    break;
                }

                // End the context menu popup
                ImGui::EndPopup();
            }

            // Increment the index for the next GameObject
            ++index;
        }

        // Optional: Display a message if there are no GameObjects
        if (g_GameObjects.empty())
        {
            ImGui::Text("No Game Objects available.");
        }

        // End the ImGui window or group

        ImGui::EndChild();

        ImGui::Separator();

        // Show currently selected object details at the bottom
        if (g_SelectedObject)
        {
            ImGui::Text("Selected Object: %s", g_SelectedObject->name.c_str());
            // Optionally, display more details or provide editing capabilities
        }
        else
        {
            ImGui::Text("No Object Selected");
        }
    }
    ImGui::End();
}

// AddGameObject: Adds a new GameObject
void SceneWindow::AddGameObject()
{
    std::shared_ptr<GameObject> newObj = CreateDefaultCube();
    // Modify the name to ensure uniqueness
    newObj->name += " " + std::to_string(g_GameObjects.size());
    g_GameObjects.push_back(newObj);
}

// RemoveGameObject: Removes a GameObject by index
void SceneWindow::RemoveGameObject(int index)
{
    if (index >= 0 && index < static_cast<int>(g_GameObjects.size()))
    {
        // If the object to be removed is selected, clear the selection
        if (g_SelectedObject == g_GameObjects[index])
        {
            g_SelectedObject = nullptr;
        }

        g_GameObjects.erase(g_GameObjects.begin() + index);
    }
    else
    {
        DEBUG_PRINT("Attempted to remove GameObject with invalid index: %d", index);
    }
}

// GetSelectedObject function implementation
std::shared_ptr<GameObject> SceneWindow::GetSelectedObject() const
{
    return g_SelectedObject;
}
