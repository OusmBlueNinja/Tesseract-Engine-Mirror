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
    mesh->textureID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(g_AssetManager.loadAsset(AssetType::TEXTURE, "assets/textures/wood.png")));

    return newGameObject;
}

void SceneWindow::Show()
{
    if (ImGui::Begin("Scene Window"))
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

        // Define TreeNode flags for better visuals and interaction
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        // Iterate through GameObjects using index for unique identification
        for (size_t i = 0; i < g_GameObjects.size(); ++i)
        {
            auto &obj = g_GameObjects[i];

            // Determine flags based on selection
            ImGuiTreeNodeFlags flags = nodeFlags;
            if (g_SelectedObject == obj)
                flags |= ImGuiTreeNodeFlags_Selected;

            // Unique identifier for each GameObject node using pointer to ensure uniqueness
            // Alternatively, you can use the object's ID or address
            std::string nodeLabel = obj->name;
            bool nodeOpen = ImGui::TreeNodeEx((void *)(intptr_t)i, flags, nodeLabel.c_str());

            // Handle selection
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                g_SelectedObject = obj;
                std::cout << "Selected GameObject: " << obj->name << " with ID: " << obj->id << std::endl;
            }

            // Right-click context menu for GameObject actions
            if (ImGui::BeginPopupContextItem())
            {
                // Delete GameObject Option
                if (ImGui::MenuItem("Remove"))
                {
                    std::cout << "Attempting to remove GameObject: " << obj->name << " with ID: " << obj->id << std::endl;

                    RemoveGameObject(static_cast<int>(i));
                    std::cout << "Removed GameObject: " << obj->name << std::endl;

                    ImGui::EndPopup();
                    // Since we've erased the current entity, adjust the loop accordingly
                    // Decrement i to account for the removed element
                    --i;
                    continue; // Skip the rest of the loop iteration
                }

                ImGui::EndPopup();
            }

            // Optionally, implement double-click to rename or perform other actions

            // Close the tree node
            if (nodeOpen)
            {
                // If you decide to add child nodes in the future, handle them here
                // Currently, no additional handling is required

                ImGui::TreePop();
            }
        }

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
    std::cout << "Added GameObject: " << newObj->name << " with ID: " << newObj->id << std::endl;
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
            std::cout << "Cleared selection as the selected GameObject was removed." << std::endl;
        }

        std::cout << "Removing GameObject: " << g_GameObjects[index]->name << " with ID: " << g_GameObjects[index]->id << std::endl;
        g_GameObjects.erase(g_GameObjects.begin() + index);
    }
    else
    {
        std::cerr << "Attempted to remove GameObject with invalid index: " << index << std::endl;
    }
}

// GetSelectedObject function implementation
std::shared_ptr<GameObject> SceneWindow::GetSelectedObject() const
{
    return g_SelectedObject;
}
