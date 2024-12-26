#pragma once

#include "../Rendering/FBO.h"
#include <glm/glm.hpp>

#include "Rendering/Shader.h" // 

class RenderWindow
{
public:
    void Show();

private:
    void InitGLResources();
    void RenderSceneToFBO();

    // Offscreen render target
    FBO m_FBO;

    // Keep track if we've initialized
    bool m_Initialized = false;

    // GL objects for the cube
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;

    // Spin
    float m_RotationAngle = 0.f;
    int   m_LastWidth     = 0;
    int   m_LastHeight    = 0;

    // The loaded texture
    unsigned int m_TextureID       = 0;

    // The loaded shader program (via AssetManager)
    Shader* m_ShaderPtr = nullptr; 
};
