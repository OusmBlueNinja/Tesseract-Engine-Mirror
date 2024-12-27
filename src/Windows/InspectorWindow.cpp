#include "InspectorWindow.h"
#include <cstdio>  // for debugging or printing if needed
#include <cstring> // for strcpy, if needed

#include <glm/gtc/type_ptr.hpp> // Required for glm::value_ptr

#include <vector>

extern std::vector<GameObject> g_GameObjects;
extern GameObject *g_SelectedObject; // Pointer to the currently selected object

void InspectorWindow::Show()
{
    // Increase window/item spacing for a cleaner look
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

    if (ImGui::Begin("Inspector"))
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
            ImGui::Separator();
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

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // ===========================
            // 1) TRANSFORM
            // ===========================

            std::shared_ptr<TransformComponent> transform = g_SelectedObject->GetComponent<TransformComponent>();
            std::shared_ptr<MeshComponent> mesh = g_SelectedObject->GetComponent<MeshComponent>();

            // Color the Transform header

            if (transform && g_SelectedObject) //! Funny: I did not put a null check here and it broke everything.
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

            if (mesh && g_SelectedObject) //! Funny: I did not put a null check here and it broke everything.
            {

                // Transform* transform = &g_SelectedObject->transform;

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                bool meshOpen = ImGui::CollapsingHeader("Mesh##Main", ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::PopStyleColor();
                // printf("%p\n", &transform);
                if (meshOpen)
                {

                    int vao = static_cast<int>(mesh->vao);
                    if (ImGui::InputInt("vao", &vao, 1, 0))
                    {
                        mesh->vao = static_cast<GLuint>(vao);
                    }

                    int indexCount = static_cast<int>(mesh->indexCount);
                    if (ImGui::InputInt("indexCount", &indexCount, 1, 0))
                    {
                        mesh->indexCount = static_cast<GLuint>(indexCount);
                    }

                    int textureID = static_cast<int>(mesh->textureID);
                    if (ImGui::InputInt("textureID", &textureID, 1, 0))
                    {
                        mesh->textureID = static_cast<GLuint>(textureID);
                    }
                    //    Define a maximum buffer size
                    const size_t BUFFER_SIZE = 256;
                    // Allocate a buffer and initialize it with the current string
                    char buffer[BUFFER_SIZE];
                    strncpy(buffer, mesh->MeshPath.c_str(), BUFFER_SIZE - 1);
                    buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination
                    // Render the InputText widget
                    if (ImGui::InputText(mesh->MeshPath.c_str(), buffer, BUFFER_SIZE))
                    {
                        // Update the string if user made changes
                        mesh->MeshPath = buffer;
                    }
                    float availableWidth = ImGui::GetContentRegionAvail().x;

                    ImGui::Image(mesh->textureID, ImVec2(availableWidth, availableWidth), ImVec2(0, 0), ImVec2(1, 1));
                }
            }
            ImGui::Spacing();

            // ===========================
            // 2) SCRIPT
            // ===========================
            // We keep script text in white
            // if (ImGui::CollapsingHeader("Script##Main", ImGuiTreeNodeFlags_DefaultOpen))
            //{
            //     if (ImGui::IsItemHovered())
            //     {
            //         ImGui::BeginTooltip();
            //         ImGui::TextUnformatted("Attach a script or logic component here.");
            //         ImGui::EndTooltip();
            //     }

            //    ImGui::TextUnformatted("Script Name:");
            //    ImGui::SameLine();

            //    {
            //        char buffer[128];
            //        std::snprintf(buffer, sizeof(buffer), "%s", script.scriptName.c_str());
            //        ImGui::SetNextItemWidth(-1);
            //        if (ImGui::InputText("##ScriptName", buffer, sizeof(buffer)))
            //        {
            //            script.scriptName = buffer;
            //        }
            //    }

            //    ImGui::Spacing();

            //    ImGui::TextUnformatted("Script Enabled:");
            //    ImGui::SameLine();
            //    ImGui::Checkbox("##ScriptEnabled", &script.enabled);

            //    ImGui::Spacing();
            //    ImGui::Separator();
            //}
        }
        ImGui::End();

    } //

    // Restore style
    ImGui::PopStyleVar(3);
}
