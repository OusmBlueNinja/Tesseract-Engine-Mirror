#include "InspectorWindow.h"
#include <cstdio>  // for debugging or printing if needed
#include <cstring> // for strcpy, if needed

#include <glm/gtc/type_ptr.hpp> // Required for glm::value_ptr

#include <vector>

extern std::vector<GameObject> g_GameObjects;
extern GameObject *g_SelectedObject; // Pointer to the currently selected object
extern std::shared_ptr<CameraComponent> g_RuntimeCameraObject;

extern LoggerWindow *g_LoggerWindow;

void InspectorWindow::Show()
{
    // Increase window/item spacing for a cleaner look
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

    if (ImGui::Begin("Inspector##InspectorWindow"))
    {
        // Title label (white text)
        if (g_SelectedObject)
        {

            if (g_SelectedObject == nullptr)
            {
                ImGui::Text("No object selected.");
                return;
            }

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

            ImGui::Combo("##ComponentCombo", &selectedComponent, componentOptions, componentCount);

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
                bool transformOpen = ImGui::CollapsingHeader("Transform##Main", ImGuiTreeNodeFlags_DefaultOpen);
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

                    // -----------------------------------
                    // Position
                    // -----------------------------------
                    ImGui::TextUnformatted("Position");
                    ImGui::Spacing();

                    {
                        // We'll assign colors for X, Y, Z buttons
                        // (normal, hovered, active)
                        static const ImVec4 colX = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        static const ImVec4 colXHover = ImVec4(1.0f, 0.6f, 0.6f, 1.0f);
                        static const ImVec4 colXActive = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);

                        static const ImVec4 colY = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                        static const ImVec4 colYHover = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
                        static const ImVec4 colYActive = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);

                        static const ImVec4 colZ = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                        static const ImVec4 colZHover = ImVec4(0.6f, 0.6f, 1.0f, 1.0f);
                        static const ImVec4 colZActive = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);

                        const char *axisNames[3] = {"X", "Y", "Z"};
                        // We'll reference transform.position here
                        float *pos = glm::value_ptr(transform->position);

                        ImGui::PushID("PositionRow");
                        for (int i = 0; i < 3; i++)
                        {
                            // Determine color set
                            ImVec4 col, colH, colA;
                            if (i == 0)
                            {
                                col = colX;
                                colH = colXHover;
                                colA = colXActive;
                            }
                            else if (i == 1)
                            {
                                col = colY;
                                colH = colYHover;
                                colA = colYActive;
                            }
                            else
                            {
                                col = colZ;
                                colH = colZHover;
                                colA = colZActive;
                            }

                            // Push color style for button
                            ImGui::PushStyleColor(ImGuiCol_Button, col);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colH);
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, colA);

                            // Small button with the axis name
                            if (ImGui::Button(axisNames[i], ImVec2(20, 0)))
                            {
                                // No action on click, but we have a box with color
                            }

                            ImGui::PopStyleColor(3);

                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(60.0f);
                            ImGui::DragFloat((std::string("##Pos") + axisNames[i]).c_str(), &pos[i], 0.1f);

                            if (i < 2)
                                ImGui::SameLine(0, 15);
                        }
                        ImGui::PopID();
                    }

                    ImGui::Spacing();
                    ImGui::Separator();

                    // -----------------------------------
                    // Rotation
                    // -----------------------------------
                    ImGui::TextUnformatted("Rotation");
                    ImGui::Spacing();

                    {
                        // Same approach, but referencing transform.rotation
                        const char *axisNames[3] = {"X", "Y", "Z"};
                        float *rot = glm::value_ptr(transform->rotation);

                        // We can reuse the same color sets
                        ImGui::PushID("RotationRow");
                        for (int i = 0; i < 3; i++)
                        {
                            // Decide color sets for X, Y, Z
                            ImVec4 col, colH, colA;
                            if (i == 0)
                            {
                                col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                                colH = ImVec4(1.0f, 0.6f, 0.6f, 1.0f);
                                colA = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                            }
                            else if (i == 1)
                            {
                                col = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                                colH = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
                                colA = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
                            }
                            else
                            {
                                col = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                                colH = ImVec4(0.6f, 0.6f, 1.0f, 1.0f);
                                colA = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
                            }

                            ImGui::PushStyleColor(ImGuiCol_Button, col);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colH);
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, colA);

                            if (ImGui::Button(axisNames[i], ImVec2(20, 0)))
                            {
                                // No action
                            }
                            ImGui::PopStyleColor(3);

                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(60.0f);
                            ImGui::DragFloat((std::string("##Rot") + axisNames[i]).c_str(), &rot[i], 0.1f);

                            if (i < 2)
                                ImGui::SameLine(0, 15);
                        }
                        ImGui::PopID();
                    }

                    ImGui::Spacing();
                    ImGui::Separator();

                    // -----------------------------------
                    // Scale
                    // -----------------------------------
                    ImGui::TextUnformatted("Scale");
                    ImGui::Spacing();

                    {
                        const char *axisNames[3] = {"X", "Y", "Z"};
                        float *scl = glm::value_ptr(transform->scale);

                        ImGui::PushID("ScaleRow");
                        for (int i = 0; i < 3; i++)
                        {
                            // same color approach
                            ImVec4 col, colH, colA;
                            if (i == 0)
                            {
                                col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                                colH = ImVec4(1.0f, 0.6f, 0.6f, 1.0f);
                                colA = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                            }
                            else if (i == 1)
                            {
                                col = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                                colH = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
                                colA = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
                            }
                            else
                            {
                                col = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                                colH = ImVec4(0.6f, 0.6f, 1.0f, 1.0f);
                                colA = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
                            }

                            ImGui::PushStyleColor(ImGuiCol_Button, col);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colH);
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, colA);

                            if (ImGui::Button(axisNames[i], ImVec2(20, 0)))
                            {
                                // No action
                            }
                            ImGui::PopStyleColor(3);

                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(60.0f);
                            ImGui::DragFloat((std::string("##Scl") + axisNames[i]).c_str(), &scl[i], 0.1f);

                            if (i < 2)
                                ImGui::SameLine(0, 15);
                        }
                        ImGui::PopID();
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                }
            }

            if (camera)
            {
                // Styling: Set text color to white for the header
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

                // Create a collapsing header for the Camera component
                bool cameraOpen = ImGui::CollapsingHeader("Camera##CamerInspector", ImGuiTreeNodeFlags_DefaultOpen);
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
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                bool meshOpen = ImGui::CollapsingHeader("Mesh##Main", ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor();

                if (meshOpen)
                {
                    // --- VAO ---
                    int vao = static_cast<int>(mesh->vao);
                    if (ImGui::InputInt("VAO", &vao, 1, 0))
                    {
                        mesh->vao = static_cast<GLuint>(vao);
                    }

                    // --- Index Count ---
                    int indexCount = static_cast<int>(mesh->indexCount);
                    if (ImGui::InputInt("Index Count", &indexCount, 1, 0))
                    {
                        mesh->indexCount = static_cast<GLuint>(indexCount);
                    }

                    // --- Mesh Path ---
                    const size_t BUFFER_SIZE = 256;
                    char buffer[BUFFER_SIZE];
                    strncpy(buffer, mesh->MeshPath.c_str(), BUFFER_SIZE - 1);
                    buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

                    if (ImGui::InputText("Mesh Path", buffer, BUFFER_SIZE))
                    {
                        mesh->MeshPath = buffer;
                        // Optionally, trigger reloading the mesh if the path changes
                        // e.g., g_AssetManager.loadAsset<Model *>(AssetType::MODEL, mesh->MeshPath.c_str());
                    }

                    // --- Textures ---
                    ImGui::Separator();
                    ImGui::Text("Textures:");

                    // Display list of textures
                    float availableWidth = ImGui::GetContentRegionAvail().x;

                    for (size_t i = 0; i < mesh->textures.size(); ++i)
                    {
                        Texture &texture = mesh->textures[i];

                        // Create a unique identifier for each texture section
                        std::string header = "Texture " + std::to_string(i + 1) + "##" + std::to_string(i);


                        // Collapsing header for each texture
                        if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            // --- Texture Type ---
                            char typeBuffer[32];
                            strncpy(typeBuffer, texture.type.c_str(), sizeof(typeBuffer) - 1);
                            typeBuffer[sizeof(typeBuffer) - 1] = '\0';

                            if (ImGui::InputText(("Type##" + std::to_string(i)).c_str(), typeBuffer, sizeof(typeBuffer)))
                            {
                                texture.type = std::string(typeBuffer);
                                // Optionally, validate the type or restrict it to certain values
                            }

                            // --- Texture ID ---
                            int texID = static_cast<int>(texture.id);
                            if (ImGui::InputInt(("Texture ID##" + std::to_string(i)).c_str(), &texID, 1, 0))
                            {
                                texture.id = static_cast<GLuint>(texID);
                            }

                            // --- Texture Path ---
                            char pathBuffer[256];
                            strncpy(pathBuffer, texture.path.c_str(), sizeof(pathBuffer) - 1);
                            pathBuffer[sizeof(pathBuffer) - 1] = '\0';

                            if (ImGui::InputText(("Path##" + std::to_string(i)).c_str(), pathBuffer, sizeof(pathBuffer)))
                            {
                                texture.path = std::string(pathBuffer);
                                // Optionally, trigger reloading the texture if the path changes
                                // e.g., texture.id = g_AssetManager.loadTexture(texture.path, directory);
                            }

                            // --- Texture Preview ---
                            if (texture.id != 0)
                            {
                                // Retrieve the texture's width and height if needed
                                // For demonstration, using a fixed size
                                
                                ImGui::Image(static_cast<ImTextureID>(texture.id), ImVec2(availableWidth, availableWidth), ImVec2(0, 0), ImVec2(1, 1));
                            }
                            else
                            {
                                ImGui::Text("No texture bound.");
                            }

                            // --- Remove Texture Button ---
                            std::string removeButtonLabel = "Remove##" + std::to_string(i);
                            if (ImGui::Button(removeButtonLabel.c_str()))
                            {
                                mesh->textures.erase(mesh->textures.begin() + i);
                                // Adjust index after removal
                                ImGui::TreePop();
                                break; // Exit the loop as the current index is invalidated
                            }

                            ImGui::Separator();
                        }
                    }

                    // --- Add New Texture Button ---
                    // if (ImGui::Button("Add Texture"))
                    //{
                    //    // Define default values for the new texture
                    //    Texture newTexture;
                    //    newTexture.id = 0;                       // Assign a default or invalid texture ID
                    //    newTexture.type = "texture_diffuse";     // Default type
                    //    newTexture.path = "path/to/texture.png"; // Default path
                    //
                    //    mesh->textures.push_back(newTexture);
                    //}
                    //
                    // --- Display All Textures ---
                    // Optionally, display all textures in a separate section or after the list

                
                }
            }

            ImGui::Spacing();

            if (script && g_SelectedObject)
            {
                // Transform* transform = &g_SelectedObject->transform;

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                bool scriptOpen = ImGui::CollapsingHeader("Script##Main", ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor();

                // printf("%p\n", &transform);
                if (scriptOpen)
                {
                    //    Define a maximum buffer size
                    const size_t BUFFER_SIZE = 256;

                    // Allocate a buffer and initialize it with the current string
                    char buffer[BUFFER_SIZE];
                    strncpy(buffer, script->ScriptPath.c_str(), BUFFER_SIZE - 1);

                    buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

                    // Render the InputText widget
                    if (ImGui::InputText("Script Path", buffer, BUFFER_SIZE))
                    {

                        // Update the string if user made changes
                        script->ScriptPath = buffer;
                    }

                    if (ImGui::Button("Reload Script"))
                    {

                        if (script->Initialize())
                        {
                            script->Init();
                            g_LoggerWindow->AddLog("Reloaded Script: %s", ImVec4(0.0f, 1.0f, 0.0f, 1.0f), script->ScriptPath.c_str());
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
