#pragma once

#include "../Rendering/FBO.h"
#include "../Rendering/Shader.h"
#include <glm/glm.hpp>
#include "../Engine/AssetManager.h"


// Forward-declare a texture loading function if you have it in a separate file.
// extern unsigned int LoadTexture(const char* filePath);

class RenderWindow
{
public:
    void Show(); // Called each frame to draw the ImGui window with the rendered texture

private:
    // Initialize VAO/VBO/EBO, load shaders/textures, etc.
    void InitGLResources();

    // Actually render the spinning cube into the FBO each frame
    void RenderSceneToFBO();

    // Offscreen render target
    FBO m_FBO;

    // Our unlit shader
    Shader m_Shader;

    // Keep track if we've initialized (so we only do so once)
    bool m_Initialized = false;

    // GL objects for the cube
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;

    // Spin
    float m_RotationAngle = 0.0f;

    // Track last known size (to recreate FBO if user resizes ImGui window)
    int m_LastWidth  = 0;
    int m_LastHeight = 0;

    // The loaded texture handle
    unsigned int m_TextureID = 0;
};
