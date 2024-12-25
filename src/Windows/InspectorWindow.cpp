#include "InspectorWindow.h"
#include <cstdio>   // for debugging or printing if needed
#include <cstring>  // for strcpy, if needed

void InspectorWindow::Show(Transform& transform, Script& script)
{
    // Increase window/item spacing for a cleaner look
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(10, 10));

    if (ImGui::Begin("Inspector"))
    {
        // Title label (white text)
        ImGui::TextUnformatted("Selected Object Inspector");
        ImGui::Separator();
        ImGui::Spacing();

        // ===========================
        // 1) TRANSFORM
        // ===========================
        // Color the Transform header
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.4f, 1.0f)); 
        bool transformOpen = ImGui::CollapsingHeader("Transform##Main", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::PopStyleColor();

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
                static const ImVec4 colX      = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                static const ImVec4 colXHover = ImVec4(1.0f, 0.6f, 0.6f, 1.0f);
                static const ImVec4 colXActive= ImVec4(1.0f, 0.2f, 0.2f, 1.0f);

                static const ImVec4 colY      = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                static const ImVec4 colYHover = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
                static const ImVec4 colYActive= ImVec4(0.2f, 1.0f, 0.2f, 1.0f);

                static const ImVec4 colZ      = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                static const ImVec4 colZHover = ImVec4(0.6f, 0.6f, 1.0f, 1.0f);
                static const ImVec4 colZActive= ImVec4(0.2f, 0.2f, 1.0f, 1.0f);

                const char* axisNames[3] = { "X", "Y", "Z" };
                // We'll reference transform.position here
                float* pos = transform.position;

                ImGui::PushID("PositionRow");
                for (int i = 0; i < 3; i++)
                {
                    // Determine color set
                    ImVec4 col, colH, colA;
                    if (i == 0) { col = colX; colH = colXHover; colA = colXActive; }
                    else if (i == 1) { col = colY; colH = colYHover; colA = colYActive; }
                    else { col = colZ; colH = colZHover; colA = colZActive; }

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

                    if (i < 2) ImGui::SameLine(0, 15);
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
                const char* axisNames[3] = { "X", "Y", "Z" };
                float* rot = transform.rotation;

                // We can reuse the same color sets
                ImGui::PushID("RotationRow");
                for (int i = 0; i < 3; i++)
                {
                    // Decide color sets for X, Y, Z
                    ImVec4 col, colH, colA;
                    if (i == 0) {
                        col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        colH= ImVec4(1.0f, 0.6f, 0.6f, 1.0f);
                        colA= ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                    }
                    else if (i == 1) {
                        col = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                        colH= ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
                        colA= ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
                    }
                    else {
                        col = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                        colH= ImVec4(0.6f, 0.6f, 1.0f, 1.0f);
                        colA= ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
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

                    if (i < 2) ImGui::SameLine(0, 15);
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
                const char* axisNames[3] = { "X", "Y", "Z" };
                float* scl = transform.scale;

                ImGui::PushID("ScaleRow");
                for (int i = 0; i < 3; i++)
                {
                    // same color approach
                    ImVec4 col, colH, colA;
                    if (i == 0) {
                        col = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        colH= ImVec4(1.0f, 0.6f, 0.6f, 1.0f);
                        colA= ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                    }
                    else if (i == 1) {
                        col = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                        colH= ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
                        colA= ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
                    }
                    else {
                        col = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                        colH= ImVec4(0.6f, 0.6f, 1.0f, 1.0f);
                        colA= ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
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


                    if (i < 2) ImGui::SameLine(0, 15);
                }
                ImGui::PopID();
            }

            ImGui::Spacing();
            ImGui::Separator();
        }

        ImGui::Spacing();

        // ===========================
        // 2) SCRIPT
        // ===========================
        // We keep script text in white
        if (ImGui::CollapsingHeader("Script##Main", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("Attach a script or logic component here.");
                ImGui::EndTooltip();
            }

            ImGui::TextUnformatted("Script Name:");
            ImGui::SameLine();

            {
                char buffer[128];
                std::snprintf(buffer, sizeof(buffer), "%s", script.scriptName.c_str());
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputText("##ScriptName", buffer, sizeof(buffer)))
                {
                    script.scriptName = buffer;
                }
            }

            ImGui::Spacing();

            ImGui::TextUnformatted("Script Enabled:");
            ImGui::SameLine();
            ImGui::Checkbox("##ScriptEnabled", &script.enabled);

            ImGui::Spacing();
            ImGui::Separator();
        }
    }
    ImGui::End();

    // Restore style
    ImGui::PopStyleVar(3);
}