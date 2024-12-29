#include <vector>
#include "LuaEditorWindow.h"
#include <fstream>
#include <sstream>

#include <unordered_set>

// Lua keywords for highlighting
const std::unordered_set<std::string> luaKeywords = {
    "and", "break", "do", "else", "elseif", "end", "false", "for", "function",
    "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true",
    "until", "while"
};

bool IsLuaKeyword(const std::string& word) {
    return luaKeywords.find(word) != luaKeywords.end();
}


LuaEditorWindow::LuaEditorWindow() {
    m_FilePath = "assets/scripts/script.lua";
    m_EditorContent.resize(BUFFER_SIZE, '\0'); // Initialize buffer
    std::string initialText = "-- Lua Text Editor Example\nprint('Hello, World!')";
    std::copy(initialText.begin(), initialText.end(), m_EditorContent.begin());
}







void LuaEditorWindow::Show() {
    if (m_EditorContent.empty()) {
        ImGui::Text("Editor content not initialized.");
        return;
    }

    ImGui::Begin("Lua Text Editor##LuaEditor");

    // Toolbar buttons
    if (ImGui::Button("Save")) {
        if (!m_FilePath.empty()) {
            SaveToFile(m_FilePath);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        if (!m_FilePath.empty()) {
            LoadFromFile(m_FilePath);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        Clear();
    }

    ImGui::Separator();

    // Editable text input
    float contentHeight = ImGui::GetContentRegionAvail().y;
    ImVec2 inputBoxSize(-FLT_MIN, contentHeight);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Reduce spacing for overlay
    ImGui::BeginChild("##editor_background", inputBoxSize, true, ImGuiWindowFlags_NoScrollbar);

    // Syntax highlighting overlay
    ImGui::SetCursorPos(ImVec2(0, 0));
    std::istringstream stream(GetText());
    std::string line;

    while (std::getline(stream, line)) {
        const char* cursor = line.c_str();
        const char* end = cursor + line.size();
        std::string inlineText;
        while (cursor < end) {
            // Handle comments
            if (*cursor == '-' && (cursor + 1 < end) && *(cursor + 1) == '-') {
                if (!inlineText.empty()) {
                    ImGui::TextUnformatted(inlineText.c_str());
                    ImGui::SameLine(0.0f, 0.0f);
                    inlineText.clear();
                }
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray for comments
                ImGui::TextUnformatted(cursor);
                ImGui::PopStyleColor();
                break;
            }

            // Handle strings
            if (*cursor == '"' || *cursor == '\'') {
                char stringDelimiter = *cursor;
                if (!inlineText.empty()) {
                    ImGui::TextUnformatted(inlineText.c_str());
                    ImGui::SameLine(0.0f, 0.0f);
                    inlineText.clear();
                }
                const char* stringStart = cursor++;
                while (cursor < end && *cursor != stringDelimiter) {
                    if (*cursor == '\\' && (cursor + 1 < end) && *(cursor + 1) == stringDelimiter) {
                        cursor++;
                    }
                    cursor++;
                }
                if (cursor < end) {
                    cursor++;
                }
                std::string stringToken(stringStart, cursor);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green for strings
                ImGui::TextUnformatted(stringToken.c_str());
                ImGui::PopStyleColor();
                ImGui::SameLine(0.0f, 0.0f);
                continue;
            }

            // Handle keywords
            const char* wordStart = cursor;
            while (cursor < end && (isalnum(*cursor) || *cursor == '_')) {
                cursor++;
            }
            std::string word(wordStart, cursor);
            if (IsLuaKeyword(word)) {
                if (!inlineText.empty()) {
                    ImGui::TextUnformatted(inlineText.c_str());
                    ImGui::SameLine(0.0f, 0.0f);
                    inlineText.clear();
                }
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.5f, 1.0f, 1.0f)); // Blue for keywords
                ImGui::TextUnformatted(word.c_str());
                ImGui::PopStyleColor();
                ImGui::SameLine(0.0f, 0.0f);
            } else {
                inlineText += word;
            }

            // Handle symbols
            if (cursor < end && !isalnum(*cursor) && *cursor != '_') {
                if (!inlineText.empty()) {
                    ImGui::TextUnformatted(inlineText.c_str());
                    ImGui::SameLine(0.0f, 0.0f);
                    inlineText.clear();
                }
                ImGui::TextUnformatted(std::string(1, *cursor).c_str());
                ImGui::SameLine(0.0f, 0.0f);
                cursor++;
            }
        }

        if (!inlineText.empty()) {
            ImGui::TextUnformatted(inlineText.c_str());
        }
        ImGui::NewLine();
    }

    ImGui::EndChild(); // End background overlay
    ImGui::PopStyleVar();

    // Editable content area
    ImGui::InputTextMultiline("##editor", m_EditorContent.data(), m_EditorContent.size(),
                              inputBoxSize, ImGuiInputTextFlags_AllowTabInput);

    ImGui::End();
}






void LuaEditorWindow::SetText(const std::string& text) {
    if (text.size() >= BUFFER_SIZE) {
        m_EditorContent.resize(text.size() + 1); // Resize if needed
    }
    std::copy(text.begin(), text.end(), m_EditorContent.begin());
    m_EditorContent[text.size()] = '\0'; // Ensure null termination
}

std::string LuaEditorWindow::GetText() const {
    return std::string(m_EditorContent.data());
}

void LuaEditorWindow::Clear() {
    std::fill(m_EditorContent.begin(), m_EditorContent.end(), '\0');
}

bool LuaEditorWindow::SaveToFile(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    file << m_EditorContent.data();
    return true;
}

bool LuaEditorWindow::LoadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    SetText(buffer.str());
    m_FilePath = filePath;
    return true;
}

