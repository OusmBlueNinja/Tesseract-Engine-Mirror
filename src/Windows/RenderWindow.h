#pragma once
#include "../Rendering/FBO.h"
#include "../Rendering/Shader.h"
#include <glm/glm.hpp>

class RenderWindow
{
public:
    void Show();  // Called per-frame

private:
    void InitGLResources();   // Create VAO/VBO/EBO + load shader
    void RenderSceneToFBO();  // Renders the spinning cube with our unlit shader

    FBO   m_FBO;
    float m_RotationAngle = 0.f;
    int   m_LastWidth     = 0;
    int   m_LastHeight    = 0;

    // VAO/VBO/EBO IDs
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;

    // Our unlit shader
    Shader m_Shader;
    bool m_Initialized = false;
};
