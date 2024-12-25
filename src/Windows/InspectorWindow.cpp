#include "InspectorWindow.h"
#include <cstdio>  // for printf, if needed
#include <cstring> // for strcpy, if needed

void InspectorWindow::Show(Transform &transform, Script &script)
{
    // We can push additional style for a more Unity/Godot-like inspector
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

    // Optional: a slight color for window background
    // (If you want to override the theme’s default)
    // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.12f, 1.f));

    if (ImGui::Begin("Inspector"))
    {
        // Title or header-like text
        {
            // A mild accent color for the header text
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.85f, 0.2f, 1.f));
            ImGui::TextUnformatted("Selected Object Inspector");
            ImGui::PopStyleColor();
        }

        ImGui::Separator();
        ImGui::Spacing();

        // ----------------
        // TRANSFORM
        // ----------------
        {
            // A bit of color or bold for the header
            // Inside your InspectorWindow::Show(...) method,
            // specifically within the Transform section:

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Provide a quick tooltip/hint on hover
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted("Controls the object's Position, Rotation, and Scale.");
                    ImGui::EndTooltip();
                }

                // ------------------------------------------
                // Position
                // ------------------------------------------
                if (ImGui::TreeNodeEx("Position", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
                {
                    // We'll do a custom layout with color-coded X, Y, Z labels
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Position:");
                    ImGui::Spacing();

                    // Colors for each axis: X=red, Y=green, Z=blue
                    ImVec4 colX(1.0f, 0.4f, 0.4f, 1.0f);
                    ImVec4 colY(0.4f, 1.0f, 0.4f, 1.0f);
                    ImVec4 colZ(0.4f, 0.4f, 1.0f, 1.0f);

                    // Axis labels for reference
                    const char *axisLabels[3] = {"X", "Y", "Z"};
                    ImVec4 axisColors[3] = {colX, colY, colZ};
                    float *pos = transform.position;

                    // We'll lay them out on one line with spacing
                    ImGui::PushID("PositionFields");
                    for (int i = 0; i < 3; i++)
                    {
                        // Color-coded label
                        ImGui::TextColored(axisColors[i], "%s", axisLabels[i]);
                        ImGui::SameLine();

                        // We push another ID so each DragFloat is unique
                        ImGui::PushID(i);
                        ImGui::SetNextItemWidth(60.0f); // or -1 for full stretch
                        ImGui::DragFloat("##Pos", &pos[i], 0.1f);
                        ImGui::PopID();

                        // Small spacing between each axis
                        if (i < 2)
                        {
                            ImGui::SameLine(0, 15);
                        }
                    }
                    ImGui::PopID();

                    ImGui::TreePop();
                }
                ImGui::Spacing();

                // ------------------------------------------
                // Rotation
                // ------------------------------------------
                if (ImGui::TreeNodeEx("Rotation", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Rotation:");
                    ImGui::Spacing();

                    ImVec4 colX(1.0f, 0.4f, 0.4f, 1.0f);
                    ImVec4 colY(0.4f, 1.0f, 0.4f, 1.0f);
                    ImVec4 colZ(0.4f, 0.4f, 1.0f, 1.0f);

                    const char *axisLabels[3] = {"X", "Y", "Z"};
                    ImVec4 axisColors[3] = {colX, colY, colZ};
                    float *rot = transform.rotation;

                    ImGui::PushID("RotationFields");
                    for (int i = 0; i < 3; i++)
                    {
                        ImGui::TextColored(axisColors[i], "%s", axisLabels[i]);
                        ImGui::SameLine();

                        ImGui::PushID(i);
                        ImGui::SetNextItemWidth(60.0f);
                        ImGui::DragFloat("##Rot", &rot[i], 0.1f);
                        ImGui::PopID();

                        if (i < 2)
                        {
                            ImGui::SameLine(0, 15);
                        }
                    }
                    ImGui::PopID();

                    ImGui::TreePop();
                }
                ImGui::Spacing();

                // ------------------------------------------
                // Scale
                // ------------------------------------------
                if (ImGui::TreeNodeEx("Scale", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Scale:");
                    ImGui::Spacing();

                    ImVec4 colX(1.0f, 0.4f, 0.4f, 1.0f);
                    ImVec4 colY(0.4f, 1.0f, 0.4f, 1.0f);
                    ImVec4 colZ(0.4f, 0.4f, 1.0f, 1.0f);

                    const char *axisLabels[3] = {"X", "Y", "Z"};
                    ImVec4 axisColors[3] = {colX, colY, colZ};
                    float *scl = transform.scale;

                    ImGui::PushID("ScaleFields");
                    for (int i = 0; i < 3; i++)
                    {
                        ImGui::TextColored(axisColors[i], "%s", axisLabels[i]);
                        ImGui::SameLine();

                        ImGui::PushID(i);
                        ImGui::SetNextItemWidth(60.0f);
                        ImGui::DragFloat("##Scale", &scl[i], 0.1f);
                        ImGui::PopID();

                        if (i < 2)
                        {
                            ImGui::SameLine(0, 15);
                        }
                    }
                    ImGui::PopID();

                    ImGui::TreePop();
                }

                ImGui::Spacing();
                ImGui::Separator();
            }

            ImGui::Spacing();
        }

        // ----------------
        // SCRIPT
        // ----------------
        {
            if (ImGui::CollapsingHeader("Script##Inspector", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted("Attach a script (logic) to this object.");
                    ImGui::EndTooltip();
                }

                // Script Name
                ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Script Name:");
                ImGui::SameLine();
                {
                    // We'll allocate a buffer to edit the script name
                    char buffer[128];
                    // Copy the current name into the buffer
                    std::snprintf(buffer, sizeof(buffer), "%s", script.scriptName.c_str());

                    // Provide an input text
                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::InputText("##ScriptName", buffer, sizeof(buffer)))
                    {
                        script.scriptName = buffer;
                    }
                }

                // Enabled?
                ImGui::Checkbox("Enabled##ScriptEnabled", &script.enabled);

                // In a real engine, you might display all public fields of the script here
                // e.g., float moveSpeed, int health, etc.

                ImGui::Spacing();
                ImGui::Separator();
            }
        }
    }
    ImGui::End();

    // Pop the potential override color if used
    // ImGui::PopStyleColor();

    // Restore style
    ImGui::PopStyleVar(3);
}
