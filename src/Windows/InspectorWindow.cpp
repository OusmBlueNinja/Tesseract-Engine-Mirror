#include "InspectorWindow.h"
#include <cstdio>  // for debugging or printing if needed
#include <cstring> // for strcpy, if needed

#include <glm/gtc/type_ptr.hpp> // Required for glm::value_ptr

#include <vector>

#include "Icons.h"

extern std::vector<GameObject> g_GameObjects;
extern GameObject *g_SelectedObject; // Pointer to the currently selected object
extern std::shared_ptr<CameraComponent> g_RuntimeCameraObject;

#include "Engine/AssetManager.h"
extern AssetManager *g_AssetManager;
extern LoggerWindow *g_LoggerWindow;

void InspectorWindow::Show()
{
    // Increase window/item spacing for a cleaner look
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

    if (ImGui::Begin(ICON_FA_MAGNIFYING_GLASS " Inspector##InspectorWindow"))
    {
        // Title label (white text)
        if (g_SelectedObject)
        {

            if (g_SelectedObject == nullptr)
            {
                ImGui::Text("No object selected.");
                return;
            }

            ImVec2 size = ImGui::GetContentRegionAvail();

            // Display object name and component count
            ImGui::Text("Editing Object: %s", g_SelectedObject->name.c_str());
            ImGui::Text("Components: %d", g_SelectedObject->GetComponentCount());

            ImGui::Spacing();

            // Begin two-column layout for labels and inputs
            ImGui::Columns(2, "InspectorColumns", true); // 2 columns, ID "InspectorColumns", border=true
            ImGui::SetColumnWidth(0, 100.0f);            // Optional: Set fixed width for the first column

            // Label in the first column
            ImGui::Text("Tag:");
            ImGui::NextColumn(); // Move to the second column

            // Define buffer size
            const size_t BUFFER_SIZE = 256;

            // Allocate buffer and copy the current string
            char buffer[BUFFER_SIZE];
            std::strncpy(buffer, g_SelectedObject->name.c_str(), BUFFER_SIZE - 1);
            buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

            // Unique identifier for the InputText to prevent ImGui state conflicts
            const char *inputLabel = "##TagInput";

            // Render InputText widget
            if (ImGui::InputText(inputLabel, buffer, BUFFER_SIZE))
            {
                // Update the GameObject's name if modified
                g_SelectedObject->name = buffer;
            }

            ImGui::NextColumn(); // Move back to the first column (if adding more fields)

            // End columns
            ImGui::Columns(1);

            ImGui::Separator();

            // ===========================
            // 2) ADD COMPONENT SECTION
            // ===========================

            ImGui::Text("Add Component:");
            ImGui::SameLine();

            // Define available components to add
            static int selectedComponent = 0;
            const char *componentOptions[] = {"Transform", "Mesh", "Script", "Camera"};
            const int componentCount = sizeof(componentOptions) / sizeof(componentOptions[0]);

            // Create a Combo Box for component selection

            ImGui::Combo(" ##ComponentCombo", &selectedComponent, componentOptions, componentCount);

            // Add Button to add the selected component
            if (ImGui::Button("Add"))
            {
                if (selectedComponent == 0) // TransformComponent
                {
                    // Check if TransformComponent already exists to prevent duplicates
                    std::shared_ptr<TransformComponent> existingTransform = g_SelectedObject->GetComponent<TransformComponent>();
                    if (!existingTransform)
                    {
                        g_SelectedObject->AddComponent(std::make_shared<TransformComponent>());
                        g_LoggerWindow->AddLog("TransformComponent added to %s.", g_SelectedObject->name.c_str());
                    }
                    else
                    {
                        g_LoggerWindow->AddLog("TransformComponent already exists on %s.", g_SelectedObject->name.c_str());
                    }
                }
                else if (selectedComponent == 1) // MeshComponent
                {
                    // Check if MeshComponent already exists to prevent duplicates
                    std::shared_ptr<MeshComponent> existingMesh = g_SelectedObject->GetComponent<MeshComponent>();
                    if (!existingMesh)
                    {
                        g_SelectedObject->AddComponent(std::make_shared<MeshComponent>());
                        g_LoggerWindow->AddLog("MeshComponent added to %s.", g_SelectedObject->name.c_str());
                    }
                    else
                    {
                        g_LoggerWindow->AddLog("MeshComponent already exists on %s.", g_SelectedObject->name.c_str());
                    }
                }
                else if (selectedComponent == 2) // ScriptComponent
                {
                    // Check if ScriptComponent already exists to prevent duplicates
                    std::shared_ptr<ScriptComponent> existingScript = g_SelectedObject->GetComponent<ScriptComponent>();
                    if (!existingScript)
                    {
                        g_SelectedObject->AddComponent(std::make_shared<ScriptComponent>());
                        g_LoggerWindow->AddLog("ScriptComponent added to %s.", g_SelectedObject->name.c_str());
                    }
                    else
                    {
                        g_LoggerWindow->AddLog("ScriptComponent already exists on %s.", g_SelectedObject->name.c_str());
                    }
                }
                else if (selectedComponent == 3) // CameraComponent
                {
                    // Check if CameraComponent already exists to prevent duplicates
                    std::shared_ptr<CameraComponent> existingCamera = g_SelectedObject->GetComponent<CameraComponent>();
                    if (!existingCamera)
                    {
                        g_SelectedObject->AddComponent(std::make_shared<CameraComponent>());
                        g_LoggerWindow->AddLog("CameraComponent added to %s.", g_SelectedObject->name.c_str());
                    }
                    else
                    {
                        g_LoggerWindow->AddLog("CameraComponent already exists on %s.", g_SelectedObject->name.c_str());
                    }
                }
                else
                {
                    // Handle unknown selections if necessary
                    g_LoggerWindow->AddLog("Unknown component selection.");
                }
            }

            ImGui::Separator();

            // ===========================
            // 1) TRANSFORM
            // ===========================

            std::shared_ptr<TransformComponent> transform = g_SelectedObject->GetComponent<TransformComponent>();
            std::shared_ptr<MeshComponent> mesh = g_SelectedObject->GetComponent<MeshComponent>();
            std::shared_ptr<ScriptComponent> script = g_SelectedObject->GetComponent<ScriptComponent>();
            std::shared_ptr<CameraComponent> camera = g_SelectedObject->GetComponent<CameraComponent>();

            // Color the Transform header

            if (transform && g_SelectedObject) // Funny: I did not put a null check here and it broke everything.
            {

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                bool transformOpen = ImGui::CollapsingHeader(ICON_FA_CIRCLE_NODES " Transform##Main", ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor();
                // Transform* transform = &g_SelectedObject->transform;
                // printf("%p\n", &transform);
                if (transformOpen)
                {
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted("Controls the object's Position, Rotation, and Scale.");
                        ImGui::EndTooltip();
                    }

                    const char *axisNames[3] = {"X", "Y", "Z"};
                    static const ImVec4 colX = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    static const ImVec4 colY = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                    static const ImVec4 colZ = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);

                    static const ImVec4 colXHover = ImVec4(1.0f, 0.6f, 0.6f, 1.0f);
                    static const ImVec4 colYHover = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
                    static const ImVec4 colZHover = ImVec4(0.6f, 0.6f, 1.0f, 1.0f);

                    static const ImVec4 colXActive = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                    static const ImVec4 colYActive = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
                    static const ImVec4 colZActive = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);

                    int ittr = 0;

                    auto drawTransformRow = [&](const char *label, float *values)
                    {
                        ImGui::TextUnformatted(label);
                        ImGui::SameLine(90); // Align labels to a fixed width

                        ImVec4 colors[3][3] = {
                            {colX, colXHover, colXActive},
                            {colY, colYHover, colYActive},
                            {colZ, colZHover, colZActive}};

                        for (int i = 0; i < 3; i++)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Button, colors[i][0]);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[i][1]);
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[i][2]);
                            std::string name = std::string(axisNames[i]) + "##" + std::to_string(ittr);
                            if (ImGui::Button(name.c_str(), ImVec2(20, 0)))
                            {
                                // No action on click
                            }
                            ImGui::PopStyleColor(3);

                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(60.0f); // Adjust field width to be more compact
                            ImGui::DragFloat((std::string("##") + label + axisNames[i]).c_str(), &values[i], 0.1f);

                            if (i < 2)
                                ImGui::SameLine(0, 5); // Reduce spacing between fields
                        }
                    };

                    // Position Row
                    drawTransformRow("Position", glm::value_ptr(transform->position));
                    ittr += 1;

                    // Rotation Row
                    drawTransformRow("Rotation", glm::value_ptr(transform->rotation));
                    ittr += 1;

                    // Scale Row
                    drawTransformRow("Scale", glm::value_ptr(transform->scale));
                }
            }

            if (camera)
            {
                // Styling: Set text color to white for the header
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

                // Create a collapsing header for the Camera component
                bool cameraOpen = ImGui::CollapsingHeader(ICON_FA_CAMERA "Camera##CamerInspector", ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor(); // Revert text color

                if (cameraOpen)
                {

                    // Edit Field of View (FOV)
                    if (ImGui::InputFloat("Field of View°", &camera->FOV, 1.0f, 10.0f, "%.1f"))
                    {
                        // Optionally, validate or clamp FOV values
                        if (camera->FOV < 1.0f)
                            camera->FOV = 1.0f;
                        if (camera->FOV > 179.0f)
                            camera->FOV = 179.0f;
                    }

                    // Edit Near Plane
                    if (ImGui::InputFloat("Near Plane", &camera->NearPlane, 0.1f, 1.0f, "%.2f"))
                    {
                        // Clamp Near Plane to a minimum value to prevent rendering issues
                        if (camera->NearPlane < 0.1f)
                            camera->NearPlane = 0.1f;

                        // Ensure Near Plane is always less than Far Plane
                        if (camera->NearPlane >= camera->FarPlane)
                            camera->NearPlane = camera->FarPlane - 0.1f;
                    }

                    // Edit Far Plane
                    if (ImGui::InputFloat("Far Plane", &camera->FarPlane, 10.0f, 100.0f, "%.2f"))
                    {
                        // Clamp Far Plane to a minimum value greater than Near Plane
                        if (camera->FarPlane <= camera->NearPlane)
                            camera->FarPlane = camera->NearPlane + 0.1f;
                    }

                    // Edit Aspect Ratio
                    if (ImGui::InputFloat("Aspect Ratio", &camera->AspectRatio, 0.01f, 0.1f, "%.2f"))
                    {
                        // Optionally, validate or clamp Aspect Ratio values
                        if (camera->AspectRatio <= 0.0f)
                            camera->AspectRatio = 1.77f; // Default to 16:9
                    }

                    // *** Begin Changed Code ***

                    // Replace the "Orthographic" Checkbox with a "Perspective" Switch

                    // Determine if the current camera is in perspective mode
                    bool isPerspective = camera->IsPerspective;

                    // Render the switch (styled checkbox to resemble a switch)
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
                    if (ImGui::Checkbox("Perspective", &isPerspective))
                    {
                        camera->IsPerspective = isPerspective;

                        if (isPerspective)
                        {
                            // Set to Perspective Projection
                            // Use existing camera properties or set to default values
                            camera->SetPerspective(camera->FOV, camera->AspectRatio, camera->NearPlane, camera->FarPlane);
                        }
                        else
                        {
                            // Set to Orthographic Projection
                            // Define orthographic boundaries based on current aspect ratio or set to default values
                            float orthoLeft = -camera->AspectRatio;
                            float orthoRight = camera->AspectRatio;
                            float orthoBottom = -1.0f;
                            float orthoTop = 1.0f;
                            camera->SetOrthographic(orthoLeft, orthoRight, orthoBottom, orthoTop, camera->NearPlane, camera->FarPlane);
                        }

                        // Update the global primary camera if needed
                        g_RuntimeCameraObject = camera;

                        // Log the projection mode change
                        std::string projectionMode = isPerspective ? "Perspective" : "Orthographic";
                        g_LoggerWindow->AddLog("Changed Projection Mode to %s for Camera: %s",
                                               ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                                               projectionMode.c_str(),
                                               g_SelectedObject->name.c_str());
                    }
                    ImGui::PopStyleVar();

                    // Optional: Add a tooltip for the Perspective switch
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("Toggle between Perspective and Orthographic projection modes.");
                        ImGui::EndTooltip();
                    }

                    // *** End Changed Code ***

                    // Add more camera properties here as needed

                    ImGui::Spacing();

                    // Replace the "Set as Primary" Button with a Checkbox

                    // Determine if the current camera is the primary camera
                    bool isPrimary = (g_RuntimeCameraObject == camera);

                    // Render the Checkbox
                    if (ImGui::Checkbox("Primary", &isPrimary))
                    {
                        if (isPrimary)
                        {
                            // Assign the current camera as the primary camera
                            g_RuntimeCameraObject = camera;
                            camera->DefaultRuntimeCamera = true;

                            // unset other cameras' DefaultRuntimeCamera flags
                            //! Not used because I don't want to
                            /*
                            for (auto& [name, cam] : allCameras)
                            {
                                if (cam != camera)
                                    cam->DefaultRuntimeCamera = false;
                            }
                            */

                            // Log the event
                            g_LoggerWindow->AddLog("Set Primary Camera: %s", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), g_SelectedObject->name.c_str());
                        }
                        else
                        {
                            // If unchecked and this camera was the primary, unset it
                            if (g_RuntimeCameraObject == camera)
                                g_RuntimeCameraObject.reset(); // Assuming SharedPtr has a reset method

                            camera->DefaultRuntimeCamera = false;

                            // Log the event
                            g_LoggerWindow->AddLog("Unset Primary Camera: %s", ImVec4(1.0f, 0.0f, 0.0f, 1.0f), g_SelectedObject->name.c_str());
                        }
                    }

                    //// Optional: Indicate if this camera is currently the primary camera
                    // if (g_RuntimeCameraObject == camera)
                    //{
                    //     ImGui::SameLine();
                    //     ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "(Primary)");
                    // }
                }
            }

            // Inside your RenderWindow.cpp or the relevant ImGui rendering function

            if (mesh && g_SelectedObject)
            {
                // Set text color to white for visibility
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

                // Create a collapsing header for the MeshComponent
                bool meshOpen = ImGui::CollapsingHeader(ICON_FA_CUBE " Mesh##Main", ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor();

                if (meshOpen)
                {
                    // --- Mesh Path (Editable) ---
                    const size_t BUFFER_SIZE = 256;
                    char buffer[BUFFER_SIZE];
                    strncpy(buffer, mesh->MeshPath.c_str(), BUFFER_SIZE - 1);
                    buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

                    if (ImGui::InputText("Mesh Path", buffer, BUFFER_SIZE))
                    {
                        mesh->MeshPath = buffer;
                        // Optionally, trigger reloading the mesh if the path changes
                        // Example:
                        std::shared_ptr<Model> model = g_AssetManager->loadAsset<Model>(AssetType::MODEL, mesh->MeshPath.c_str());
                    }

                    // --- Submeshes Information ---
                    ImGui::Indent();
                    if (ImGui::CollapsingHeader("Submeshes", ImGuiTreeNodeFlags_None))
                    {

                        // Check if the model is loaded
                        if (mesh)
                        {
                            // Iterate through each Submesh
                            ImGui::Indent();

                            for (size_t sm = 0; sm < mesh->submeshes.size(); ++sm)
                            {
                                const Submesh &submesh = mesh->submeshes[sm];
                                std::string header = "Submesh " + std::to_string(sm + 1) + "##Submesh" + std::to_string(sm);

                                // Create a collapsing header for each Submesh
                                if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_None))
                                {
                                    ImGui::Indent();
                                    size = ImGui::GetContentRegionAvail();


                                    // --- Submesh VAO (Read-Only) ---
                                    ImGui::Text("VAO: %d", static_cast<int>(submesh.vao));

                                    // --- Submesh Index Count (Read-Only) ---
                                    ImGui::Text("Index Count: %d", static_cast<int>(submesh.indices.size()));

                                    // --- Textures Associated with the Submesh ---
                                    ImGui::Separator();
                                    ImGui::Text("Textures:");

                                    // Iterate through each Texture in the Submesh
                                    for (size_t i = 0; i < submesh.textures.size(); ++i)
                                    {
                                        const Texture &texture = submesh.textures[i];
                                        std::string texHeader = "Texture " + std::to_string(i + 1) + "##Submesh" + std::to_string(sm) + "_Texture" + std::to_string(i);

                                        // Create a collapsing header for each Texture
                                        if (ImGui::CollapsingHeader(texHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                        {
                                            // --- Texture Type (Read-Only) ---
                                            ImGui::Text("Type: %s", texture.type.c_str());

                                            // --- Texture ID (Read-Only) ---
                                            ImGui::Text("Texture ID: %d", static_cast<int>(texture.id));

                                            // --- Texture Path (Read-Only) ---
                                            ImGui::Text("Path: %s", texture.path.c_str());

                                            // --- Texture Preview ---
                                            if (texture.id != 0)
                                            {
                                                // Adjust the size as needed
                                                ImGui::Image(static_cast<ImTextureID>(texture.id), ImVec2(size.x, size.x), ImVec2(0, 0), ImVec2(1, 1));
                                            }
                                            else
                                            {
                                                ImGui::Text("No texture bound.");
                                            }

                                            ImGui::Separator();
                                        }
                                    }
                                    ImGui::Unindent();

                                    ImGui::Separator();
                                }
                            }
                            ImGui::Unindent();
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No model loaded.");
                        }
                    }
                    ImGui::Unindent();
                }
            }

            if (script && g_SelectedObject)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                bool scriptOpen = ImGui::CollapsingHeader(ICON_FA_SCROLL " Script##Main", ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor();

                if (scriptOpen)
                {
                    // Define a maximum buffer size for script path
                    const size_t BUFFER_SIZE = 256;

                    // Allocate a buffer and initialize it with the current script path
                    char buffer[BUFFER_SIZE];
                    strncpy(buffer, script->ScriptPath.c_str(), BUFFER_SIZE - 1);
                    buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

                    // Render the InputText widget for editing the script path
                    if (ImGui::InputText("Script Path", buffer, BUFFER_SIZE))
                    {
                        script->ScriptPath = buffer; // Update the script path if modified
                    }

                    // Reload Script Button
                    if (ImGui::Button("Reload Script"))
                    {
                        if (script->Initialize())
                        {
                            script->Init();
                            g_LoggerWindow->AddLog("Reloaded Script: %s", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), script->ScriptPath.c_str());
                        }
                    }

                    // Assuming script->GetExposedVariables() returns std::unordered_map<std::string, LuaExposedVariant>
                    auto exposedVariables = script->GetExposedVariables();

                    for (const auto &[name, value] : exposedVariables)
                    {
                        ImGui::Text("%s:", name.c_str());

                        if (std::holds_alternative<int>(value))
                        {
                            int intValue = std::get<int>(value);
                            if (ImGui::InputInt(name.c_str(), &intValue))
                            {
                                script->UpdateVariable(name, intValue);
                            }
                        }
                        else if (std::holds_alternative<float>(value))
                        {
                            float floatValue = std::get<float>(value);
                            if (ImGui::InputFloat(name.c_str(), &floatValue))
                            {
                                script->UpdateVariable(name, floatValue);
                            }
                        }
                        else if (std::holds_alternative<std::string>(value))
                        {
                            const std::string &strValue = std::get<std::string>(value);
                            char buffer[256];
                            strncpy(buffer, strValue.c_str(), sizeof(buffer));
                            buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination
                            if (ImGui::InputText(name.c_str(), buffer, sizeof(buffer)))
                            {
                                script->UpdateVariable(name, std::string(buffer));
                            }
                        }
                        else if (std::holds_alternative<bool>(value))
                        {
                            bool boolValue = std::get<bool>(value);
                            if (ImGui::Checkbox(name.c_str(), &boolValue))
                            {
                                script->UpdateVariable(name, boolValue);
                            }
                        }
                        else
                        {
                            ImGui::TextDisabled("Unsupported Type");
                        }
                    }
                }
            }
        }
        ImGui::End();

    } //

    // Restore style
    ImGui::PopStyleVar(3);
}
