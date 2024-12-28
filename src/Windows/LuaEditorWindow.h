#pragma once

#include <string>
#include "imgui.h"

class LuaEditorWindow {
public:
    LuaEditorWindow();
    ~LuaEditorWindow() = default;

    void Show();                          // Renders the Lua editor window
    void SetText(const std::string& text); // Sets the editor content
    std::string GetText() const;           // Gets the editor content
    void Clear();                          // Clears the editor content
    bool LoadFromFile(const std::string& filePath); // Loads content from a file
    bool SaveToFile(const std::string& filePath);   // Saves content to a file

private:
    
    std::vector<char> m_EditorContent; // Buffer for the editor's content
    static constexpr size_t BUFFER_SIZE = 1024 * 16; // 16 KB default size
    std::string m_FilePath;               // Path to the loaded file
    bool m_ScrollToBottom = false;        // Flag to auto-scroll to the bottom
};
