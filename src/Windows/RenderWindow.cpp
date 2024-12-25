// RenderWindow.cpp

#include "RenderWindow.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

// Include your AssetManager & Shader headers
#include "Engine/AssetManager.h"
#include "Rendering/Shader.h"

// Extern reference to our global (or extern) asset manager
extern AssetManager g_AssetManager;

// Example cube data (position + UVs)
static float g_CubeVertices[] =
{
    // FRONT (z=+1)
    -1.f, -1.f,  1.f,  0.f, 0.f,
     1.f, -1.f,  1.f,  1.f, 0.f,
     1.f,  1.f,  1.f,  1.f, 1.f,
    -1.f,  1.f,  1.f,  0.f, 1.f,

    // BACK (z=-1)
    -1.f, -1.f, -1.f,  1.f, 0.f,
     1.f, -1.f, -1.f,  0.f, 0.f,
     1.f,  1.f, -1.f,  0.f, 1.f,
    -1.f,  1.f, -1.f,  1.f, 1.f,

    // LEFT (x=-1)
    -1.f, -1.f, -1.f,  0.f, 0.f,
    -1.f, -1.f,  1.f,  1.f, 0.f,
    -1.f,  1.f,  1.f,  1.f, 1.f,
    -1.f,  1.f, -1.f,  0.f, 1.f,

    // RIGHT (x=+1)
     1.f, -1.f, -1.f,  1.f, 0.f,
     1.f, -1.f,  1.f,  0.f, 0.f,
     1.f,  1.f,  1.f,  0.f, 1.f,
     1.f,  1.f, -1.f,  1.f, 1.f,

    // TOP (y=+1)
    -1.f,  1.f, -1.f,  0.f, 0.f,
     1.f,  1.f, -1.f,  1.f, 0.f,
     1.f,  1.f,  1.f,  1.f, 1.f,
    -1.f,  1.f,  1.f,  0.f, 1.f,

    // BOTTOM (y=-1)
    -1.f, -1.f, -1.f,  1.f, 0.f,
     1.f, -1.f, -1.f,  0.f, 0.f,
     1.f, -1.f,  1.f,  0.f, 1.f,
    -1.f, -1.f,  1.f,  1.f, 1.f,
};

static unsigned int g_CubeIndices[] =
{
    // Front
    0,1,2,   2,3,0,
    // Back
    4,5,6,   6,7,4,
    // Left
    8,9,10,  10,11,8,
    // Right
    12,13,14, 14,15,12,
    // Top
    16,17,18, 18,19,16,
    // Bottom
    20,21,22, 22,23,20
};

void RenderWindow::Show()
{
    ImGui::Begin("OpenGL Output");

    ImVec2 size = ImGui::GetContentRegionAvail();
    int w = static_cast<int>(size.x);
    int h = static_cast<int>(size.y);

    if (!m_Initialized)
    {
        InitGLResources();
        m_Initialized = true;
    }

    // If there's space, render to the FBO, then show it as an ImGui image
    if (w > 0 && h > 0)
    {
        if (w != m_LastWidth || h != m_LastHeight)
        {
            m_FBO.Create(w, h);
            m_LastWidth  = w;
            m_LastHeight = h;
        }

        RenderSceneToFBO();
        ImGui::Image(m_FBO.GetTextureID(), size, ImVec2(0,0), ImVec2(1,1));
    }
    else
    {
        ImGui::Text("No space to render.");
    }

    ImGui::End();
}

void RenderWindow::InitGLResources()
{
    // ----------------------------------------------------
    // 1) Load SHADER from the asset manager
    // ----------------------------------------------------
    {
        void* shaderAsset = g_AssetManager.loadAsset(AssetType::SHADER, "assets/shaders/UnlitMaterial");
        if (!shaderAsset)
        {
            fprintf(stderr, "[RenderWindow] Failed to load shader via AssetManager.\n");
            return;
        }
        // Cast back to your Shader class
        m_ShaderPtr = static_cast<Shader*>(shaderAsset);
    }

    // ----------------------------------------------------
    // 2) Create VAO/VBO/EBO for the cube
    // ----------------------------------------------------
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_CubeVertices), g_CubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_CubeIndices), g_CubeIndices, GL_STATIC_DRAW);

    // Position = location 0, UV = location 1
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ----------------------------------------------------
    // 3) Load TEXTURE from the asset manager
    // ----------------------------------------------------
    {
        void* texAsset = g_AssetManager.loadAsset(AssetType::TEXTURE, "assets/textures/default.png");
        if (!texAsset)
        {
            fprintf(stderr, "[RenderWindow] Failed to load texture.\n");
        }
        else
        {
            // Cast from void* to GLuint
            m_TextureID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texAsset));
        }
    }
}

void RenderWindow::RenderSceneToFBO()
{
    m_RotationAngle += 0.5f; // spin per frame

    m_FBO.Bind();
    glViewport(0, 0, m_LastWidth, m_LastHeight);

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.15f, 0.2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our loaded shader
    if (m_ShaderPtr)
        m_ShaderPtr->Use();
    else
        return; // No shader? Can't render

    // Build MVP
    glm::mat4 model = glm::rotate(glm::mat4(1.0f),
                                  glm::radians(m_RotationAngle),
                                  glm::vec3(1.f, 1.f, 0.f));

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -5.f));
    float aspect    = (m_LastHeight != 0) ? (float)m_LastWidth / (float)m_LastHeight : 1.0f;
    glm::mat4 proj  = glm::perspective(glm::radians(45.f), aspect, 0.1f, 100.f);
    glm::mat4 mvp   = proj * view * model;

    // Pass MVP to the shader
    GLuint programID = m_ShaderPtr->GetProgramID();
    GLint mvpLoc = glGetUniformLocation(programID, "uMVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    // Bind the texture to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // If your shader has a uniform sampler2D named "uTexture"
    GLint texLoc = glGetUniformLocation(programID, "uTexture");
    glUniform1i(texLoc, 0);

    // Draw the cube
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_FBO.Unbind();
}
