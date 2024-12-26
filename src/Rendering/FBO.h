// src/Rendering/FBO.h

#pragma once
#include <GL/glew.h>
#include "imgui.h"

// A simple FBO wrapper
class FBO
{
public:
    FBO() = default;
    ~FBO() { Cleanup(); }

    // Create the FBO with a given size
    bool Create(int width, int height);

    // Cleanup
    void Cleanup();

    // Bind / Unbind
    void Bind();
    static void Unbind();

    // The texture ID to use in ImGui::Image()
    ImTextureID GetTextureID() const;

    // Size info
    int GetWidth() const  { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    GLuint m_FBO         = 0;
    GLuint m_TextureID   = 0;
    GLuint m_RBO         = 0;
    int    m_Width       = 0;
    int    m_Height      = 0;
};
